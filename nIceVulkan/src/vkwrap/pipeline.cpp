#include "stdafx.h"
#include "vkwrap/pipeline.h"
#include "util/setops.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	pipeline::pipeline()
	{
	}

	pipeline::pipeline(
		const pipeline_layout &layout,
		const render_pass &pass,
		const std::vector<shader_module> &shaderModules,
		const vk::PipelineVertexInputStateCreateInfo &vertexInputStateCreateInfo,
		const pipeline_cache &cache) :
		device_(&pass.parent_device())
	{
		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState;
		inputAssemblyState.topology(vk::PrimitiveTopology::eTriangleList);

		vk::PipelineRasterizationStateCreateInfo rasterizationState;

		vector<vk::PipelineColorBlendAttachmentState> blendAttachmentState(1);
		blendAttachmentState[0].colorWriteMask(static_cast<vk::ColorComponentFlagBits>(0xf));

		vk::PipelineColorBlendStateCreateInfo colorBlendState;
		colorBlendState.attachmentCount(static_cast<uint32_t>(blendAttachmentState.size()));
		colorBlendState.pAttachments(blendAttachmentState.data());

		vk::PipelineMultisampleStateCreateInfo multisampleState;

		vk::PipelineViewportStateCreateInfo viewportState;
		viewportState.viewportCount(1);
		viewportState.scissorCount(1);

		vk::StencilOpState stencilOpState;
		stencilOpState.compareOp(vk::CompareOp::eAlways);

		vk::PipelineDepthStencilStateCreateInfo depthStencilState;
		depthStencilState.depthTestEnable(VK_TRUE);
		depthStencilState.depthWriteEnable(VK_TRUE);
		depthStencilState.depthCompareOp(vk::CompareOp::eLessOrEqual);
		depthStencilState.back(stencilOpState);
		depthStencilState.front(stencilOpState);

		std::vector<vk::DynamicState> dynamicStateEnables = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		vk::PipelineDynamicStateCreateInfo dynamicState;
		dynamicState.dynamicStateCount(static_cast<uint32_t>(dynamicStateEnables.size()));
		dynamicState.pDynamicStates(dynamicStateEnables.data());

		auto shaderStages = set::from(shaderModules)
			.select([](const shader_module &x) {
				return vk::PipelineShaderStageCreateInfo(0, x.stage(), x.handle(), "main", nullptr);
			}).to_vector();

		vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
		pipelineCreateInfo.layout(layout.handle());
		pipelineCreateInfo.renderPass(pass.handle());
		pipelineCreateInfo.stageCount(static_cast<uint32_t>(shaderStages.size()));
		pipelineCreateInfo.pStages(shaderStages.data());
		pipelineCreateInfo.pVertexInputState(&vertexInputStateCreateInfo);
		pipelineCreateInfo.pInputAssemblyState(&inputAssemblyState);
		pipelineCreateInfo.pRasterizationState(&rasterizationState);
		pipelineCreateInfo.pColorBlendState(&colorBlendState);
		pipelineCreateInfo.pMultisampleState(&multisampleState);
		pipelineCreateInfo.pViewportState(&viewportState);
		pipelineCreateInfo.pDepthStencilState(&depthStencilState);
		pipelineCreateInfo.pDynamicState(&dynamicState);

		vk_try(vk::createGraphicsPipelines(pass.parent_device().handle(), cache.handle(), 1, &pipelineCreateInfo, nullptr, &handle_));
	}

	pipeline::pipeline(pipeline &&old) :
		handle_(old.handle_),
		device_(old.device_)
	{
		old.handle_ = nullptr;
	}

	pipeline::~pipeline()
	{
		if (handle_)
			vk::destroyPipeline(device_->handle(), handle_, nullptr);
	}

	vk::Pipeline pipeline::handle() const
	{
		return handle_;
	}

	pipeline& pipeline::operator=(pipeline && rhs)
	{
		if (handle_ != rhs.handle_) {
			handle_ = rhs.handle_;
			device_ = rhs.device_;
			rhs.handle_ = nullptr;
		}

		return *this;
	}
}