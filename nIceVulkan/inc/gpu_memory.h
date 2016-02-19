#pragma once
#include "device.h"

namespace nif
{
	class gpu_memory
	{
	public:
		gpu_memory(const device &device, const vk::MemoryRequirements &memreqs, const vk::MemoryPropertyFlags &memtype, const void *data);
		~gpu_memory();
		vk::DeviceMemory handle() const;

	private:
		vk::DeviceMemory handle_;
		const device &device_;
	};
}
