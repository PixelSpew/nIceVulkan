#pragma once
#include "device.h"

namespace nif
{
	class win32_surface
	{
		win32_surface(const win32_surface&) = delete;

	public:
		win32_surface(const device &device, const HINSTANCE platformHandle, const HWND platformWindow);
		~win32_surface();
		vk::SurfaceKHR handle() const;
		const device& parent_device() const;
		uint32_t queue_node_index() const;

	private:
		vk::SurfaceKHR handle_;
		const device &device_;
		uint32_t width_;
		uint32_t height_;
		uint32_t queue_node_index_;
	};
}
