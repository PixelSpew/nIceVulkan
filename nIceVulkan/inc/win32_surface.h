#pragma once
#include "instance.h"

namespace nif
{
	class win32_surface
	{
		win32_surface(const win32_surface&) = delete;

	public:
		win32_surface(const instance &instance, const HINSTANCE platformHandle, const HWND platformWindow);
		~win32_surface();
		vk::SurfaceKHR handle() const;

	private:
		vk::SurfaceKHR handle_;
		const instance &instance_;
	};
}
