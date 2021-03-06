#pragma once
#include "vkwrap/device.h"

namespace nif
{
	class descriptor_pool
	{
	public:
		explicit descriptor_pool(const device &device);
		descriptor_pool(const descriptor_pool&) = delete;
		~descriptor_pool();
		vk::DescriptorPool handle() const;
		const device& parent_device() const;

	private:
		vk::DescriptorPool handle_;
		const device &device_;
	};
}
