#include "stdafx.h"
#include "pipeline_cache.h"

namespace nif
{
	pipeline_cache::pipeline_cache(const device &device)
		: device_(device)
	{
		vk::PipelineCacheCreateInfo pipelineCacheCreateInfo;
		vk::createPipelineCache(device.handle(), &pipelineCacheCreateInfo, nullptr, &handle_);
	}

	pipeline_cache::~pipeline_cache()
	{
		vk::destroyPipelineCache(device_.handle(), handle_, nullptr);
	}

	vk::PipelineCache pipeline_cache::handle() const
	{
		return handle_;
	}
}
