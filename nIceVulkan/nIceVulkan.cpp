#include "stdafx.h"
#include "buffer.h"
#include "math/vec3.h"
#include "math/mat4.h"
#include "render_pass.h"
#include "command_buffer.h"
#include "framebuffer.h"
#include "window.h"
#include "util/linq.h"
#include "pipeline_layout.h"
#include "descriptor_set.h"
#include "util/file.h"
#include "shader_module.h"
#include "pipeline.h"
#include <vector>
#include <string>
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
	vector<vertex> vertices = { { vec3(1, 1, 0) }, { vec3(-1, 1, 0) }, { vec3(0, -1, 0) } };
	std::vector<unsigned int> indices = { 0, 1, 2 };

	instance vkinstance("nIce Framework");
	device vkdevice(vkinstance);
	vertex_buffer<vertex> vbuffer(vkdevice, vk::BufferUsageFlagBits::eVertexBuffer, vertices);
	buffer<uint32_t> ibuffer(vkdevice, vk::BufferUsageFlagBits::eIndexBuffer, indices);

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
	depthStencil.view = unique_ptr<image_view>(new image_view(*depthStencil.image, vkdevice.depth_format(), vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil));

	std::vector<unique_ptr<framebuffer>> framebuffers;
	for (uint32_t i = 0; i < swap.image_count(); i++)
		framebuffers.push_back(unique_ptr<framebuffer>(new framebuffer(win.width(), win.height(), renderpass, { swap.buffers()[i].view, *depthStencil.view })));

	descriptor_set_layout descriptorSetLayout(vkdevice);
	pipeline_layout pipelineLayout({ descriptorSetLayout });
	descriptor_pool descriptorPool(vkdevice);

	//prepare uniform buffers
	struct ubo_type {
		mat4 projectionMatrix;
		mat4 modelMatrix;
		mat4 viewMatrix;
	} uboVS;

	uboVS.projectionMatrix = mat4::perspective_fov(.9f, static_cast<float>(win.width()), static_cast<float>(win.height()), 0.1f, 256.0f);
	uboVS.viewMatrix = mat4::translation(vec3(0.0f, 0.0f, -2.5f));
	uboVS.modelMatrix = mat4::identity();

	buffer<ubo_type> uboBuffer(vkdevice, vk::BufferUsageFlagBits::eUniformBuffer, vector<ubo_type>(1, uboVS));
	descriptor_set descriptorSet({ descriptorSetLayout }, descriptorPool, uboBuffer);

	vector<shader_module> shaderModules =
	{
		shader_module(vkdevice, file::read_all_text(""), vk::ShaderStageFlagBits::eVertex),
		shader_module(vkdevice, file::read_all_text(""), vk::ShaderStageFlagBits::eFragment)
	};
	pipeline_cache pipelineCache(vkdevice);
	pipeline solidPipeline(pipelineLayout, renderpass, shaderModules, vertex::pipeline_info(), pipelineCache);

	////////////

	for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
	{
		drawCmdBuffers[i].begin();
		drawCmdBuffers[i].begin_render_pass(renderpass, *framebuffers[i], win.width(), win.height());
		drawCmdBuffers[i].set_viewport(win.width(), win.height());
		drawCmdBuffers[i].set_scissor(0, 0, win.width(), win.height());
		drawCmdBuffers[i].bind_descriptor_sets(pipelineLayout, descriptorSet);
		drawCmdBuffers[i].bind_pipeline(solidPipeline);
		drawCmdBuffers[i].bind_vertex_buffer(vbuffer);
		drawCmdBuffers[i].bind_index_buffer(ibuffer);
		drawCmdBuffers[i].draw_indexed(indices.size());
		drawCmdBuffers[i].end_render_pass();
		drawCmdBuffers[i].pipeline_barrier(*swap.buffers()[i].image);
	}
}

