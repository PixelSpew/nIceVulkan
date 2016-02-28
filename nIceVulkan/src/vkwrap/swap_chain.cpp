#include "stdafx.h"
#include "vkwrap/swap_chain.h"

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
		// Get list of supported formats
		uint32_t formatCount;
		if (vk::getPhysicalDeviceSurfaceFormatsKHR(device.physical_handle(), surface_.handle(), &formatCount, nullptr) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		std::vector<vk::SurfaceFormatKHR> surfFormats(formatCount);
		if (vk::getPhysicalDeviceSurfaceFormatsKHR(device.physical_handle(), surface_.handle(), &formatCount, surfFormats.data()) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		// If the format list includes just one entry of VK_FORMAT_UNDEFINED,
		// the surface has no preferred format.  Otherwise, at least one
		// supported format will be returned.
		if (formatCount == 1 && surfFormats[0].format() == vk::Format::eUndefined)
			colorFormat = vk::Format::eB8G8R8A8Unorm;
		else
			colorFormat = surfFormats[0].format();
		colorSpace = surfFormats[0].colorSpace();
	}

	swap_chain::~swap_chain()
	{
	}

	void swap_chain::setup(command_buffer &cmdBuffer, uint32_t *width, uint32_t *height)
	{
		vk::SurfaceCapabilitiesKHR surfCaps;
		if (vk::getPhysicalDeviceSurfaceCapabilitiesKHR(cmdBuffer.parent_device().physical_handle(), surface_.handle(), &surfCaps) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		uint32_t presentModeCount;
		if (vk::getPhysicalDeviceSurfacePresentModesKHR(cmdBuffer.parent_device().physical_handle(), surface_.handle(), &presentModeCount, nullptr) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		vector<vk::PresentModeKHR> presentModes(presentModeCount);
		if (vk::getPhysicalDeviceSurfacePresentModesKHR(cmdBuffer.parent_device().physical_handle(), surface_.handle(), &presentModeCount, presentModes.data()) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		vk::Extent2D swapchainExtent;
		if (surfCaps.currentExtent().width() == -1)
		{
			swapchainExtent.width(*width);
			swapchainExtent.height(*height);
		}
		else
		{
			swapchainExtent = surfCaps.currentExtent();
			*width = surfCaps.currentExtent().width();
			*height = surfCaps.currentExtent().height();
		}

		vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eVkPresentModeFifoKhr;
		for (size_t i = 0; i < presentModeCount; i++)
		{
			if (presentModes[i] == vk::PresentModeKHR::eVkPresentModeMailboxKhr)
			{
				swapchainPresentMode = vk::PresentModeKHR::eVkPresentModeMailboxKhr;
				break;
			}
			if (swapchainPresentMode != vk::PresentModeKHR::eVkPresentModeMailboxKhr && presentModes[i] == vk::PresentModeKHR::eVkPresentModeImmediateKhr)
			{
				swapchainPresentMode = vk::PresentModeKHR::eVkPresentModeImmediateKhr;
			}
		}

		uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount() + 1;
		if ((surfCaps.maxImageCount() > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount()))
		{
			desiredNumberOfSwapchainImages = surfCaps.maxImageCount();
		}

		vk::SurfaceTransformFlagBitsKHR preTransform;
		if (surfCaps.supportedTransforms() & vk::SurfaceTransformFlagBitsKHR::eIdentity)
			preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
		else
			preTransform = surfCaps.currentTransform();

		vk::SwapchainCreateInfoKHR swapchainCI;
		swapchainCI.surface(surface_.handle());
		swapchainCI.minImageCount(desiredNumberOfSwapchainImages);
		swapchainCI.imageFormat(colorFormat);
		swapchainCI.imageColorSpace(colorSpace);
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

		if (vk::createSwapchainKHR(device_.handle(), &swapchainCI, nullptr, &swapChain) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		if (vk::getSwapchainImagesKHR(device_.handle(), swapChain, &image_count_, nullptr) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		vector<vk::Image> swapchainImages(image_count_);
		if (vk::getSwapchainImagesKHR(device_.handle(), swapChain, &image_count_, swapchainImages.data()) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		buffers_.reserve(image_count_);
		for (uint32_t i = 0; i < image_count_; i++)
		{
			buffers_.push_back(buffer(device_, swapchainImages[i], colorFormat));
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
		if (vk::acquireNextImageKHR(device_.handle(), swapChain, UINT64_MAX, semaphore.handle(), nullptr, &ret) != vk::Result::eVkSuccess)
			throw runtime_error("fail");
		return ret;
	}

	void swap_chain::queuePresent(const uint32_t currentBuffer) const
	{
		vk::PresentInfoKHR presentInfo;
		presentInfo.swapchainCount(1);
		presentInfo.pSwapchains(&swapChain);
		presentInfo.pImageIndices(&currentBuffer);
		if (vk::queuePresentKHR(device_.queue(), &presentInfo) != vk::Result::eVkSuccess)
			throw runtime_error("fail");
	}

	void swap_chain::cleanup()
	{
		buffers_.clear();
		vk::destroySwapchainKHR(device_.handle(), swapChain, nullptr);
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
