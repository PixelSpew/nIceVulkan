#include "stdafx.h"
#include "vkwrap/buffer.h"

using namespace std;

namespace nif
{
	ibuffer::ibuffer(const device &device, const vk::BufferUsageFlags flags, const void *data, const size_t size)
		: device_(device), size_(size)
	{
		vk::BufferCreateInfo bufInfo;
		bufInfo.size(static_cast<vk::DeviceSize>(size));
		bufInfo.usage(flags);

		if (vk::createBuffer(device.handle(), &bufInfo, nullptr, &handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		vk::MemoryRequirements memreq;
		vk::getBufferMemoryRequirements(device.handle(), handle_, &memreq);
		gpumem_ = gpu_memory(device, memreq, vk::MemoryPropertyFlagBits::eHostVisible, data);
		if (vk::bindBufferMemory(device.handle(), handle_, gpumem_.handle(), 0) != vk::Result::eVkSuccess)
			throw runtime_error("fail");
	}

	ibuffer::~ibuffer()
	{
		if (handle_ != nullptr)
			vk::destroyBuffer(device_.handle(), handle_, nullptr);
	}

	ibuffer::ibuffer(ibuffer &&old) :
		device_(old.device_),
		handle_(old.handle_),
		gpumem_(move(old.gpumem_)),
		size_(old.size_)
	{
		old.handle_ = nullptr;
	}

	vk::Buffer ibuffer::handle() const
	{
		return handle_;
	}

	size_t ibuffer::size() const
	{
		return size_;
	}
}
