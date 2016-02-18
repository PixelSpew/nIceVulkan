#pragma once
#include "device.h"
#include "vulkan/vk_cpp.h"
#include <vector>

namespace nif
{
	struct swap_chain_buffer
	{
		vk::Image image;
		vk::ImageView view;
	};

	class swap_chain
	{
	public:
		swap_chain(const instance &instance, const device &device, HINSTANCE platformHandle, HWND platformWindow);
		~swap_chain();
		void setup(VkCommandBuffer cmdBuffer, uint32_t *width, uint32_t *height);
		vk::Result acquireNextImage(vk::Semaphore presentCompleteSemaphore, uint32_t *currentBuffer);
		vk::Result queuePresent(vk::Queue queue, uint32_t currentBuffer);
		void cleanup();

	private:
		vk::SurfaceKHR surface;
		vk::Format colorFormat;
		vk::ColorSpaceKHR colorSpace;
		std::vector<vk::Image> swapchainImages;
		vk::SwapchainKHR swapChain = VK_NULL_HANDLE;

		uint32_t imageCount;
		std::vector<swap_chain_buffer> buffers;
		uint32_t queueNodeIndex = UINT32_MAX;
		const device &device_;
		const instance &instance_;
	};
}
