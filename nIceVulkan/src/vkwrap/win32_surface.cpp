#include "stdafx.h"
#include "vkwrap/surface_win32.h"
#include "util/setops.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	surface_win32::surface_win32(const device &device, const HINSTANCE platformHandle, const HWND platformWindow) :
		device_(device)
	{
		vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
		surfaceCreateInfo.hinstance(platformHandle);
		surfaceCreateInfo.hwnd(platformWindow);
		if (vk::createWin32SurfaceKHR(device.parent_instance().handle(), &surfaceCreateInfo, nullptr, &handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		//////////////

		vk::PhysicalDevice physicalHandle = device.physdevice().handle();
		const std::vector<vk::QueueFamilyProperties>& queueProps = device.physdevice().queue_props();

		queue_node_index_ = static_cast<uint32_t>(
			set::from(queueProps)
				.first_index([&](const vk::QueueFamilyProperties &x, uint32_t i) {
					vk::Bool32 supportsPresent;
					vk_try(vk::getPhysicalDeviceSurfaceSupportKHR(physicalHandle, i, handle_, &supportsPresent));
					return x.queueFlags() & vk::QueueFlagBits::eGraphics && supportsPresent;
				}));

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

	surface_win32::~surface_win32()
	{
		vk::destroySurfaceKHR(device_.parent_instance().handle(), handle_, nullptr);
	}

	vk::SurfaceKHR surface_win32::handle() const
	{
		return handle_;
	}

	const device& surface_win32::parent_device() const
	{
		return device_;
	}

	uint32_t surface_win32::queue_node_index() const
	{
		return queue_node_index_;
	}

	const vk::SurfaceCapabilitiesKHR& surface_win32::capabilities() const
	{
		return capabilities_;
	}

	const vector<vk::PresentModeKHR>& surface_win32::present_modes() const
	{
		return present_modes_;
	}

	const std::vector<vk::SurfaceFormatKHR>& surface_win32::formats() const
	{
		return formats_;
	}
}
