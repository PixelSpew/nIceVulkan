#pragma once
#include "vkwrap/command_pool.h"
#include "vkwrap/descriptor_set.h"
#include "vkwrap/framebuffer.h"
#include "vkwrap/pipeline.h"
#include "vkwrap/semaphore.h"

namespace nif
{
	class command_buffer
	{
	public:
		explicit command_buffer(const command_pool &pool);
		command_buffer(const command_buffer&) = delete;
		command_buffer(command_buffer &&old);
		~command_buffer();
		void begin();
		void end();
		void submit(const device &device);
		void submit(const device &device, const semaphore &semaphore);
		void begin_render_pass(const render_pass &pass, const framebuffer &framebuffer, uint32_t width, uint32_t height);
		void end_render_pass();
		void set_viewport(const float width, const float height);
		void set_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height);
		void bind_descriptor_sets(const pipeline_layout &pipelayout, const descriptor_set &descset);
		void bind_pipeline(const pipeline &pipeline);
		void bind_vertex_buffer(const ibuffer &buffers);
		void bind_index_buffer(const buffer<uint32_t> &buffer);
		void draw_indexed(const uint32_t indexCount);
		void pipeline_barrier(const image &image);
		void setImageLayout(const image &image, const vk::ImageAspectFlags &aspectMask, const vk::ImageLayout oldImageLayout, const vk::ImageLayout newImageLayout);

		vk::CommandBuffer handle() const;
		const device& parent_device() const;

	private:
		vk::CommandBuffer handle_;
		vk::CommandBufferBeginInfo begin_info_;
		const command_pool &pool_;
	};
}
