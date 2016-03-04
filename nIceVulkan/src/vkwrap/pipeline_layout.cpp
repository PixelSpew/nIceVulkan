#include "stdafx.h"
#include "vkwrap/pipeline_layout.h"
#include "util/setops.h"
#include "util/shortcuts.h"

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

		vk_try(vk::createPipelineLayout(device_.handle(), &pPipelineLayoutCreateInfo, nullptr, &handle_));
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
