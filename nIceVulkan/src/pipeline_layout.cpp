#include "stdafx.h"
#include "pipeline_layout.h"
#include "util/linq.h"

using namespace std;

namespace nif
{
	pipeline_layout::pipeline_layout(const std::initializer_list<std::reference_wrapper<const descriptor_set_layout>> descSetLayouts)
		: device_((*descSetLayouts.begin()).get().parent_device())
	{
		vk::PipelineLayoutCreateInfo pPipelineLayoutCreateInfo;
		pPipelineLayoutCreateInfo.setLayoutCount(static_cast<uint32_t>(descSetLayouts.size()));
		pPipelineLayoutCreateInfo.pSetLayouts(
			from(descSetLayouts)
			.select<vk::DescriptorSetLayout>([](const reference_wrapper<const descriptor_set_layout> &x) { return x.get().handle(); })
			.to_vector()
			.data()
		);

		vk::createPipelineLayout((*descSetLayouts.begin()).get().parent_device().handle(), &pPipelineLayoutCreateInfo, nullptr, &handle_);
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
