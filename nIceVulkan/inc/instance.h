#pragma once
#include "vulkan/vk_cpp.h"
#include <string>

namespace nif
{
	class instance
	{
		instance(const instance&) = delete;

	public:
		instance(const std::string &name);
		~instance();
		vk::Instance handle() const;

	private:
		vk::Instance handle_;
	};
}
