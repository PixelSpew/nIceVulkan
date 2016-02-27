#pragma once
#include "vkwrap/device.h"

namespace nif
{
	class gpu_memory
	{
	public:
		/**
		* Creates an empty instance with a null handle
		*/
		gpu_memory();
		gpu_memory(const device &device, const vk::MemoryRequirements &memreqs, const vk::MemoryPropertyFlags &memtype, const void *data);
		gpu_memory(const gpu_memory&) = delete;
		gpu_memory(gpu_memory &&old);
		~gpu_memory();
		vk::DeviceMemory handle() const;

		gpu_memory& operator=(gpu_memory &&rhs);

	private:
		vk::DeviceMemory handle_;
		const device *device_;
	};
}
