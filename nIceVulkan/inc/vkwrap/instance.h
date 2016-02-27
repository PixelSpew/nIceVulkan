#pragma once
#include "vulkan/vk_cpp.h"
#include <string>
#include <vector>

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
		static const std::vector<const char*>& layers();

	private:
		vk::Instance handle_;
#ifdef _DEBUG
		vk::DebugReportCallbackEXT debug_report_;
		PFN_vkDestroyDebugReportCallbackEXT destroy_debug_report_;
#endif
	};
}
