#pragma once
#include "vkwrap/physical_device.h"
#include <string>

namespace nif
{
	class instance
	{
	public:
		explicit instance(const std::string &name);
		instance(const instance&) = delete;
		instance(instance &&old);
		~instance();

#ifdef VK_USE_PLATFORM_WIN32_KHR
		const vk::SurfaceKHR create_win32_surface(const vk::Win32SurfaceCreateInfoKHR &createInfo) const;
#endif
		void destroy_surface(const vk::SurfaceKHR surface) const;

		template<typename T>
		T get_proc_addr(const std::string &name);
		const std::vector<physical_device> enumerate_physical_devices() const;

		vk::Instance handle() const;

		static const std::vector<const char*>& layers();

	private:
		vk::Instance handle_;
#ifdef _DEBUG
		VkDebugReportCallbackEXT debug_report_;
#endif
	};

	template<typename T>
	T instance::get_proc_addr(const std::string & name)
	{
		return reinterpret_cast<T>(handle_.getProcAddr(name.c_str()));
	}
}
