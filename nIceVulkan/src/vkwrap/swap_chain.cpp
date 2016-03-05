#include "stdafx.h"
#include "vkwrap/swap_chain.h"
#include "util/setops.h"
#include "util/shortcuts.h"
#include <map>

using namespace std;

namespace nif
{
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

	swap_chain::swap_chain(const device &device, const win32_surface &surface, const HINSTANCE platformHandle, const HWND platformWindow) :
		device_(device),
		surface_(surface),
		width_(surface.capabilities().currentExtent().width()),
		height_(surface.capabilities().currentExtent().height()),
		depth_stencil_image_(width_, height_, device),
		depth_stencil_view_(depth_stencil_image_, device.depth_format(), vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil)
	{
		auto &formats = surface.formats();
		color_format_ = formats.size() == 1 && formats[0].format() == vk::Format::eUndefined ?
			vk::Format::eB8G8R8A8Unorm :
			formats[0].format();
		color_space_ = formats[0].colorSpace();
	}

	swap_chain::~swap_chain()
	{
	}

	void swap_chain::setup(command_buffer &cmdBuffer)
	{
		map<vk::PresentModeKHR, int> prefModes = {
			{ vk::PresentModeKHR::eVkPresentModeMailboxKhr, 3 },
			{ vk::PresentModeKHR::eVkPresentModeImmediateKhr, 2 },
			{ vk::PresentModeKHR::eVkPresentModeFifoKhr, 1 }
		};

		vk::PresentModeKHR swapchainPresentMode = set::from(surface_.present_modes())
			.order_by([&](const vk::PresentModeKHR x) { return prefModes[x]; })
			.last();

		const vk::SurfaceCapabilitiesKHR& surfCaps = surface_.capabilities();
		uint32_t imageCount = surfCaps.minImageCount() == surfCaps.maxImageCount() ?
			surfCaps.maxImageCount() :
			surfCaps.minImageCount() + 1;

		vk::SurfaceTransformFlagBitsKHR preTransform;
		if (surfCaps.supportedTransforms() & vk::SurfaceTransformFlagBitsKHR::eIdentity)
			preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
		else
			preTransform = surfCaps.currentTransform();

		vk::SwapchainCreateInfoKHR swapchainCI;
		swapchainCI.surface(surface_.handle());
		swapchainCI.minImageCount(imageCount);
		swapchainCI.imageFormat(color_format_);
		swapchainCI.imageColorSpace(color_space_);
		swapchainCI.imageExtent(vk::Extent2D(width_, height_));
		swapchainCI.imageUsage(vk::ImageUsageFlagBits::eColorAttachment);
		swapchainCI.preTransform(preTransform);
		swapchainCI.imageArrayLayers(1);
		swapchainCI.imageSharingMode(vk::SharingMode::eExclusive);
		swapchainCI.queueFamilyIndexCount(0);
		swapchainCI.pQueueFamilyIndices(nullptr);
		swapchainCI.presentMode(swapchainPresentMode);
		swapchainCI.clipped(VK_TRUE);
		swapchainCI.compositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

		vk_try(vk::createSwapchainKHR(device_.handle(), &swapchainCI, nullptr, &handle_));

		vk_try(vk::getSwapchainImagesKHR(device_.handle(), handle_, &image_count_, nullptr));
		vector<vk::Image> swapchainImages(image_count_);
		vk_try(vk::getSwapchainImagesKHR(device_.handle(), handle_, &image_count_, swapchainImages.data()));

		buffers_.reserve(image_count_);
		for (uint32_t i = 0; i < image_count_; i++) {
			buffers_.push_back(buffer(device_, swapchainImages[i], color_format_));
			cmdBuffer.setImageLayout(
				buffers_[i].image,
				vk::ImageAspectFlagBits::eColor,
				vk::ImageLayout::eUndefined,
				static_cast<vk::ImageLayout>(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR));
		}

		cmdBuffer.setImageLayout(depth_stencil_image_, vk::ImageAspectFlagBits::eDepth, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
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

	uint32_t swap_chain::image_count() const
	{
		return image_count_;
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
