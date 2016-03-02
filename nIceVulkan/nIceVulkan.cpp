#include "stdafx.h"
#include "math/mat4.h"
#include "window.h"
#include "util/file.h"
#include "vkwrap/swap_chain.h"
#include "model.h"
#include "tiny_obj_loader.h"
#include <iostream>

using namespace std;
using namespace nif;
using namespace tinyobj;

int main()
{
	instance instance("nIce Framework");
	device device(instance);
	window win(device);
	render_pass renderpass(device);

	model sphere(device, "C:/Users/Icy Defiance/Documents/CodeNew/nIceVulkan/nIceVulkan/res/sphere.obj");

	std::vector<command_buffer> drawCmdBuffers;
	for (uint32_t i = 0; i < win.vk_swap_chain().image_count(); i++)
		drawCmdBuffers.push_back(command_buffer(win.vk_command_pool()));

	std::vector<framebuffer> framebuffers;
	for (uint32_t i = 0; i < win.vk_swap_chain().image_count(); i++)
		framebuffers.push_back(framebuffer(win.vk_width(), win.vk_height(), renderpass, { win.vk_swap_chain().buffers()[i].view, win.depth_stencil_view() }));

	vector<descriptor_set_layout> descriptorSetLayouts;
	descriptorSetLayouts.push_back(descriptor_set_layout(device));
	pipeline_layout pipelineLayout(descriptorSetLayouts);
	descriptor_pool descriptorPool(device);

	//prepare uniform buffers
	struct ubo_type {
		mat4 projectionMatrix;
		mat4 modelMatrix;
		mat4 viewMatrix;
	} uboVS;

	uboVS.projectionMatrix = mat4::perspective_fov(.9f, static_cast<float>(win.vk_width()), static_cast<float>(win.vk_height()), 0.1f, 256.0f);
	uboVS.viewMatrix = mat4::translation(vec3(0.0f, 0.0f, -2.5f));
	uboVS.modelMatrix = mat4::identity();

	buffer<ubo_type> uboBuffer(device, vk::BufferUsageFlagBits::eUniformBuffer, vector<ubo_type>(1, uboVS));
	descriptor_set descriptorSet(descriptorSetLayouts, descriptorPool, uboBuffer);

	vector<shader_module> shaderModules;
	shaderModules.push_back(shader_module(device, file::read_all_text("res/triangle.vert.spv"), vk::ShaderStageFlagBits::eVertex));
	shaderModules.push_back(shader_module(device, file::read_all_text("res/triangle.frag.spv"), vk::ShaderStageFlagBits::eFragment));

	pipeline_cache pipelineCache(device);
	pipeline solidPipeline(pipelineLayout, renderpass, shaderModules, model::vertex::pipeline_info(), pipelineCache);

	for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
	{
		drawCmdBuffers[i].begin();
		drawCmdBuffers[i].begin_render_pass(renderpass, framebuffers[i], win.vk_width(), win.vk_height());
		drawCmdBuffers[i].set_viewport(static_cast<float>(win.vk_width()), static_cast<float>(win.vk_height()));
		drawCmdBuffers[i].set_scissor(0, 0, win.vk_width(), win.vk_height());
		drawCmdBuffers[i].bind_descriptor_sets(pipelineLayout, descriptorSet);
		drawCmdBuffers[i].bind_pipeline(solidPipeline);
		drawCmdBuffers[i].bind_vertex_buffer(sphere.meshes()[0].vertex_buffer());
		drawCmdBuffers[i].bind_index_buffer(sphere.meshes()[0].index_buffer());
		drawCmdBuffers[i].draw_indexed(sphere.meshes()[0].index_count());
		drawCmdBuffers[i].end_render_pass();
		drawCmdBuffers[i].pipeline_barrier(win.vk_swap_chain().buffers()[i].image);
		drawCmdBuffers[i].end();
	}

	/////////

	uint32_t currentBuffer = 0;
	command_buffer postPresentCmdBuffer(win.vk_command_pool());

	win.keyhit(keys::escape).add([&]() {
		win.close();
	});

	win.buttonhit(buttons::left).add([&]() {
		win.close();
	});

	win.draw().add([&](double delta) {
		device.wait_queue_idle();

		semaphore presentCompleteSemaphore(device);
		currentBuffer = win.vk_swap_chain().acquireNextImage(presentCompleteSemaphore, currentBuffer);
		drawCmdBuffers[currentBuffer].submit(device);
		win.vk_swap_chain().queuePresent(currentBuffer);

		postPresentCmdBuffer.begin();
		postPresentCmdBuffer.pipeline_barrier(win.vk_swap_chain().buffers()[currentBuffer].image);
		postPresentCmdBuffer.end();
		postPresentCmdBuffer.submit(device);
	});
	win.run(60);
}
