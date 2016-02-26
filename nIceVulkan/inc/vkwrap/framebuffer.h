#pragma once
#include "vkwrap/image_view.h"
#include "vkwrap/render_pass.h"

namespace nif
{
	class framebuffer
	{
	public:
		framebuffer(const uint32_t width, const uint32_t height, const render_pass &pass, const std::initializer_list<std::reference_wrapper<const image_view>> views);
		framebuffer(const framebuffer&) = delete;
		framebuffer(framebuffer &&old);
		~framebuffer();
		vk::Framebuffer handle() const;

	private:
		vk::Framebuffer handle_;
		const device &device_;
	};
}
