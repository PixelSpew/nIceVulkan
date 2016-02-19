#pragma once
#include "swap_chain.h"

namespace nif
{
	class command_pool
	{
	public:
		command_pool(const swap_chain &swap);
		~command_pool();
		vk::CommandPool handle() const;
		const device& parent_device() const;

	private:
		vk::CommandPool handle_;
		const device &device_;
	};
}
