#include "stdafx.h"
#include "swap_chain.h"
#include "util/linq.h"

using namespace std;

namespace nif
{
	swap_chain::buffer::buffer(const device &device, const vk::Image imghandle, const vk::Format format)
		: image(&image::wrap(device, imghandle)), view(*image, format, vk::ImageAspectFlagBits::eColor)
	{
	}

	swap_chain::swap_chain(const instance &instance, const device &device, const HINSTANCE platformHandle, const HWND platformWindow)
		: instance_(instance), device_(device), surface_(instance, platformHandle, platformWindow)
	{
		// Get queue properties
		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(device.physical_handle(), &queueCount, nullptr);

		std::vector<vk::QueueFamilyProperties> queueProps(queueCount);
		vk::getPhysicalDeviceQueueFamilyProperties(device.physical_handle(), &queueCount, queueProps.data());

		// Iterate over each queue to learn whether it supports presenting:
		std::vector<vk::Bool32> supportsPresent(queueCount);
		for (uint32_t i = 0; i < queueCount; i++)
			vk::getPhysicalDeviceSurfaceSupportKHR(device.physical_handle(), i, surface_.handle(), &supportsPresent[i]);

		// Search for a graphics and a present queue in the array of queue
		// families, try to find one that supports both
		uint32_t graphicsQueueNodeIndex = UINT32_MAX;
		uint32_t presentQueueNodeIndex = UINT32_MAX;
		for (uint32_t i = 0; i < queueCount; i++)
		{
			if ((queueProps[i].queueFlags() & vk::QueueFlagBits::eGraphics) != 0)
			{
				if (graphicsQueueNodeIndex == UINT32_MAX)
					graphicsQueueNodeIndex = i;

				if (supportsPresent[i])
				{
					graphicsQueueNodeIndex = i;
					presentQueueNodeIndex = i;
					break;
				}
			}
		}
		if (presentQueueNodeIndex == UINT32_MAX)
		{
			// If there's no queue that supports both present and graphics
			// try to find a separate present queue
			for (uint32_t i = 0; i < queueCount; ++i)
			{
				if (supportsPresent[i])
				{
					presentQueueNodeIndex = i;
					break;
				}
			}
		}

		// Generate error if could not find both a graphics and a present queue
		if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX)
		{
			// todo : error message
		}

		if (graphicsQueueNodeIndex != presentQueueNodeIndex)
		{
			// todo : error message
		}

		queue_node_index_ = presentQueueNodeIndex;

		// Get list of supported formats
		uint32_t formatCount;
		vk::getPhysicalDeviceSurfaceFormatsKHR(device.physical_handle(), surface_.handle(), &formatCount, nullptr);

		std::vector<vk::SurfaceFormatKHR> surfFormats(formatCount);
		vk::getPhysicalDeviceSurfaceFormatsKHR(device.physical_handle(), surface_.handle(), &formatCount, surfFormats.data());

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

	void swap_chain::setup(VkCommandBuffer cmdBuffer, uint32_t * width, uint32_t * height)
	{
		// Get physical device surface properties and formats
		vk::SurfaceCapabilitiesKHR surfCaps;
		vk::getPhysicalDeviceSurfaceCapabilitiesKHR(device_.physical_handle(), surface_.handle(), &surfCaps);

		uint32_t presentModeCount;
		vk::getPhysicalDeviceSurfacePresentModesKHR(device_.physical_handle(), surface_.handle(), &presentModeCount, NULL);

		// todo : replace with vector?
		vector<vk::PresentModeKHR> presentModes(presentModeCount);
		vk::getPhysicalDeviceSurfacePresentModesKHR(device_.physical_handle(), surface_.handle(), &presentModeCount, presentModes.data());

		vk::Extent2D swapchainExtent;
		// width and height are either both -1, or both not -1.
		if (surfCaps.currentExtent().width() == -1)
		{
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width(*width);
			swapchainExtent.height(*height);
		}
		else
		{
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = surfCaps.currentExtent();
			*width = surfCaps.currentExtent().width();
			*height = surfCaps.currentExtent().height();
		}

		// Try to use mailbox mode
		// Low latency and non-tearing
		vk::PresentModeKHR swapchainPresentMode = vk::PresentModeKHR::eVkPresentModeFifoKhr;
		for (size_t i = 0; i < presentModeCount; i++)
		{
			if (presentModes[i] == vk::PresentModeKHR::eVkPresentModeMailboxKhr)
			{
				swapchainPresentMode = vk::PresentModeKHR::eVkPresentModeMailboxKhr;
				break;
			}
			if ((swapchainPresentMode != vk::PresentModeKHR::eVkPresentModeMailboxKhr) && (presentModes[i] == vk::PresentModeKHR::eVkPresentModeImmediateKhr))
			{
				swapchainPresentMode = vk::PresentModeKHR::eVkPresentModeImmediateKhr;
			}
		}

		// Determine the number of images
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

		vk::SwapchainKHR oldSwapchain = swapChain;
		vk::SwapchainCreateInfoKHR swapchainCI;
		swapchainCI.surface(surface_.handle());
		swapchainCI.minImageCount(desiredNumberOfSwapchainImages);
		swapchainCI.imageFormat(colorFormat);
		swapchainCI.imageColorSpace(colorSpace);
		swapchainCI.imageExtent(swapchainExtent);
		swapchainCI.imageUsage(vk::ImageUsageFlagBits::eColorAttachment);
		swapchainCI.preTransform(preTransform);
		swapchainCI.imageArrayLayers(1);
		swapchainCI.imageSharingMode(vk::SharingMode::eExclusive);
		swapchainCI.presentMode(swapchainPresentMode);
		swapchainCI.oldSwapchain(oldSwapchain);
		swapchainCI.clipped(true);

		vk::createSwapchainKHR(device_.handle(), &swapchainCI, nullptr, &swapChain);

		// If we just re-created an existing swapchain, we should destroy the old
		// swapchain at this point.
		// Note: destroying the swapchain also cleans up all its associated
		// presentable images once the platform is done with them.
		if (oldSwapchain != VK_NULL_HANDLE)
		{
			vk::destroySwapchainKHR(device_.handle(), oldSwapchain, nullptr);
		}

		std::vector<vk::Image> imageHandles(image_count_);
		vk::getSwapchainImagesKHR(device_.handle(), swapChain, &image_count_, imageHandles.data());

		buffers_ = from(imageHandles)
			.select<buffer>([&](auto x) { return buffer(device_, x, colorFormat); })
			.to_vector();
	}

	vk::Result swap_chain::acquireNextImage(vk::Semaphore presentCompleteSemaphore, uint32_t * currentBuffer)
	{
		return vk::acquireNextImageKHR(device_.handle(), swapChain, UINT64_MAX, presentCompleteSemaphore, nullptr, currentBuffer);
	}

	vk::Result swap_chain::queuePresent(vk::Queue queue, uint32_t currentBuffer)
	{
		vk::PresentInfoKHR presentInfo;
		presentInfo.swapchainCount(1);
		presentInfo.pSwapchains(&swapChain);
		presentInfo.pImageIndices(&currentBuffer);
		return vk::queuePresentKHR(queue, &presentInfo);
	}

	void swap_chain::cleanup()
	{
		buffers_.clear();
		vk::destroySwapchainKHR(device_.handle(), swapChain, nullptr);
	}

	uint32_t swap_chain::image_count() const
	{
		return image_count_;
	}

	uint32_t swap_chain::queue_node_index() const
	{
		return queue_node_index_;
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
