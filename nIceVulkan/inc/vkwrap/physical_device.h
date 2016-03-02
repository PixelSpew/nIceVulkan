#pragma once
#include "vulkan/vk_cpp.h"
#include <vector>

namespace nif
{
	class physical_device
	{
	public:
		physical_device(vk::PhysicalDevice handle);
		vk::PhysicalDevice handle() const;
		const std::vector<vk::QueueFamilyProperties>& queue_props() const;

	private:
		vk::PhysicalDevice handle_;
		std::vector<vk::QueueFamilyProperties> queue_props_;
	};
}