#pragma once
#include "vkwrap/surface_win32.h"

namespace nif
{
	class command_pool
	{
	public:
		explicit command_pool(const surface_win32 &surface);
		command_pool(const command_pool&) = delete;
		~command_pool();
		vk::CommandPool handle() const;
		const device& parent_device() const;

	private:
		vk::CommandPool handle_;
		const device &device_;
	};
}
