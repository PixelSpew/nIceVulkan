#include "stdafx.h"
#include "device.h"
#include <vector>

using namespace std;

namespace nif
{
	device::device(const instance &instance)
	{
		uint32_t gpuCount;
		vk::enumeratePhysicalDevices(instance.handle(), &gpuCount, &physical_handle_);

		uint32_t queueCount;
		vk::getPhysicalDeviceQueueFamilyProperties(physical_handle_, &queueCount, nullptr);
		vector<vk::QueueFamilyProperties> queueProps;
		queueProps.resize(queueCount);
		vk::getPhysicalDeviceQueueFamilyProperties(physical_handle_, &queueCount, queueProps.data());

		uint32_t graphicsQueueIndex = 0;
		for (graphicsQueueIndex = 0; graphicsQueueIndex < queueCount; graphicsQueueIndex++)
		{
			if (queueProps[graphicsQueueIndex].queueFlags() & VK_QUEUE_GRAPHICS_BIT)
				break;
		}

		vector<float> queuePriorities = { 0.0f };
		vk::DeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo.queueFamilyIndex(graphicsQueueIndex);
		queueCreateInfo.queueCount(static_cast<uint32_t>(queuePriorities.size()));
		queueCreateInfo.pQueuePriorities(queuePriorities.data());

		vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		vk::DeviceCreateInfo deviceCreateInfo;
		deviceCreateInfo.queueCreateInfoCount(1);
		deviceCreateInfo.pQueueCreateInfos(&queueCreateInfo);
		deviceCreateInfo.enabledExtensionCount(static_cast<uint32_t>(extensions.size()));
		deviceCreateInfo.ppEnabledExtensionNames(extensions.data());

		vk::createDevice(physical_handle_, &deviceCreateInfo, nullptr, &handle_);
	}

	device::~device()
	{
		vk::destroyDevice(handle_, nullptr);
	}

	vk::Device device::handle() const
	{
		return handle_;
	}

	vk::PhysicalDevice device::physical_handle() const
	{
		return physical_handle_;
	}

	const vk::PhysicalDeviceMemoryProperties& device::memory_properties() const
	{
		return memory_properties_;
	}
}
