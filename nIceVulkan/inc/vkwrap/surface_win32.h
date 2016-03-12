#pragma once
#include <Windows.h>
#include "vkwrap/device.h"

namespace nif
{
	class surface_win32
	{
	public:
		surface_win32(const device &device, const HINSTANCE platformHandle, const HWND platformWindow);
		surface_win32(const surface_win32&) = delete;
		~surface_win32();
		vk::SurfaceKHR handle() const;
		const device& parent_device() const;
		uint32_t queue_node_index() const;
		const vk::SurfaceCapabilitiesKHR& capabilities() const;
		const std::vector<vk::PresentModeKHR>& present_modes() const;
		const std::vector<vk::SurfaceFormatKHR>& formats() const;

	private:
		vk::SurfaceKHR handle_;
		const device &device_;
		uint32_t width_;
		uint32_t height_;
		uint32_t queue_node_index_;
		vk::SurfaceCapabilitiesKHR capabilities_;
		std::vector<vk::PresentModeKHR> present_modes_;
		std::vector<vk::SurfaceFormatKHR> formats_;
	};
}
