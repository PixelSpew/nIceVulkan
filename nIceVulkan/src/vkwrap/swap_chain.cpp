#include "stdafx.h"
#include "vkwrap/swap_chain.h"
#include "util/setops.h"
#include <map>

using namespace std;

namespace nif
{
	swap_chain::buffer::buffer(const device &device, const vk::Image imghandle, const vk::Format format)
		: image(device, imghandle), view(image, format, vk::ImageAspectFlagBits::eColor)
	{
	}

	swap_chain::buffer::buffer(buffer &&old)
		: image(move(old.image)), view(move(old.view))
	{
	}

	swap_chain::swap_chain(const device &device, const HINSTANCE platformHandle, const HWND platformWindow)
		: device_(device), surface_(device, platformHandle, platformWindow)
	{
		vk::PhysicalDevice physicalHandle = device.physical_device().handle();

		uint32_t formatCount;
		if (vk::getPhysicalDeviceSurfaceFormatsKHR(physicalHandle, surface_.handle(), &formatCount, nullptr) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		std::vector<vk::SurfaceFormatKHR> surfFormats(formatCount);
		if (vk::getPhysicalDeviceSurfaceFormatsKHR(physicalHandle, surface_.handle(), &formatCount, surfFormats.data()) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		if (formatCount == 1 && surfFormats[0].format() == vk::Format::eUndefined)
			color_format_ = vk::Format::eB8G8R8A8Unorm;
		else
			color_format_ = surfFormats[0].format();
		color_space_ = surfFormats[0].colorSpace();
	}

	swap_chain::~swap_chain()
	{
	}

	void swap_chain::setup(command_buffer &cmdBuffer, uint32_t *width, uint32_t *height)
	{
		const vk::SurfaceCapabilitiesKHR& surfCaps = surface_.capabilities();
		vk::Extent2D swapchainExtent;
		if (surfCaps.currentExtent().width() == -1) {
			swapchainExtent.width(*width);
			swapchainExtent.height(*height);
		} else {
			swapchainExtent = surfCaps.currentExtent();
			*width = surfCaps.currentExtent().width();
			*height = surfCaps.currentExtent().height();
		}

		map<vk::PresentModeKHR, int> prefModes = {
			{ vk::PresentModeKHR::eVkPresentModeMailboxKhr, 3 },
			{ vk::PresentModeKHR::eVkPresentModeImmediateKhr, 2 },
			{ vk::PresentModeKHR::eVkPresentModeFifoKhr, 1 }
		};

		vk::PresentModeKHR swapchainPresentMode = set::from(surface_.present_modes())
			.order_by([&](const vk::PresentModeKHR x) { return prefModes[x]; })
			.last();

		uint32_t imageCount = surfCaps.minImageCount() + 1;
		if (surfCaps.maxImageCount() > 0 && imageCount > surfCaps.maxImageCount())
			imageCount = surfCaps.maxImageCount();

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
		swapchainCI.imageExtent(vk::Extent2D(swapchainExtent.width(), swapchainExtent.height()));
		swapchainCI.imageUsage(vk::ImageUsageFlagBits::eColorAttachment);
		swapchainCI.preTransform(preTransform);
		swapchainCI.imageArrayLayers(1);
		swapchainCI.imageSharingMode(vk::SharingMode::eExclusive);
		swapchainCI.queueFamilyIndexCount(0);
		swapchainCI.pQueueFamilyIndices(nullptr);
		swapchainCI.presentMode(swapchainPresentMode);
		swapchainCI.clipped(VK_TRUE);
		swapchainCI.compositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);

		if (vk::createSwapchainKHR(device_.handle(), &swapchainCI, nullptr, &handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		if (vk::getSwapchainImagesKHR(device_.handle(), handle_, &image_count_, nullptr) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		vector<vk::Image> swapchainImages(image_count_);
		if (vk::getSwapchainImagesKHR(device_.handle(), handle_, &image_count_, swapchainImages.data()) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		buffers_.reserve(image_count_);
		for (uint32_t i = 0; i < image_count_; i++) {
			buffers_.push_back(buffer(device_, swapchainImages[i], color_format_));
			cmdBuffer.setImageLayout(
				buffers_[i].image,
				vk::ImageAspectFlagBits::eColor,
				vk::ImageLayout::eUndefined,
				static_cast<vk::ImageLayout>(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR));
		}
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

	const win32_surface& swap_chain::surface() const
	{
		return surface_;
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
}
