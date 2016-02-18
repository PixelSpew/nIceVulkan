#pragma once
#include "vulkan/vk_cpp.h"
#include <string>

namespace nif
{
	class instance
	{
	public:
		instance(const std::string &name);
		~instance();
		vk::Instance handle() const;

	private:
		instance(const instance&);

		vk::Instance handle_;
	};
}
