#include "stdafx.h"
#include "buffer.h"
#include "math/vec3.h"
#include "math/mat4.h"
#include "render_pass.h"
#include "command_buffer.h"
#include "framebuffer.h"
#include "window.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace nif;

struct vertex
{
	vec3 pos;
};

int exit(string message)
{
	cout << message << endl;
	cin.ignore();
	return 1;
}

int main()
{
	vector<vertex> vertices = { { vec3(1, 1, 0) }, { vec3(-1, 1, 0) }, { vec3(0, -1, 0) } };
	std::vector<unsigned int> indices = { 0, 1, 2 };

	instance vkinstance("nIce Framework");
	device vkdevice(vkinstance);
	buffer<vertex> vbuffer(vkdevice, vk::BufferUsageFlagBits::eVertexBuffer, vertices);
	buffer<uint32_t> ibuffer(vkdevice, vk::BufferUsageFlagBits::eIndexBuffer, indices);

	vbuffer.bind_descs().resize(1);
	vbuffer.bind_descs()[0].stride(sizeof(vertex));

	vbuffer.attrib_descs().resize(1);
	vbuffer.attrib_descs()[0].location(0);
	vbuffer.attrib_descs()[0].format(vk::Format::eR32G32B32Sfloat);
	vbuffer.attrib_descs()[0].offset(0);

	vbuffer.pipeline_info().vertexBindingDescriptionCount(static_cast<uint32_t>(vbuffer.bind_descs().size()));
	vbuffer.pipeline_info().pVertexBindingDescriptions(vbuffer.bind_descs().data());
	vbuffer.pipeline_info().vertexAttributeDescriptionCount(static_cast<uint32_t>(vbuffer.attrib_descs().size()));
	vbuffer.pipeline_info().pVertexAttributeDescriptions(vbuffer.attrib_descs().data());

	window win;
	render_pass renderpass(vkdevice);
	swap_chain swap(vkinstance, vkdevice, win.hinstance(), win.hwnd());

	command_pool cmdpool(swap);
	std::vector<command_buffer> drawCmdBuffers(swap.image_count(), command_buffer(cmdpool));
	command_buffer postPresentCmdBuffer(cmdpool);
	command_buffer setupCmdBuffer(cmdpool);
	setupCmdBuffer.begin();

	//setup depth stencil
	struct
	{
		unique_ptr<image> image;
		unique_ptr<image_view> view;
	} depthStencil;

	depthStencil.image = unique_ptr<image>(new image(win.width(), win.height(), vkdevice));
	setupCmdBuffer.setImageLayout(*depthStencil.image, vk::ImageAspectFlagBits::eDepth, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
	depthStencil.view = unique_ptr<image_view>(new image_view(*depthStencil.image));


	std::vector<framebuffer> framebuffers;
	framebuffers.reserve(swap.image_count());
	for (uint32_t i = 0; i < framebuffers.size(); i++)
	{
		framebuffers.push_back(framebuffer(win.width(), win.height(), renderpass, { swap.buffers()[i].view, *depthStencil.view }));
	}

	//setup descriptor set layout
	vk::DescriptorSetLayoutBinding layoutBinding;
	layoutBinding.descriptorType(vk::DescriptorType::eUniformBuffer);
	layoutBinding.descriptorCount(1);
	layoutBinding.stageFlags(vk::ShaderStageFlagBits::eVertex);
	layoutBinding.pImmutableSamplers(nullptr);

	vk::DescriptorSetLayoutCreateInfo descriptorLayout;
	descriptorLayout.bindingCount(1);
	descriptorLayout.pBindings(&layoutBinding);

	vk::DescriptorSetLayout descriptorSetLayout;
	vk::createDescriptorSetLayout(vkdevice.handle(), &descriptorLayout, NULL, &descriptorSetLayout);

	vk::PipelineLayoutCreateInfo pPipelineLayoutCreateInfo;
	pPipelineLayoutCreateInfo.setLayoutCount(1);
	pPipelineLayoutCreateInfo.pSetLayouts(&descriptorSetLayout);

	vk::PipelineLayout pipelineLayout;
	vk::createPipelineLayout(vkdevice.handle(), &pPipelineLayoutCreateInfo, nullptr, &pipelineLayout);

	//setup descriptor pool
	vk::DescriptorPoolSize typeCounts[1];
	typeCounts[0].type(vk::DescriptorType::eUniformBuffer);
	typeCounts[0].descriptorCount(1);

	vk::DescriptorPoolCreateInfo descriptorPoolInfo;
	descriptorPoolInfo.poolSizeCount(1);
	descriptorPoolInfo.pPoolSizes(typeCounts);
	descriptorPoolInfo.maxSets(1);

	vk::DescriptorPool descriptorPool;
	vk::createDescriptorPool(vkdevice.handle(), &descriptorPoolInfo, nullptr, &descriptorPool);

	//prepare uniform buffers
	struct {
		mat4 projectionMatrix;
		mat4 modelMatrix;
		mat4 viewMatrix;
	} uboVS;

	struct {
		vk::Buffer buffer;
		vk::DeviceMemory memory;
		vk::DescriptorBufferInfo descriptor;
	}  uniformDataVS;

	vk::MemoryRequirements memReqs;

	vk::MemoryAllocateInfo allocInfo;
	allocInfo.allocationSize(0);
	allocInfo.memoryTypeIndex(0);

	vk::BufferCreateInfo bufferInfo;
	bufferInfo.size(sizeof(uboVS));
	bufferInfo.usage(vk::BufferUsageFlagBits::eUniformBuffer);

	vk::createBuffer(vkdevice.handle(), &bufferInfo, nullptr, &uniformDataVS.buffer);
	vk::getBufferMemoryRequirements(vkdevice.handle(), uniformDataVS.buffer, &memReqs);
	allocInfo.allocationSize = memReqs.size;

	uint32_t typeBits = memReqs.memoryTypeBits();
	for (uint32_t i = 0; i < 32; i++)
	{
		if (typeBits & 1)
		{
			if (vkdevice.memory_properties().memoryTypes()[i].propertyFlags() & vk::MemoryPropertyFlagBits::eHostVisible)
			{
				allocInfo.memoryTypeIndex(i);
				break;
			}
		}
		typeBits >>= 1;
	}

	vk::allocateMemory(vkdevice.handle(), &allocInfo, nullptr, &(uniformDataVS.memory));
	vk::bindBufferMemory(vkdevice.handle(), uniformDataVS.buffer, uniformDataVS.memory, 0);

	uniformDataVS.descriptor.buffer = uniformDataVS.buffer;
	uniformDataVS.descriptor.offset = 0;
	uniformDataVS.descriptor.range = sizeof(uboVS);

	//update uniform buffers
	uboVS.projectionMatrix = mat4::perspective_fov(.9f, win.width(), win.height(), 0.1f, 256.0f);
	uboVS.viewMatrix = mat4::translation(vec3(0.0f, 0.0f, -2.5f));
	uboVS.modelMatrix = mat4::identity();

	uint8_t *pData;
	vk::mapMemory(vkdevice.handle(), uniformDataVS.memory, 0, sizeof(uboVS), 0, (void **)&pData);
	memcpy(pData, &uboVS, sizeof(uboVS));
	vk::unmapMemory(vkdevice.handle(), uniformDataVS.memory);

	//setup descriptor set
	vk::DescriptorSetAllocateInfo dsAllocInfo;
	dsAllocInfo.descriptorPool(descriptorPool);
	dsAllocInfo.descriptorSetCount(1);
	dsAllocInfo.pSetLayouts(&descriptorSetLayout);

	vk::DescriptorSet descriptorSet;
	vk::allocateDescriptorSets(vkdevice.handle(), &dsAllocInfo, &descriptorSet);

	vk::WriteDescriptorSet writeDescriptorSet;
	writeDescriptorSet.dstSet(descriptorSet);
	writeDescriptorSet.descriptorCount(1);
	writeDescriptorSet.descriptorType(vk::DescriptorType::eUniformBuffer);
	writeDescriptorSet.pBufferInfo(&uniformDataVS.descriptor);
	writeDescriptorSet.dstBinding(0);

	vk::updateDescriptorSets(vkdevice.handle(), 1, &writeDescriptorSet, 0, NULL);

	//prepare pipelines
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

	vk::PipelineViewportStateCreateInfo viewportState;
	viewportState.viewportCount(1);
	viewportState.scissorCount(1);

	vector<vk::DynamicState> dynamicStateEnables;
	dynamicStateEnables.push_back(vk::DynamicState::eViewport);
	dynamicStateEnables.push_back(vk::DynamicState::eScissor);

	vk::PipelineDynamicStateCreateInfo dynamicState;
	dynamicState.pDynamicStates(dynamicStateEnables.data());
	dynamicState.dynamicStateCount(dynamicStateEnables.size());


	vk::PipelineDepthStencilStateCreateInfo depthStencilState;
	depthStencilState.depthTestEnable(VK_TRUE);
	depthStencilState.depthWriteEnable(VK_TRUE);
	depthStencilState.depthCompareOp(vk::CompareOp::eLessOrEqual);
	depthStencilState.depthBoundsTestEnable(VK_FALSE);
	depthStencilState.back.failOp(vk::StencilOp::eKeep);
	depthStencilState.back.passOp(vk::StencilOp::eKeep);
	depthStencilState.back.compareOp(vk::CompareOp::eAlways);
	depthStencilState.stencilTestEnable(VK_FALSE);
	depthStencilState.front(depthStencilState.back);

	vk::PipelineMultisampleStateCreateInfo multisampleState;
	multisampleState.pSampleMask(nullptr);
	multisampleState.rasterizationSamples(vk::SampleCountFlagBits::e1);

	std::vector<VkShaderModule> shaderModules;

	//load shaders
	vk::PipelineShaderStageCreateInfo vShaderStage;
	vShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vShaderStage.stage = vk::ShaderStageFlagBits::eVertex;

	size_t size = 0;
	const char *shaderCode = readBinaryFile(fileName, &size);

	VkShaderModule shaderModule;
	VkShaderModuleCreateInfo moduleCreateInfo;
	VkResult err;

	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.pNext = NULL;

	moduleCreateInfo.codeSize = size;
	moduleCreateInfo.pCode = (uint32_t*)shaderCode;
	moduleCreateInfo.flags = 0;
	err = vkCreateShaderModule(device, &moduleCreateInfo, NULL, &shaderModule);

	vShaderStage.module = vkTools::loadShader(fileName, vkdevice.handle(), vk::ShaderStageFlagBits::eVertex);
	vShaderStage.pName = "main";
	shaderModules.push_back(vShaderStage.module);
	//end load shaders

	vk::PipelineShaderStageCreateInfo shaderStages[2];
	shaderStages[0] = loadShader("./../data/shaders/triangle.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	shaderStages[1] = loadShader("./../data/shaders/triangle.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

	vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
	pipelineCreateInfo.layout(pipelineLayout);
	pipelineCreateInfo.renderPass(renderPass);
	pipelineCreateInfo.stageCount(2);
	pipelineCreateInfo.pVertexInputState(&vertices.vi);
	pipelineCreateInfo.pInputAssemblyState(&inputAssemblyState);
	pipelineCreateInfo.pRasterizationState(&rasterizationState);
	pipelineCreateInfo.pColorBlendState(&colorBlendState);
	pipelineCreateInfo.pMultisampleState(&multisampleState);
	pipelineCreateInfo.pViewportState(&viewportState);
	pipelineCreateInfo.pDepthStencilState(&depthStencilState);
	pipelineCreateInfo.pStages(shaderStages);
	pipelineCreateInfo.renderPass(renderPass);
	pipelineCreateInfo.pDynamicState(&dynamicState);

	struct {
		VkPipeline solid;
	} pipelines;
	vk::createGraphicsPipelines(vkdevice.handle(), pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipelines.solid);

	//build command buffers
	vk::ClearValue clearValues[2];
	std::array<float, 4> clearColor = { .2f, .2f, .2f, 1 };
	clearValues[0].color(vk::ClearColorValue(clearColor));
	clearValues[1].depthStencil(vk::ClearDepthStencilValue(1.0f, 0));

	vk::RenderPassBeginInfo renderPassBeginInfo;
	renderPassBeginInfo.renderPass(renderPass);
	renderPassBeginInfo.renderArea(
		vk::Rect2D(
			vk::Offset2D(0, 0),
			vk::Extent2D(static_cast<uint32_t>(win.width()), static_cast<uint32_t>(win.height()))
		)
	);
	renderPassBeginInfo.clearValueCount(2);
	renderPassBeginInfo.pClearValues(clearValues);

	vk::CommandBufferBeginInfo cmdBufInfo;
	for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
	{
		// Set target frame buffer
		renderPassBeginInfo.framebuffer = frameBuffers[i];

		vk::beginCommandBuffer(drawCmdBuffers[i], &cmdBufInfo);

		vk::cmdBeginRenderPass(drawCmdBuffers[i], &renderPassBeginInfo, vk::SubpassContents::eInline);

		// Update dynamic viewport state
		vk::Viewport viewport;
		viewport.width(static_cast<float>(win.width()));
		viewport.height(static_cast<float>(win.height()));
		viewport.minDepth(0.0f);
		viewport.maxDepth(1.0f);
		vk::cmdSetViewport(drawCmdBuffers[i], 0, 1, &viewport);

		// Update dynamic scissor state
		vk::Rect2D scissor;
		scissor.extent(vk::Extent2D(win.width(), win.height()));
		scissor.offset(vk::Offset2D(0, 0));
		vk::cmdSetScissor(drawCmdBuffers[i], 0, 1, &scissor);

		// Bind descriptor sets describing shader binding points
		vk::cmdBindDescriptorSets(drawCmdBuffers[i], vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

		// Bind the rendering pipeline (including the shaders)
		vkCmdBindPipeline(drawCmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines.solid);

		// Bind triangle vertices
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(drawCmdBuffers[i], VERTEX_BUFFER_BIND_ID, 1, &vertices.buf, offsets);

		// Bind triangle indices
		vkCmdBindIndexBuffer(drawCmdBuffers[i], indices.buf, 0, VK_INDEX_TYPE_UINT32);

		// Draw indexed triangle
		vkCmdDrawIndexed(drawCmdBuffers[i], indices.count, 1, 0, 0, 1);

		vkCmdEndRenderPass(drawCmdBuffers[i]);

		// Add a present memory barrier to the end of the command buffer
		// This will transform the frame buffer color attachment to a
		// new layout for presenting it to the windowing system integration 
		VkImageMemoryBarrier prePresentBarrier = {};
		prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		prePresentBarrier.pNext = NULL;
		prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		prePresentBarrier.dstAccessMask = 0;
		prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		prePresentBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		prePresentBarrier.image = swapChain.buffers[i].image;

		VkImageMemoryBarrier *pMemoryBarrier = &prePresentBarrier;
		vkCmdPipelineBarrier(
			drawCmdBuffers[i],
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_FLAGS_NONE,
			0, nullptr,
			0, nullptr,
			1, &prePresentBarrier);

		err = vkEndCommandBuffer(drawCmdBuffers[i]);
		assert(!err);
	}


	//semaphore
	vk::SemaphoreCreateInfo presentCompleteSemaphoreCreateInfo;
	vk::Semaphore presentCompleteSemaphore;
	vk::createSemaphore(vkdevice.handle(), &presentCompleteSemaphoreCreateInfo, nullptr, &presentCompleteSemaphore);

	//swap.acquireNextImage(presentCompleteSemaphore, &currentBuffer);
}

