#pragma once
#include "vulkan/vk_cpp.h"
#include <string>

namespace nif
{
	class instance
	{
	public:
		instance(const std::string &name);
		instance(const instance&) = delete;
		instance(instance &&old);
		~instance();
		vk::Instance handle() const;

	private:
		vk::Instance handle_;
	};
}
