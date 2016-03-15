#include "stdafx.h"
#include "vkwrap/physical_device.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	physical_device::physical_device(vk::PhysicalDevice handle) :
		handle_(handle)
	{
	}

	vk::PhysicalDevice physical_device::handle() const
	{
		return handle_;
	}

	vk::Device physical_device::create_device(const vk::DeviceCreateInfo & createInfo) const
	{
		vk::Device ret;
		vk_try(handle_.createDevice(&createInfo, nullptr, &ret));
		return ret;
	}

	vk::FormatProperties physical_device::get_format_properties(vk::Format format) const
	{
		vk::FormatProperties ret;
		handle_.getFormatProperties(format, &ret);
		return ret;
	}

	vk::PhysicalDeviceMemoryProperties physical_device::get_memory_properties() const
	{
		vk::PhysicalDeviceMemoryProperties ret;
		handle_.getMemoryProperties(&ret);
		return ret;
	}

	vector<vk::QueueFamilyProperties> physical_device::get_queue_family_properties() const
	{
		uint32_t queueCount;
		handle_.getQueueFamilyProperties(&queueCount, nullptr);
		vector<vk::QueueFamilyProperties> ret(queueCount);
		handle_.getQueueFamilyProperties(&queueCount, ret.data());
		return ret;
	}

	vk::Bool32 physical_device::get_surface_support(const uint32_t queueFamilyIndex, const vk::SurfaceKHR surface) const
	{
		vk::Bool32 ret;
		vk_try(handle_.getSurfaceSupportKHR(queueFamilyIndex, surface, &ret));
		return ret;
	}

	vk::SurfaceCapabilitiesKHR physical_device::get_surface_capabilities(const vk::SurfaceKHR surface) const
	{
		vk::SurfaceCapabilitiesKHR ret;
		vk_try(handle_.getSurfaceCapabilitiesKHR(surface, &ret));
		return ret;
	}

	vector<vk::PresentModeKHR> physical_device::get_surface_present_modes(const vk::SurfaceKHR surface) const
	{
		uint32_t count;
		vk_try(handle_.getSurfacePresentModesKHR(surface, &count, nullptr));
		vector<vk::PresentModeKHR> ret(count);
		vk_try(handle_.getSurfacePresentModesKHR(surface, &count, ret.data()));
		return ret;
	}

	vector<vk::SurfaceFormatKHR> physical_device::get_surface_formats(const vk::SurfaceKHR surface) const
	{
		uint32_t count;
		vk_try(handle_.getSurfaceFormatsKHR(surface, &count, nullptr));
		vector<vk::SurfaceFormatKHR> ret(count);
		vk_try(handle_.getSurfaceFormatsKHR(surface, &count, ret.data()));
		return ret;
	}
}
