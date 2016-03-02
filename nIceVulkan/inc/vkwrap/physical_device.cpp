#include "stdafx.h"
#include "physical_device.h"

using namespace std;

namespace nif
{
	physical_device::physical_device(vk::PhysicalDevice handle) :
		handle_(handle)
	{
		uint32_t queueCount;
		vk::getPhysicalDeviceQueueFamilyProperties(handle, &queueCount, nullptr);
		queue_props_.resize(queueCount);
		vk::getPhysicalDeviceQueueFamilyProperties(handle_, &queueCount, queue_props_.data());

		vk::getPhysicalDeviceMemoryProperties(handle_, &memory_properties_);
	}

	vk::PhysicalDevice physical_device::handle() const
	{
		return handle_;
	}

	const std::vector<vk::QueueFamilyProperties>& physical_device::queue_props() const
	{
		return queue_props_;
	}

	const vk::PhysicalDeviceMemoryProperties& physical_device::memory_properties() const
	{
		return memory_properties_;
	}

	vk::FormatProperties physical_device::query_format_properties(vk::Format format) const
	{
		vk::FormatProperties ret;
		vk::getPhysicalDeviceFormatProperties(handle_, format, &ret);
		return ret;
	}
}
