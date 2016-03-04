#include "stdafx.h"
#include "vkwrap/semaphore.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	semaphore::semaphore(const device &device)
		: device_(device)
	{
		vk::SemaphoreCreateInfo createInfo;
		vk_try(vk::createSemaphore(device.handle(), &createInfo, nullptr, &handle_));
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
