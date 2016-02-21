#pragma once
#include "command_buffer.h"

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
		void setup(command_buffer &cmdBuffer, uint32_t *width, uint32_t *height);
		vk::Result acquireNextImage(const semaphore &semaphore, uint32_t *currentBuffer);
		vk::Result queuePresent(uint32_t currentBuffer);
		void cleanup();

		const win32_surface& surface() const;
		uint32_t image_count() const;
		const std::vector<std::unique_ptr<buffer>>& buffers() const;
		const device& parent_device() const;

	private:
		win32_surface surface_;
		vk::Format colorFormat;
		vk::ColorSpaceKHR colorSpace;
		vk::SwapchainKHR swapChain = VK_NULL_HANDLE;

		uint32_t image_count_;
		std::vector<std::unique_ptr<buffer>> buffers_;
		const device &device_;
		const instance &instance_;
	};
}
