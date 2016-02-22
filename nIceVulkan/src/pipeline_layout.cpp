#include "stdafx.h"
#include "pipeline_layout.h"

using namespace std;

namespace nif
{
	pipeline_layout::pipeline_layout(const vector<descriptor_set_layout> &descSetLayouts)
		: device_(descSetLayouts[0].parent_device())
	{
		vk::PipelineLayoutCreateInfo pPipelineLayoutCreateInfo;
		pPipelineLayoutCreateInfo.setLayoutCount(static_cast<uint32_t>(descSetLayouts.size()));

		vector<vk::DescriptorSetLayout> handles;
		for (auto &layout : descSetLayouts)
			handles.push_back(layout.handle());

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
