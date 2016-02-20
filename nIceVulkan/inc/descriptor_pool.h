#pragma once
#include "device.h"

namespace nif
{
	class descriptor_pool
	{
	public:
		descriptor_pool(const device &device);
		~descriptor_pool();
		vk::DescriptorPool handle() const;
		const device& parent_device() const;

	private:
		vk::DescriptorPool handle_;
		const device &device_;
	};
}
