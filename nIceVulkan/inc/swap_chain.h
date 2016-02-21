#pragma once
#include "image_view.h"
#include "win32_surface.h"
#include "vulkan/vk_cpp.h"
#include <vector>

namespace nif
{
	class swap_chain
	{
		swap_chain(const swap_chain&) = delete;

	public:
		struct buffer
		{
			buffer(const device &device, const vk::Image imghandle, const vk::Format format);

			image image;
			image_view view;
		};

	public:
		swap_chain(const instance &instance, const device &device, const HINSTANCE platformHandle, const HWND platformWindow);
		~swap_chain();
		void setImageLayout(vk::CommandBuffer cmdbuffer, vk::Image image, vk::ImageAspectFlags aspectMask, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout);
		void setup(VkCommandBuffer cmdBuffer, uint32_t *width, uint32_t *height);
		vk::Result acquireNextImage(vk::Semaphore presentCompleteSemaphore, uint32_t *currentBuffer);
		vk::Result queuePresent(vk::Queue queue, uint32_t currentBuffer);
		void cleanup();

		uint32_t image_count() const;
		uint32_t queue_node_index() const;
		const std::vector<std::unique_ptr<buffer>>& buffers() const;
		const device& parent_device() const;

	private:
		win32_surface surface_;
		vk::Format colorFormat;
		vk::ColorSpaceKHR colorSpace;
		vk::SwapchainKHR swapChain = VK_NULL_HANDLE;

		uint32_t image_count_;
		std::vector<std::unique_ptr<buffer>> buffers_;
		uint32_t queue_node_index_ = UINT32_MAX;
		const device &device_;
		const instance &instance_;
	};
}
