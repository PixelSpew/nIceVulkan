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
		auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo()
			.topology(vk::PrimitiveTopology::eTriangleList);

		vk::PipelineRasterizationStateCreateInfo rasterizationState;

		vector<vk::PipelineColorBlendAttachmentState> blendAttachmentState(1);
		blendAttachmentState[0].colorWriteMask(static_cast<vk::ColorComponentFlagBits>(0xf));

		auto colorBlendState = vk::PipelineColorBlendStateCreateInfo()
			.attachmentCount(static_cast<uint32_t>(blendAttachmentState.size()))
			.pAttachments(blendAttachmentState.data());

		vk::PipelineMultisampleStateCreateInfo multisampleState;

		auto viewportState = vk::PipelineViewportStateCreateInfo()
			.viewportCount(1)
			.scissorCount(1);

		auto stencilOpState = vk::StencilOpState()
			.compareOp(vk::CompareOp::eAlways);

		auto depthStencilState = vk::PipelineDepthStencilStateCreateInfo()
			.depthTestEnable(VK_TRUE)
			.depthWriteEnable(VK_TRUE)
			.depthCompareOp(vk::CompareOp::eLessOrEqual)
			.back(stencilOpState)
			.front(stencilOpState);

		std::vector<vk::DynamicState> dynamicStateEnables = {
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor
		};

		auto dynamicState = vk::PipelineDynamicStateCreateInfo()
			.dynamicStateCount(static_cast<uint32_t>(dynamicStateEnables.size()))
			.pDynamicStates(dynamicStateEnables.data());

		auto shaderStages = set::from(shaderModules)
			.select([](const shader_module &x) {
				return vk::PipelineShaderStageCreateInfo()
					.stage(x.stage())
					.module(x.handle())
					.pName("main");
			}).to_vector();

		handle_ = pass.parent_device().create_graphics_pipelines(
			cache.handle(),
			{
				vk::GraphicsPipelineCreateInfo()
					.layout(layout.handle())
					.renderPass(pass.handle())
					.stageCount(static_cast<uint32_t>(shaderStages.size()))
					.pStages(shaderStages.data())
					.pVertexInputState(&vertexInputStateCreateInfo)
					.pInputAssemblyState(&inputAssemblyState)
					.pRasterizationState(&rasterizationState)
					.pColorBlendState(&colorBlendState)
					.pMultisampleState(&multisampleState)
					.pViewportState(&viewportState)
					.pDepthStencilState(&depthStencilState)
					.pDynamicState(&dynamicState)
			})[0];
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
			device_->destroy_pipeline(handle_);
	}

	vk::Pipeline pipeline::handle() const
	{
		return handle_;
	}

	pipeline& pipeline::operator=(pipeline &&rhs)
	{
		if (handle_ != rhs.handle_) {
			handle_ = rhs.handle_;
			device_ = rhs.device_;
			rhs.handle_ = nullptr;
		}

		return *this;
	}
}