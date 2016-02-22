#include "stdafx.h"
#include "math/mat4.h"
#include "window.h"
#include "util/file.h"
#include "vkwrap/swap_chain.h"
#include <iostream>

using namespace std;
using namespace nif;

struct vertex
{
	vec3 pos;

	static const vector<vk::VertexInputBindingDescription>& binding_descriptions()
	{
		static const vector<vk::VertexInputBindingDescription> bindDescs =
		{
			vk::VertexInputBindingDescription(0, sizeof(vertex), vk::VertexInputRate::eVertex)
		};

		return bindDescs;
	}

	static const vector<vk::VertexInputAttributeDescription>& attribute_descriptions()
	{
		static const vector<vk::VertexInputAttributeDescription> attribDescs =
		{
			vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, 0)
		};

		return attribDescs;
	}

	static const vk::PipelineVertexInputStateCreateInfo& pipeline_info()
	{
		static const vk::PipelineVertexInputStateCreateInfo pipelineInfo( 0, 1, binding_descriptions().data(), 1, attribute_descriptions().data());
		return pipelineInfo;
	}
};

int main()
{
	instance vkinstance("nIce Framework");
	device vkdevice(vkinstance);

	vector<vertex> vertices = { { vec3(1, 1, 0) }, { vec3(-1, 1, 0) }, { vec3(0, -1, 0) } };
	vertex_buffer<vertex> vbuffer(vkdevice, vk::BufferUsageFlagBits::eVertexBuffer, vertices);

	std::vector<unsigned int> indices = { 0, 1, 2 };
	buffer<uint32_t> ibuffer(vkdevice, vk::BufferUsageFlagBits::eIndexBuffer, indices);

	window win;
	render_pass renderpass(vkdevice);
	swap_chain swap(vkdevice, win.hinstance(), win.hwnd());

	command_pool cmdpool(swap.surface());

	command_buffer setupCmdBuffer(cmdpool);
	setupCmdBuffer.begin();

	uint32_t width = win.width();
	uint32_t height = win.height();
	swap.setup(setupCmdBuffer, &width, &height);

	std::vector<command_buffer> drawCmdBuffers;
	for (uint32_t i = 0; i < swap.image_count(); i++)
		drawCmdBuffers.push_back(command_buffer(cmdpool));

	//setup depth stencil
	struct
	{
		unique_ptr<image> image;
		unique_ptr<image_view> view;
	} depthStencil;

	depthStencil.image = unique_ptr<image>(new image(width, height, vkdevice));
	setupCmdBuffer.setImageLayout(*depthStencil.image, vk::ImageAspectFlagBits::eDepth, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
	depthStencil.view = unique_ptr<image_view>(new image_view(*depthStencil.image, vkdevice.depth_format(), vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil));

	std::vector<framebuffer> framebuffers;
	for (uint32_t i = 0; i < swap.image_count(); i++)
		framebuffers.push_back(framebuffer(width, height, renderpass, { swap.buffers()[i]->view, *depthStencil.view }));

	setupCmdBuffer.end();
	setupCmdBuffer.submit(vkdevice);
	vkdevice.wait_queue_idle();

	vector<descriptor_set_layout> descriptorSetLayouts;
	descriptorSetLayouts.push_back(descriptor_set_layout(vkdevice));
	pipeline_layout pipelineLayout(descriptorSetLayouts);
	descriptor_pool descriptorPool(vkdevice);

	//prepare uniform buffers
	struct ubo_type {
		mat4 projectionMatrix;
		mat4 modelMatrix;
		mat4 viewMatrix;
	} uboVS;

	uboVS.projectionMatrix = mat4::perspective_fov(.9f, static_cast<float>(width), static_cast<float>(height), 0.1f, 256.0f);
	uboVS.viewMatrix = mat4::translation(vec3(0.0f, 0.0f, -2.5f));
	uboVS.modelMatrix = mat4::identity();

	buffer<ubo_type> uboBuffer(vkdevice, vk::BufferUsageFlagBits::eUniformBuffer, vector<ubo_type>(1, uboVS));
	descriptor_set descriptorSet(descriptorSetLayouts, descriptorPool, uboBuffer);

	vector<shader_module> shaderModules;
	shaderModules.push_back(shader_module(vkdevice, file::read_all_text("res/triangle.vert.spv"), vk::ShaderStageFlagBits::eVertex));
	shaderModules.push_back(shader_module(vkdevice, file::read_all_text("res/triangle.frag.spv"), vk::ShaderStageFlagBits::eFragment));

	pipeline_cache pipelineCache(vkdevice);
	pipeline solidPipeline(pipelineLayout, renderpass, shaderModules, vertex::pipeline_info(), pipelineCache);

	for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
	{
		drawCmdBuffers[i].begin();
		drawCmdBuffers[i].begin_render_pass(renderpass, framebuffers[i], width, height);
		drawCmdBuffers[i].set_viewport(static_cast<float>(width), static_cast<float>(height));
		drawCmdBuffers[i].set_scissor(0, 0, width, height);
		drawCmdBuffers[i].bind_descriptor_sets(pipelineLayout, descriptorSet);
		drawCmdBuffers[i].bind_pipeline(solidPipeline);
		drawCmdBuffers[i].bind_vertex_buffer(vbuffer);
		drawCmdBuffers[i].bind_index_buffer(ibuffer);
		drawCmdBuffers[i].draw_indexed(static_cast<uint32_t>(indices.size()));
		drawCmdBuffers[i].end_render_pass();
		drawCmdBuffers[i].pipeline_barrier(swap.buffers()[i]->image);
		drawCmdBuffers[i].end();
	}

	/////////

	uint32_t currentBuffer = 0;
	command_buffer postPresentCmdBuffer(cmdpool);

	//game loop begin
	semaphore presentCompleteSemaphore(vkdevice);
	swap.acquireNextImage(presentCompleteSemaphore, &currentBuffer);
	drawCmdBuffers[currentBuffer].submit(vkdevice);
	swap.queuePresent(currentBuffer);
	
	postPresentCmdBuffer.begin();
	postPresentCmdBuffer.pipeline_barrier(swap.buffers()[currentBuffer]->image);
	postPresentCmdBuffer.end();
	postPresentCmdBuffer.submit(vkdevice);
	vkdevice.wait_queue_idle();
	//end game loop
}
