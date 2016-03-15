#include "stdafx.h"
#include "vkwrap/surface_win32.h"
#include "util/setops.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	surface_win32::surface_win32(const instance &instance, const physical_device &physDevice, const HINSTANCE platformHandle, const HWND platformWindow) :
		instance_(instance),
		phys_device_(physDevice)
	{
		handle_ = instance.create_win32_surface(
			vk::Win32SurfaceCreateInfoKHR()
				.hinstance(platformHandle)
				.hwnd(platformWindow));

		//////////////

		queue_node_index_ = static_cast<uint32_t>(
			set::from(physDevice.get_queue_family_properties())
				.first_index([&](const vk::QueueFamilyProperties &x, const uint32_t i) {
					return x.queueFlags() & vk::QueueFlagBits::eGraphics && physDevice.get_surface_support(i, handle_);
				}));
	}

	surface_win32::~surface_win32()
	{
		instance_.destroy_surface(handle_);
	}

	vk::SurfaceKHR surface_win32::handle() const
	{
		return handle_;
	}

	uint32_t surface_win32::queue_node_index() const
	{
		return queue_node_index_;
	}

	vk::SurfaceCapabilitiesKHR surface_win32::get_capabilities() const
	{
		return phys_device_.get_surface_capabilities(handle_);
	}

	std::vector<vk::PresentModeKHR> surface_win32::get_present_modes() const
	{
		return phys_device_.get_surface_present_modes(handle_);
	}

	std::vector<vk::SurfaceFormatKHR> surface_win32::get_formats() const
	{
		return phys_device_.get_surface_formats(handle_);
	}
}
