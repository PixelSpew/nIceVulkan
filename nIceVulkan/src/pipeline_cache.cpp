#include "stdafx.h"
#include "pipeline_cache.h"

using namespace std;

namespace nif
{
	pipeline_cache::pipeline_cache(const device &device)
		: device_(device)
	{
		vk::PipelineCacheCreateInfo pipelineCacheCreateInfo;
		if (vk::createPipelineCache(device.handle(), &pipelineCacheCreateInfo, nullptr, &handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");
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
