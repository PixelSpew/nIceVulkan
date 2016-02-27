#pragma once
#include "vkwrap/device.h"

namespace nif
{
	class semaphore
	{
	public:
		explicit semaphore(const device &device);
		semaphore(const semaphore&) = delete;
		~semaphore();
		vk::Semaphore handle() const;

	private:
		vk::Semaphore handle_;
		const device &device_;
	};
}
