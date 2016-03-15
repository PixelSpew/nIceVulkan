#pragma once
#include "vulkan/vk_cpp.h"

namespace nif
{
	inline void vk_try(vk::Result result)
	{
		if (result != vk::Result::eSuccess)
			throw std::runtime_error("fail");
	}

	inline void vk_try(VkResult result)
	{
		vk_try(static_cast<vk::Result>(result));
	}
}
