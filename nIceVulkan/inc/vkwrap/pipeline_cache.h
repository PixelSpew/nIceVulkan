#pragma once
#include "vkwrap/device.h"

namespace nif
{
	class pipeline_cache
	{
	public:
		explicit pipeline_cache(const device &device);
		pipeline_cache(const pipeline_cache&) = delete;
		~pipeline_cache();
		vk::PipelineCache handle() const;

	private:
		vk::PipelineCache handle_;
		const device &device_;
	};
}