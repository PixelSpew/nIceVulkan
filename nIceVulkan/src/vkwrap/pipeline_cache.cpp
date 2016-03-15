#include "stdafx.h"
#include "vkwrap/pipeline_cache.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	pipeline_cache::pipeline_cache(const device &device) :
		device_(device)
	{
		handle_ = device.create_pipeline_cache(vk::PipelineCacheCreateInfo());
	}

	pipeline_cache::~pipeline_cache()
	{
		device_.destroy_pipeline_cache(handle_);
	}

	vk::PipelineCache pipeline_cache::handle() const
	{
		return handle_;
	}
}
