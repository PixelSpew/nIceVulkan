#include "stdafx.h"
#include "vkwrap/win32_surface.h"
#include "util/setops.h"
#include "util/shortcuts.h"

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

		vk::PhysicalDevice physicalHandle = device.physical_device().handle();
		const std::vector<vk::QueueFamilyProperties>& queueProps = device.physical_device().queue_props();

		queue_node_index_ = set::from(queueProps)
			.first_index([&](const vk::QueueFamilyProperties &x, uint32_t i) {
				vk::Bool32 supportsPresent;
				vk_try(vk::getPhysicalDeviceSurfaceSupportKHR(physicalHandle, i, handle_, &supportsPresent));
				return x.queueFlags() & vk::QueueFlagBits::eGraphics && supportsPresent;
			});

		/////////

		vk_try(vk::getPhysicalDeviceSurfaceCapabilitiesKHR(physicalHandle, handle_, &capabilities_));

		uint32_t presentModeCount;
		vk_try(vk::getPhysicalDeviceSurfacePresentModesKHR(physicalHandle, handle_, &presentModeCount, nullptr));
		present_modes_.resize(presentModeCount);
		vk_try(vk::getPhysicalDeviceSurfacePresentModesKHR(physicalHandle, handle_, &presentModeCount, present_modes_.data()));

		uint32_t formatCount;
		vk_try(vk::getPhysicalDeviceSurfaceFormatsKHR(physicalHandle, handle_, &formatCount, nullptr));
		formats_.resize(formatCount);
		vk_try(vk::getPhysicalDeviceSurfaceFormatsKHR(physicalHandle, handle_, &formatCount, formats_.data()));
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

	const vk::SurfaceCapabilitiesKHR& win32_surface::capabilities() const
	{
		return capabilities_;
	}

	const vector<vk::PresentModeKHR>& win32_surface::present_modes() const
	{
		return present_modes_;
	}

	const std::vector<vk::SurfaceFormatKHR>& win32_surface::formats() const
	{
		return formats_;
	}
}
