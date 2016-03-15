#include "stdafx.h"
#include "vkwrap/command_pool.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	command_pool::command_pool(const device &device, const surface_win32 &surface) :
		device_(device)
	{
		handle_ = device.create_command_pool(
			vk::CommandPoolCreateInfo()
				.queueFamilyIndex(surface.queue_node_index())
				.flags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer));
	}

	command_pool::~command_pool()
	{
		device_.destroy_command_pool(handle_);
	}

	vk::CommandPool command_pool::handle() const
	{
		return handle_;
	}

	const device& command_pool::parent_device() const
	{
		return device_;
	}
}