#include "stdafx.h"
#include "vkwrap/semaphore.h"

using namespace std;

namespace nif
{
	semaphore::semaphore(const device &device)
		: device_(device)
	{
		vk::SemaphoreCreateInfo createInfo;
		if (vk::createSemaphore(device.handle(), &createInfo, nullptr, &handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");
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
