#include "stdafx.h"
#include "vkwrap/semaphore.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	semaphore::semaphore(const device &device) :
		device_(device)
	{
		handle_ = device.create_semaphore(vk::SemaphoreCreateInfo());
	}

	semaphore::~semaphore()
	{
		device_.destroy_semaphore(handle_);
	}

	vk::Semaphore semaphore::handle() const
	{
		return handle_;
	}
}
