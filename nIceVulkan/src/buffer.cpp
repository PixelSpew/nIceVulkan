#include "stdafx.h"
#include "buffer.h"
#include "gpu_memory.h"

using namespace std;

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
		gpumem_ = unique_ptr<gpu_memory>(new gpu_memory(device, memreq, vk::MemoryPropertyFlagBits::eHostVisible, data));
		vk::bindBufferMemory(device.handle(), handle_, gpumem_->handle(), 0);
	}

	ibuffer::~ibuffer()
	{
		vk::destroyBuffer(device_.handle(), handle_, nullptr);
	}

	std::vector<vk::VertexInputBindingDescription>& ibuffer::bind_descs()
	{
		return bind_descs_;
	}

	const std::vector<vk::VertexInputBindingDescription>& ibuffer::bind_descs() const
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

	vk::Buffer ibuffer::handle() const
	{
		return handle_;
	}
}
