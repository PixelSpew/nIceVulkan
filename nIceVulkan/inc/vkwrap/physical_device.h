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

		vk::Device create_device(const vk::DeviceCreateInfo &createInfo) const;
		vk::FormatProperties get_format_properties(vk::Format format) const;
		vk::PhysicalDeviceMemoryProperties get_memory_properties() const;
		std::vector<vk::QueueFamilyProperties> get_queue_family_properties() const;
		vk::Bool32 get_surface_support(const uint32_t queueFamilyIndex, const vk::SurfaceKHR surface) const;
		vk::SurfaceCapabilitiesKHR get_surface_capabilities(const vk::SurfaceKHR surface) const;
		std::vector<vk::PresentModeKHR> get_surface_present_modes(const vk::SurfaceKHR surface) const;
		std::vector<vk::SurfaceFormatKHR> get_surface_formats(const vk::SurfaceKHR surface) const;

	private:
		vk::PhysicalDevice handle_;
		vk::FormatProperties format_props_;
	};
}
