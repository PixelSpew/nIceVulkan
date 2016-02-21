#pragma once
#include "device.h"

namespace nif
{
	class pipeline_cache
	{
		pipeline_cache(const pipeline_cache&) = delete;

	public:
		pipeline_cache(const device &device);
		~pipeline_cache();
		vk::PipelineCache handle() const;

	private:
		vk::PipelineCache handle_;
		const device &device_;
	};
}