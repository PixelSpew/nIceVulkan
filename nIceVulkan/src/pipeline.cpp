#include "stdafx.h"
#include "pipeline.h"

using namespace std;

namespace nif
{
	pipeline::pipeline(
		const pipeline_layout &layout,
		const render_pass &pass,
		const std::vector<unique_ptr<shader_module>> &shaderModules,
		const vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo,
		const pipeline_cache &cache)
		: device_(pass.parent_device())
	{
		vector<vk::PipelineShaderStageCreateInfo> shaderStages(shaderModules.size());
		for (size_t i = 0; i < shaderStages.size(); i++)
		{
			shaderStages[i].stage(shaderModules[i]->stage());
			shaderStages[i].module(shaderModules[i]->handle());
			shaderStages[i].pName("main");
		}

		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState;
		inputAssemblyState.topology(vk::PrimitiveTopology::eTriangleList);

		vk::PipelineRasterizationStateCreateInfo rasterizationState;
		rasterizationState.polygonMode(vk::PolygonMode::eFill);
		rasterizationState.cullMode(vk::CullModeFlagBits::eNone);
		rasterizationState.frontFace(vk::FrontFace::eCounterClockwise);
		rasterizationState.depthClampEnable(VK_FALSE);
		rasterizationState.rasterizerDiscardEnable(VK_FALSE);
		rasterizationState.depthBiasEnable(VK_FALSE);

		vk::PipelineColorBlendAttachmentState blendAttachmentState[1];
		blendAttachmentState[0].colorWriteMask(static_cast<vk::ColorComponentFlagBits>(0xf));
		blendAttachmentState[0].blendEnable(VK_FALSE);

		vk::PipelineColorBlendStateCreateInfo colorBlendState;
		colorBlendState.attachmentCount(1);
		colorBlendState.pAttachments(blendAttachmentState);

		vk::PipelineMultisampleStateCreateInfo multisampleState;
		multisampleState.rasterizationSamples(vk::SampleCountFlagBits::e1);

		vk::PipelineViewportStateCreateInfo viewportState;
		viewportState.viewportCount(1);
		viewportState.scissorCount(1);

		vk::PipelineDepthStencilStateCreateInfo depthStencilState;
		depthStencilState.depthTestEnable(VK_TRUE);
		depthStencilState.depthWriteEnable(VK_TRUE);
		depthStencilState.depthCompareOp(vk::CompareOp::eLessOrEqual);
		depthStencilState.depthBoundsTestEnable(VK_FALSE);
		depthStencilState.back(vk::StencilOpState(vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways, 0, 0, 0));
		depthStencilState.stencilTestEnable(VK_FALSE);
		depthStencilState.front(depthStencilState.back());

		std::vector<vk::DynamicState> dynamicStateEnables;
		dynamicStateEnables.push_back(vk::DynamicState::eViewport);
		dynamicStateEnables.push_back(vk::DynamicState::eScissor);

		vk::PipelineDynamicStateCreateInfo dynamicState;
		dynamicState.dynamicStateCount(static_cast<uint32_t>(dynamicStateEnables.size()));
		dynamicState.pDynamicStates(dynamicStateEnables.data());

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

		vk::createGraphicsPipelines(pass.parent_device().handle(), cache.handle(), 1, &pipelineCreateInfo, nullptr, &handle_);
	}

	pipeline::~pipeline()
	{
		vk::destroyPipeline(device_.handle(), handle_, nullptr);
	}

	vk::Pipeline pipeline::handle() const
	{
		return handle_;
	}
}