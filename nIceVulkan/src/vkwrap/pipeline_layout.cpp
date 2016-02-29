#include "stdafx.h"
#include "vkwrap/pipeline_layout.h"
#include "util/setops.h"

using namespace std;

namespace nif
{
	pipeline_layout::pipeline_layout(const vector<descriptor_set_layout> &descSetLayouts)
		: device_(descSetLayouts[0].parent_device())
	{
		auto handles = set::from(descSetLayouts)
			.select([](const descriptor_set_layout &x) { return x.handle(); })
			.to_vector();

		vk::PipelineLayoutCreateInfo pPipelineLayoutCreateInfo;
		pPipelineLayoutCreateInfo.setLayoutCount(static_cast<uint32_t>(descSetLayouts.size()));
		pPipelineLayoutCreateInfo.pSetLayouts(handles.data());

		if (vk::createPipelineLayout(device_.handle(), &pPipelineLayoutCreateInfo, nullptr, &handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");
	}

	pipeline_layout::~pipeline_layout()
	{
		vk::destroyPipelineLayout(device_.handle(), handle_, nullptr);
	}

	vk::PipelineLayout pipeline_layout::handle() const
	{
		return handle_;
	}
}
