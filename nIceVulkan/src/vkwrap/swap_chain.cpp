#include "stdafx.h"
#include "vkwrap/swap_chain.h"
#include "util/setops.h"
#include "util/shortcuts.h"
#include <map>

using namespace std;

namespace nif
{
	namespace swap_chain_static
	{
		uint32_t min_image_count(const uint32_t surfmin, const uint32_t surfmax)
		{
			return surfmin == surfmax ? surfmin : surfmin + 1;
		}

		vk::Format image_format(vk::Format surfpref)
		{
			return surfpref == vk::Format::eUndefined ? vk::Format::eB8G8R8A8Unorm : surfpref;
		}

		vk::PresentModeKHR present_mode(const vector<vk::PresentModeKHR> &presentModes)
		{
			static map<vk::PresentModeKHR, int> modePref = {
				{ vk::PresentModeKHR::eMailbox, 3 },
				{ vk::PresentModeKHR::eImmediate, 2 },
				{ vk::PresentModeKHR::eFifo, 1 }
			};

			return set::from(presentModes)
				.order_by([&](const vk::PresentModeKHR x) { return modePref[x]; })
				.last();
		}

		vk::SurfaceTransformFlagBitsKHR pretransform(const vk::SurfaceCapabilitiesKHR &surfcaps)
		{
			return surfcaps.supportedTransforms() & vk::SurfaceTransformFlagBitsKHR::eIdentity ?
				vk::SurfaceTransformFlagBitsKHR::eIdentity :
				surfcaps.currentTransform();
		}
	};

#define S swap_chain_static

	swap_chain::buffer::buffer(const device &device, const vk::Image imghandle, const vk::Format format) :
		image(device, imghandle),
		view(image, format, vk::ImageAspectFlagBits::eColor)
	{
	}

	swap_chain::buffer::buffer(buffer &&old) :
		image(move(old.image)),
		view(move(old.view))
	{
	}

	swap_chain::swap_chain(const surface_win32 &surface, const command_pool &cmdpool) :
		surface_(surface),
		width_(surface.get_capabilities().currentExtent().width()),
		height_(surface.get_capabilities().currentExtent().height()),
		depth_stencil_image_(width_, height_, cmdpool.parent_device()),
		depth_stencil_view_(depth_stencil_image_, cmdpool.parent_device().depth_format(), vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil),
		renderpass_(cmdpool.parent_device())
	{
		handle_ = cmdpool.parent_device().create_swap_chain(
			vk::SwapchainCreateInfoKHR()
				.surface(surface_.handle())
				.minImageCount(S::min_image_count(surface_.get_capabilities().minImageCount(), surface_.get_capabilities().maxImageCount()))
				.imageFormat(S::image_format(surface.get_formats()[0].format()))
				.imageColorSpace(surface.get_formats()[0].colorSpace())
				.imageExtent(vk::Extent2D(width_, height_))
				.imageArrayLayers(1)
				.imageUsage(vk::ImageUsageFlagBits::eColorAttachment)
				.imageSharingMode(vk::SharingMode::eExclusive)
				.preTransform(S::pretransform(surface_.get_capabilities()))
				.compositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
				.presentMode(S::present_mode(surface.get_present_modes()))
				.clipped(VK_TRUE));

		buffers_ = set::from(cmdpool.parent_device().get_swap_chain_images(handle_))
			.select([&](vk::Image x) { return buffer(cmdpool.parent_device(), x, S::image_format(surface.get_formats()[0].format())); })
			.to_vector();

		command_buffer cmdbuf(cmdpool);
		cmdbuf.begin();

		for (const buffer &buf : buffers_) {
			cmdbuf.setImageLayout(
				buf.image,
				vk::ImageAspectFlagBits::eColor,
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::ePresentSrcKHR);
		}

		cmdbuf.setImageLayout(
			depth_stencil_image_, vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, vk::ImageLayout::eUndefined,
			vk::ImageLayout::eDepthStencilAttachmentOptimal);
		cmdbuf.end();
		cmdbuf.submit(cmdpool.parent_device(), {});
		cmdpool.parent_device().queue().wait_idle();

		framebuffers_ = set::from(buffers_)
			.select([&](const buffer &buf) { return framebuffer(width_, height_, renderpass_, { buf.view, depth_stencil_view_ }); })
			.to_vector();
	}

	swap_chain::~swap_chain()
	{
		renderpass_.parent_device().destroy_swap_chain(handle_);
	}

	uint32_t swap_chain::acquire_next_image(const semaphore &semaphore) const
	{
		return renderpass_.parent_device().acquire_next_image(handle_, semaphore.handle());
	}

	void swap_chain::queuePresent(const uint32_t currentBuffer) const
	{
		renderpass_.parent_device().queue().present(
			vk::PresentInfoKHR()
				.swapchainCount(1)
				.pSwapchains(&handle_)
				.pImageIndices(&currentBuffer));
	}

	const vector<swap_chain::buffer>& swap_chain::buffers() const
	{
		return buffers_;
	}

	const std::vector<framebuffer>& swap_chain::framebuffers() const
	{
		return framebuffers_;
	}

	const device& swap_chain::parent_device() const
	{
		return renderpass_.parent_device();
	}

	uint32_t swap_chain::width() const
	{
		return width_;
	}

	uint32_t swap_chain::height() const
	{
		return height_;
	}

	const image_view& swap_chain::depth_stencil_view() const
	{
		return depth_stencil_view_;
	}
}
