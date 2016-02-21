#include "stdafx.h"
#include "command_pool.h"

namespace nif
{
	command_pool::command_pool(const win32_surface &surface)
		: device_(surface.parent_device())
	{
		vk::CommandPoolCreateInfo cmdPoolInfo;
		cmdPoolInfo.queueFamilyIndex(surface.queue_node_index());
		cmdPoolInfo.flags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

		vk::createCommandPool(surface.parent_device().handle(), &cmdPoolInfo, nullptr, &handle_);
	}

	command_pool::~command_pool()
	{
		vk::destroyCommandPool(device_.handle(), handle_, nullptr);
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