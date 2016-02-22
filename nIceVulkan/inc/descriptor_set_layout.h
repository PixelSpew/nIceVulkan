#pragma once
#include "device.h"

namespace nif
{
	class descriptor_set_layout
	{
	public:
		descriptor_set_layout(const device &device);
		descriptor_set_layout(const descriptor_set_layout&) = delete;
		descriptor_set_layout(descriptor_set_layout &&old);
		~descriptor_set_layout();
		vk::DescriptorSetLayout handle() const;
		const device& parent_device() const;

	private:
		vk::DescriptorSetLayout handle_;
		const device &device_;
	};
}
