#pragma once
#include <Windows.h>
#include "vkwrap/device.h"

namespace nif
{
	class surface_win32
	{
	public:
		surface_win32(const instance &instance, const physical_device &physDevice, const HINSTANCE platformHandle, const HWND platformWindow);
		surface_win32(const surface_win32&) = delete;
		~surface_win32();
		vk::SurfaceKHR handle() const;
		uint32_t queue_node_index() const;
		vk::SurfaceCapabilitiesKHR get_capabilities() const;
		std::vector<vk::PresentModeKHR> get_present_modes() const;
		std::vector<vk::SurfaceFormatKHR> get_formats() const;

	private:
		vk::SurfaceKHR handle_;
		const instance &instance_;
		const physical_device &phys_device_;
		uint32_t width_;
		uint32_t height_;
		uint32_t queue_node_index_;
	};
}
