#pragma once
#include "vulkan/vk_cpp.h"

namespace nif
{
	inline void vk_try(vk::Result result)
	{
		if (result != vk::Result::eVkSuccess)
			throw std::runtime_error("fail");
	}
}
