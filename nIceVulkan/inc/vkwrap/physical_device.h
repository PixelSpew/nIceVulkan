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
		const vk::PhysicalDeviceMemoryProperties& memory_properties() const;
		vk::FormatProperties query_format_properties(vk::Format format) const;

	private:
		vk::PhysicalDevice handle_;
		std::vector<vk::QueueFamilyProperties> queue_props_;
		vk::PhysicalDeviceMemoryProperties memory_properties_;
		vk::FormatProperties format_props_;
	};
}