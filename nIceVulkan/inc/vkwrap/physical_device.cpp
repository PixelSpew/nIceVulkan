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
	}

	vk::PhysicalDevice physical_device::handle() const
	{
		return handle_;
	}

	const std::vector<vk::QueueFamilyProperties>& physical_device::queue_props() const
	{
		return queue_props_;
	}
}
