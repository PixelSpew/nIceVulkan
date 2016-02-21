#pragma once
#include "device.h"

namespace nif
{
	class semaphore
	{
	public:
		semaphore(const device &device);
		~semaphore();
		vk::Semaphore handle() const;

	private:
		vk::Semaphore handle_;
		const device &device_;
	};
}