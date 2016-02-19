#include "stdafx.h"
#include "win32_surface.h"

namespace nif
{
	win32_surface::win32_surface(const instance &instance, const HINSTANCE platformHandle, const HWND platformWindow)
		: instance_(instance)
	{
		vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo;
		surfaceCreateInfo.hinstance(platformHandle);
		surfaceCreateInfo.hwnd(platformWindow);
		vk::createWin32SurfaceKHR(instance.handle(), &surfaceCreateInfo, nullptr, &handle_);
	}

	win32_surface::~win32_surface()
	{
		vk::destroySurfaceKHR(instance_.handle(), handle_, nullptr);
	}

	vk::SurfaceKHR win32_surface::handle() const
	{
		return handle_;
	}
}
