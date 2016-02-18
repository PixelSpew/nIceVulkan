#include "stdafx.h"
#include "buffer.h"

namespace nif
{
	ibuffer::ibuffer(const device &device, const vk::BufferUsageFlags flags, const void *data, const size_t size)
		: device_(device)
	{
		vk::BufferCreateInfo bufInfo;
		bufInfo.size(static_cast<vk::DeviceSize>(size));
		bufInfo.usage(flags);

		vk::createBuffer(device.handle(), &bufInfo, nullptr, &handle_);

		vk::MemoryRequirements memreq;
		vk::getBufferMemoryRequirements(device.handle(), handle_, &memreq);

		vk::MemoryAllocateInfo memAlloc;
		memAlloc.allocationSize(memreq.size());

		uint32_t typeBits = memreq.memoryTypeBits();
		for (uint32_t i = 0; i < 32; i++)
		{
			if (typeBits & 1)
			{
				if (device.memory_properties().memoryTypes()[i].propertyFlags() & vk::MemoryPropertyFlagBits::eHostVisible)
				{
					memAlloc.memoryTypeIndex(i);
					break;
				}
			}
			typeBits >>= 1;
		}

		vk::allocateMemory(device.handle(), &memAlloc, nullptr, &memhandle_);

		void *memmap;
		vk::mapMemory(device.handle(), memhandle_, 0, memAlloc.allocationSize(), 0, &memmap);
		memcpy(memmap, data, size);
		vk::unmapMemory(device.handle(), memhandle_);
		vk::bindBufferMemory(device.handle(), handle_, memhandle_, 0);
	}

	ibuffer::~ibuffer()
	{
		vk::freeMemory(device_.handle(), memhandle_, nullptr);
		vk::destroyBuffer(device_.handle(), handle_, nullptr);
	}

	std::vector<vk::VertexInputBindingDescription>& ibuffer::bind_descs()
	{
		return bind_descs_;
	}

	std::vector<vk::VertexInputAttributeDescription>& ibuffer::attrib_descs()
	{
		return attrib_descs_;
	}

	vk::PipelineVertexInputStateCreateInfo& ibuffer::pipeline_info()
	{
		return pipeline_info_;
	}
}
