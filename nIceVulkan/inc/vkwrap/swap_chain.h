#pragma once
#include "vkwrap/command_buffer.h"

namespace nif
{
	class swap_chain
	{
	public:
		struct buffer
		{
			buffer(const device &device, const vk::Image imghandle, const vk::Format format);
			buffer(buffer &&old);

			image image;
			image_view view;
		};

	public:
		swap_chain(const device &device, const HINSTANCE platformHandle, const HWND platformWindow);
		swap_chain(const swap_chain&) = delete;
		~swap_chain();
		void setup(command_buffer &cmdBuffer, uint32_t *width, uint32_t *height);
		uint32_t acquireNextImage(const semaphore &semaphore, const uint32_t currentBuffer) const;
		void queuePresent(const uint32_t currentBuffer) const;
		void cleanup();

		const win32_surface& surface() const;
		uint32_t image_count() const;
		const std::vector<buffer>& buffers() const;
		const device& parent_device() const;

	private:
		win32_surface surface_;
		vk::Format color_format_;
		vk::ColorSpaceKHR color_space_;
		vk::SwapchainKHR handle_ = VK_NULL_HANDLE;

		uint32_t image_count_;
		std::vector<buffer> buffers_;
		const device &device_;
	};
}
