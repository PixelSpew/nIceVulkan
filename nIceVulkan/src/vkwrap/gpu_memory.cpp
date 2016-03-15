#include "stdafx.h"
#include "vkwrap/gpu_memory.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	gpu_memory::gpu_memory() :
		handle_(nullptr) {}

	gpu_memory::gpu_memory(
		const device &device,
		const vk::MemoryRequirements &memreqs,
		const vk::MemoryPropertyFlags &memtype,
		const void *data) :
		device_(&device)
	{
		vk::MemoryAllocateInfo mem_alloc;
		mem_alloc.allocationSize(memreqs.size());

		const vk::MemoryType *memoryTypes = device.physdevice().get_memory_properties().memoryTypes();
		for (uint32_t i = 0; i < 32; i++) {
			if (memreqs.memoryTypeBits() & 1 << i) {
				if (memoryTypes[i].propertyFlags() & memtype) {
					mem_alloc.memoryTypeIndex(i);
					break;
				}
			}
		}

		handle_ = device.allocate_memory(mem_alloc);

		if (data != nullptr) {
			void *memmap = device.map_memory(handle_, memreqs.size());
			memcpy(memmap, data, memreqs.size());
			device.unmap_memory(handle_);
		}
	}

	gpu_memory::gpu_memory(gpu_memory &&old) :
		handle_(old.handle_),
		device_(old.device_)
	{
		old.handle_ = nullptr;
	}

	gpu_memory::~gpu_memory()
	{
		if (handle_)
			device_->free_memory(handle_);
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
