#pragma once
#include "vkwrap/device.h"

namespace nif
{
	class win32_surface
	{
	public:
		win32_surface(const device &device, const HINSTANCE platformHandle, const HWND platformWindow);
		win32_surface(const win32_surface&) = delete;
		~win32_surface();
		vk::SurfaceKHR handle() const;
		const device& parent_device() const;
		uint32_t queue_node_index() const;
		const vk::SurfaceCapabilitiesKHR& capabilities() const;
		const std::vector<vk::PresentModeKHR>& present_modes() const;

	private:
		vk::SurfaceKHR handle_;
		const device &device_;
		uint32_t width_;
		uint32_t height_;
		uint32_t queue_node_index_;
		vk::SurfaceCapabilitiesKHR capabilities_;
		std::vector<vk::PresentModeKHR> present_modes_;
	};
}
