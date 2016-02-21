#pragma once
#include "win32_surface.h"

namespace nif
{
	class command_pool
	{
		command_pool(const command_pool&) = delete;

	public:
		command_pool(const win32_surface &surface);
		~command_pool();
		vk::CommandPool handle() const;
		const device& parent_device() const;

	private:
		vk::CommandPool handle_;
		const device &device_;
	};
}
