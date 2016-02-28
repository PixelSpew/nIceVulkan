#include "stdafx.h"
#include "vkwrap/win32_surface.h"

using namespace std;

namespace nif
{
	win32_surface::win32_surface(const device &device, const HINSTANCE platformHandle, const HWND platformWindow)
		: device_(device)
	{
		vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
		surfaceCreateInfo.hinstance(platformHandle);
		surfaceCreateInfo.hwnd(platformWindow);
		if (vk::createWin32SurfaceKHR(device.parent_instance().handle(), &surfaceCreateInfo, nullptr, &handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		//////////////

		uint32_t queueCount;
		vk::getPhysicalDeviceQueueFamilyProperties(device.physical_handle(), &queueCount, nullptr);

		std::vector<vk::QueueFamilyProperties> queueProps(queueCount);
		vk::getPhysicalDeviceQueueFamilyProperties(device.physical_handle(), &queueCount, queueProps.data());

		std::vector<vk::Bool32> supportsPresent(queueCount);
		for (uint32_t i = 0; i < queueCount; i++)
			if (vk::getPhysicalDeviceSurfaceSupportKHR(device.physical_handle(), i, handle_, &supportsPresent[i]) != vk::Result::eVkSuccess)
				throw runtime_error("fail");

		uint32_t graphicsQueueNodeIndex = UINT32_MAX;
		for (uint32_t i = 0; i < queueCount; i++)
		{
			if ((queueProps[i].queueFlags() & vk::QueueFlagBits::eGraphics) != 0)
			{
				if (graphicsQueueNodeIndex == UINT32_MAX)
					graphicsQueueNodeIndex = i;

				if (supportsPresent[i])
				{
					graphicsQueueNodeIndex = i;
					queue_node_index_ = i;
					break;
				}
			}
		}
		if (queue_node_index_ == UINT32_MAX)
		{
			for (uint32_t i = 0; i < queueCount; ++i)
			{
				if (supportsPresent[i])
				{
					queue_node_index_ = i;
					break;
				}
			}
		}

		if (graphicsQueueNodeIndex == UINT32_MAX || queue_node_index_ == UINT32_MAX || graphicsQueueNodeIndex != queue_node_index_)
			throw runtime_error("fail");
	}

	win32_surface::~win32_surface()
	{
		vk::destroySurfaceKHR(device_.parent_instance().handle(), handle_, nullptr);
	}

	vk::SurfaceKHR win32_surface::handle() const
	{
		return handle_;
	}

	const device& win32_surface::parent_device() const
	{
		return device_;
	}

	uint32_t win32_surface::queue_node_index() const
	{
		return queue_node_index_;
	}
}
