#include "stdafx.h"
#include "vkwrap/gpu_memory.h"

using namespace std;

namespace nif
{
	gpu_memory::gpu_memory()
		: handle_(nullptr) {}

	gpu_memory::gpu_memory(const device &device, const vk::MemoryRequirements &memreqs, const vk::MemoryPropertyFlags &memtype, const void *data)
		: device_(&device)
	{
		vk::MemoryAllocateInfo mem_alloc;
		mem_alloc.allocationSize(memreqs.size());

		uint32_t typeBits = memreqs.memoryTypeBits();
		for (uint32_t i = 0; i < 32; i++)
		{
			if (typeBits & 1)
			{
				if (device.memory_properties().memoryTypes()[i].propertyFlags() & memtype)
				{
					mem_alloc.memoryTypeIndex(i);
					break;
				}
			}
			typeBits >>= 1;
		}

		if (vk::allocateMemory(device.handle(), &mem_alloc, nullptr, &handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		if (data != nullptr)
		{
			void *memmap;
			if (vk::mapMemory(device.handle(), handle_, 0, memreqs.size(), 0, &memmap) != vk::Result::eVkSuccess)
				throw runtime_error("fail");
			memcpy(memmap, data, memreqs.size());
			vk::unmapMemory(device.handle(), handle_);
		}
	}

	gpu_memory::gpu_memory(gpu_memory &&old)
		: handle_(old.handle_),
		  device_(old.device_)
	{
		old.handle_ = nullptr;
	}

	gpu_memory::~gpu_memory()
	{
		if (handle_ != nullptr)
			vk::freeMemory(device_->handle(), handle_, nullptr);
	}

	vk::DeviceMemory gpu_memory::handle() const
	{
		return handle_;
	}

	gpu_memory& gpu_memory::operator=(gpu_memory &&rhs)
	{
		if (handle_ != rhs.handle_) {
			handle_ = rhs.handle_;
			device_ = rhs.device_;
			rhs.handle_ = nullptr;
		}

		return *this;
	}
}