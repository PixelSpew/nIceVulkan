#include "stdafx.h"
#include "swap_chain.h"

using namespace std;
using namespace nif;

namespace nif
{
	swap_chain::swap_chain(const instance &instance, const device &device, HINSTANCE platformHandle, HWND platformWindow)
		: instance_(instance), device_(device)
	{
		vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
		surfaceCreateInfo.hinstance(platformHandle);
		surfaceCreateInfo.hwnd(platformWindow);
		vk::createWin32SurfaceKHR(instance.handle(), &surfaceCreateInfo, nullptr, &surface);

		// Get queue properties
		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(device.physical_handle(), &queueCount, NULL);

		std::vector<vk::QueueFamilyProperties> queueProps(queueCount);
		vk::getPhysicalDeviceQueueFamilyProperties(device.physical_handle(), &queueCount, queueProps.data());

		// Iterate over each queue to learn whether it supports presenting:
		std::vector<vk::Bool32> supportsPresent(queueCount);
		for (uint32_t i = 0; i < queueCount; i++)
			vk::getPhysicalDeviceSurfaceSupportKHR(device.physical_handle(), i, surface, &supportsPresent[i]);

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

		queueNodeIndex = presentQueueNodeIndex;

		// Get list of supported formats
		uint32_t formatCount;
		vk::getPhysicalDeviceSurfaceFormatsKHR(device.physical_handle(), surface, &formatCount, nullptr);

		std::vector<vk::SurfaceFormatKHR> surfFormats(formatCount);
		vk::getPhysicalDeviceSurfaceFormatsKHR(device.physical_handle(), surface, &formatCount, surfFormats.data());

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

	vk::ImageMemoryBarrier imageMemoryBarrier()
	{
		vk::ImageMemoryBarrier imageMemoryBarrier;
		imageMemoryBarrier.srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		imageMemoryBarrier.dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		return imageMemoryBarrier;
	}

	void setImageLayout(vk::CommandBuffer cmdbuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout)
	{
		// Create an image barrier object
		vk::ImageMemoryBarrier imgMemBarrier = imageMemoryBarrier();
		imgMemBarrier.oldLayout(oldImageLayout);
		imgMemBarrier.newLayout(newImageLayout);
		imgMemBarrier.image(image);
		imgMemBarrier.subresourceRange(vk::ImageSubresourceRange(aspectMask, 0, 1, 0, 1));

		// Source layouts (old)

		// Undefined layout
		// Only allowed as initial layout!
		// Make sure any writes to the image have been finished
		if (oldImageLayout == vk::ImageLayout::eUndefined)
			imgMemBarrier.srcAccessMask(vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite);

		// Old layout is color attachment
		// Make sure any writes to the color buffer have been finished
		if (oldImageLayout == vk::ImageLayout::eColorAttachmentOptimal)
			imgMemBarrier.srcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);

		// Old layout is transfer source
		// Make sure any reads from the image have been finished
		if (oldImageLayout == vk::ImageLayout::eTransferSrcOptimal)
			imgMemBarrier.srcAccessMask(vk::AccessFlagBits::eTransferRead);

		// Old layout is shader read (sampler, input attachment)
		// Make sure any shader reads from the image have been finished
		if (oldImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
			imgMemBarrier.srcAccessMask(vk::AccessFlagBits::eShaderRead);

		// Target layouts (new)

		// New layout is transfer destination (copy, blit)
		// Make sure any copyies to the image have been finished
		if (newImageLayout == vk::ImageLayout::eTransferDstOptimal)
			imgMemBarrier.dstAccessMask(vk::AccessFlagBits::eTransferWrite);

		// New layout is transfer source (copy, blit)
		// Make sure any reads from and writes to the image have been finished
		if (newImageLayout == vk::ImageLayout::eTransferSrcOptimal)
		{
			imgMemBarrier.srcAccessMask(imgMemBarrier.srcAccessMask() | vk::AccessFlagBits::eTransferRead);
			imgMemBarrier.dstAccessMask(vk::AccessFlagBits::eTransferRead);
		}

		// New layout is color attachment
		// Make sure any writes to the color buffer hav been finished
		if (newImageLayout == vk::ImageLayout::eColorAttachmentOptimal)
		{
			imgMemBarrier.dstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
			imgMemBarrier.srcAccessMask(vk::AccessFlagBits::eTransferRead);
		}

		// New layout is depth attachment
		// Make sure any writes to depth/stencil buffer have been finished
		if (newImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
			imgMemBarrier.dstAccessMask(imgMemBarrier.dstAccessMask() | vk::AccessFlagBits::eDepthStencilAttachmentWrite);

		// New layout is shader read (sampler, input attachment)
		// Make sure any writes to the image have been finished
		if (newImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			imgMemBarrier.srcAccessMask(vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite);
			imgMemBarrier.dstAccessMask(vk::AccessFlagBits::eShaderRead);
		}


		// Put barrier on top
		vk::PipelineStageFlags srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
		vk::PipelineStageFlags destStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;

		// Put barrier inside setup command buffer
		vk::cmdPipelineBarrier(cmdbuffer, srcStageFlags, destStageFlags, vk::DependencyFlagBits::eByRegion, 0, nullptr, 0, nullptr, 1, &imgMemBarrier);
	}

	void swap_chain::setup(VkCommandBuffer cmdBuffer, uint32_t * width, uint32_t * height)
	{
		// Get physical device surface properties and formats
		vk::SurfaceCapabilitiesKHR surfCaps;
		vk::getPhysicalDeviceSurfaceCapabilitiesKHR(device_.physical_handle(), surface, &surfCaps);

		uint32_t presentModeCount;
		vk::getPhysicalDeviceSurfacePresentModesKHR(device_.physical_handle(), surface, &presentModeCount, NULL);

		// todo : replace with vector?
		vector<vk::PresentModeKHR> presentModes(presentModeCount);
		vk::getPhysicalDeviceSurfacePresentModesKHR(device_.physical_handle(), surface, &presentModeCount, presentModes.data());

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
		swapchainCI.surface(surface);
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

		vk::getSwapchainImagesKHR(device_.handle(), swapChain, &imageCount, NULL);

		swapchainImages.resize(imageCount);
		vk::getSwapchainImagesKHR(device_.handle(), swapChain, &imageCount, swapchainImages.data());

		buffers.resize(imageCount);
		for (uint32_t i = 0; i < imageCount; i++)
		{
			vk::ImageViewCreateInfo colorAttachmentView;
			colorAttachmentView.format(colorFormat);
			colorAttachmentView.components(vk::ComponentMapping(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA));
			colorAttachmentView.subresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
			colorAttachmentView.viewType(vk::ImageViewType::e2D);

			buffers[i].image = swapchainImages[i];

			setImageLayout(cmdBuffer, buffers[i].image, vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, static_cast<vk::ImageLayout>(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR));

			colorAttachmentView.image(buffers[i].image);

			vk::createImageView(device_.handle(), &colorAttachmentView, nullptr, &buffers[i].view);
		}
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
		for (uint32_t i = 0; i < imageCount; i++)
		{
			vk::destroyImageView(device_.handle(), buffers[i].view, nullptr);
		}
		vk::destroySwapchainKHR(device_.handle(), swapChain, nullptr);
		vk::destroySurfaceKHR(instance_.handle(), surface, nullptr);
	}
}
