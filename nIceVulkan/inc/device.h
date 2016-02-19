#pragma once
#include "instance.h"

namespace nif
{
	class device
	{
	public:
		device(const instance &instance);
		~device();
		vk::Device handle() const;
		vk::PhysicalDevice physical_handle() const;
		const vk::PhysicalDeviceMemoryProperties& memory_properties() const;
		vk::Format depth_format() const;

	private:
		device(const device&);

		vk::Device handle_;
		vk::PhysicalDevice physical_handle_;
		vk::PhysicalDeviceMemoryProperties memory_properties_;
		vk::Format depth_format_;
	};
}