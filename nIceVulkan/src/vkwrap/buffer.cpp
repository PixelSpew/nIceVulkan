#include "stdafx.h"
#include "vkwrap/buffer.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	ibuffer::ibuffer(const device &device, const vk::BufferUsageFlags flags, const void *data, const size_t size)
		: device_(device), size_(size)
	{
		handle_ = device.create_buffer(
			vk::BufferCreateInfo()
				.size(static_cast<vk::DeviceSize>(size))
				.usage(flags));

		gpumem_ = gpu_memory(
			device,
			device.get_buffer_memory_requirements(handle_),
			vk::MemoryPropertyFlagBits::eHostVisible,
			data);

		device.bind_buffer_memory(handle_, gpumem_.handle(), 0);
	}

	ibuffer::~ibuffer()
	{
		if (handle_)
			device_.destroy_buffer(handle_);
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
