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
		swap_chain(const win32_surface &surface, const command_pool &cmdpool);
		swap_chain(const swap_chain&) = delete;
		~swap_chain();
		uint32_t acquireNextImage(const semaphore &semaphore, const uint32_t currentBuffer) const;
		void queuePresent(const uint32_t currentBuffer) const;
		void cleanup();

		const device& parent_device() const;
		uint32_t width() const;
		uint32_t height() const;
		const image_view& depth_stencil_view() const;
		const std::vector<buffer>& buffers() const;
		const std::vector<framebuffer>& framebuffers() const;

	private:
		vk::SwapchainKHR handle_;
		const device &device_;
		const win32_surface &surface_;
		uint32_t width_;
		uint32_t height_;
		image depth_stencil_image_;
		image_view depth_stencil_view_;
		render_pass renderpass_;
		std::vector<buffer> buffers_;
		std::vector<framebuffer> framebuffers_;
	};
}
