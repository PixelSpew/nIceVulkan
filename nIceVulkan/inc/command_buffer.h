#pragma once
#include "command_pool.h"
#include "image.h"
#include "render_pass.h"
#include "pipeline_layout.h"
#include "descriptor_set.h"
#include "framebuffer.h"
#include "pipeline.h"

namespace nif
{
	class command_buffer
	{
	public:
		command_buffer(const command_pool &pool);
		~command_buffer();
		void begin();
		void end();
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

	private:
		vk::CommandBuffer handle_;
		vk::CommandBufferBeginInfo begin_info_;
		const command_pool &pool_;
	};
}
