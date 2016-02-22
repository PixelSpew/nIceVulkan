#pragma once
#include "vkwrap/device.h"

namespace nif
{
	class render_pass
	{
	public:
		render_pass(const device &device);
		render_pass(const render_pass&) = delete;
		~render_pass();
		vk::RenderPass handle() const;
		const device& parent_device() const;

	private:
		vk::RenderPass handle_;
		const device &device_;
	};
}
