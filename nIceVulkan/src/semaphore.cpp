#include "stdafx.h"
#include "semaphore.h"

namespace nif
{
	semaphore::semaphore(const device &device)
		: device_(device)
	{
		vk::SemaphoreCreateInfo createInfo;
		vk::createSemaphore(device.handle(), &createInfo, nullptr, &handle_);
	}

	semaphore::~semaphore()
	{
		vk::destroySemaphore(device_.handle(), handle_, nullptr);
	}

	vk::Semaphore semaphore::handle() const
	{
		return handle_;
	}
}
