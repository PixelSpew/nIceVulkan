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

		vk::Instance handle() const;
		const std::vector<physical_device>& physical_devices() const;

		static const std::vector<const char*>& layers();

	private:
		vk::Instance handle_;
		std::vector<physical_device> physical_devices_;
#ifdef _DEBUG
		vk::DebugReportCallbackEXT debug_report_;
		PFN_vkDestroyDebugReportCallbackEXT destroy_debug_report_;
#endif
	};
}
