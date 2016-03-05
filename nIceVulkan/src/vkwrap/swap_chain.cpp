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
				{ vk::PresentModeKHR::eVkPresentModeMailboxKhr, 3 },
				{ vk::PresentModeKHR::eVkPresentModeImmediateKhr, 2 },
				{ vk::PresentModeKHR::eVkPresentModeFifoKhr, 1 }
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

	swap_chain::swap_chain(const win32_surface &surface, const command_pool &cmdpool) :
		device_(surface.parent_device()),
		surface_(surface),
		width_(surface.capabilities().currentExtent().width()),
		height_(surface.capabilities().currentExtent().height()),
		depth_stencil_image_(width_, height_, surface.parent_device()),
		depth_stencil_view_(depth_stencil_image_, surface.parent_device().depth_format(), vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil)
	{
		vk::SwapchainCreateInfoKHR swapchainCI(
			0,
			surface_.handle(),
			S::min_image_count(surface_.capabilities().minImageCount(), surface_.capabilities().maxImageCount()),
			S::image_format(surface.formats()[0].format()),
			surface.formats()[0].colorSpace(),
			vk::Extent2D(width_, height_),
			1,
			vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive, 0, nullptr,
			S::pretransform(surface_.capabilities()),
			vk::CompositeAlphaFlagBitsKHR::eOpaque,
			S::present_mode(surface.present_modes()),
			VK_TRUE,
			nullptr);
		vk_try(vk::createSwapchainKHR(device_.handle(), &swapchainCI, nullptr, &handle_));

		uint32_t imageCount;
		vk_try(vk::getSwapchainImagesKHR(device_.handle(), handle_, &imageCount, nullptr));
		vector<vk::Image> swapchainImages(imageCount);
		vk_try(vk::getSwapchainImagesKHR(device_.handle(), handle_, &imageCount, swapchainImages.data()));

		buffers_ = set::from(swapchainImages)
			.select([&](vk::Image x) { return buffer(device_, x, swapchainCI.imageFormat()); })
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
			depth_stencil_image_, vk::ImageAspectFlagBits::eDepth, vk::ImageLayout::eUndefined,
			vk::ImageLayout::eDepthStencilAttachmentOptimal);
		cmdbuf.end();
		cmdbuf.submit(surface.parent_device());
		surface.parent_device().wait_queue_idle();
	}

	swap_chain::~swap_chain()
	{
	}

	uint32_t swap_chain::acquireNextImage(const semaphore &semaphore, const uint32_t currentBuffer) const
	{
		uint32_t ret = currentBuffer;
		if (vk::acquireNextImageKHR(device_.handle(), handle_, UINT64_MAX, semaphore.handle(), nullptr, &ret) != vk::Result::eVkSuccess)
			throw runtime_error("fail");
		return ret;
	}

	void swap_chain::queuePresent(const uint32_t currentBuffer) const
	{
		vk::PresentInfoKHR presentInfo;
		presentInfo.swapchainCount(1);
		presentInfo.pSwapchains(&handle_);
		presentInfo.pImageIndices(&currentBuffer);
		if (vk::queuePresentKHR(device_.queue(), &presentInfo) != vk::Result::eVkSuccess)
			throw runtime_error("fail");
	}

	void swap_chain::cleanup()
	{
		buffers_.clear();
		vk::destroySwapchainKHR(device_.handle(), handle_, nullptr);
	}

	const vector<swap_chain::buffer>& swap_chain::buffers() const
	{
		return buffers_;
	}

	const device& swap_chain::parent_device() const
	{
		return device_;
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
