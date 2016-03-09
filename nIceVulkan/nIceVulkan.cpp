#include "stdafx.h"
#include "math/mat4.h"
#include "window.h"
#include "util/file.h"
#include "vkwrap/swap_chain.h"
#include "model.h"
#include "shader.h"
#include "tiny_obj_loader.h"
#include <iostream>

using namespace std;
using namespace nif;
using namespace tinyobj;

int main()
{
	instance instance("nIce Framework");
	device device(instance);
	window wnd(device);

	model sphere(device, "C:/Users/Icy Defiance/Documents/CodeNew/nIceVulkan/nIceVulkan/res/sphere.obj");

	struct ubo_type
	{
		mat4 projectionMatrix;
		mat4 modelMatrix;
		mat4 viewMatrix;
	} uboVS;

	uint32_t swapWidth = wnd.swap_chain().width();
	uint32_t swapHeight = wnd.swap_chain().height();
	uboVS.projectionMatrix = mat4::perspective_fov(.9f, static_cast<float>(swapWidth), static_cast<float>(swapHeight), 0.1f, 256.0f);
	uboVS.viewMatrix = mat4::translation(vec3(0.0f, 0.0f, -2.5f));
	uboVS.modelMatrix = mat4::identity();

	render_pass renderpass(device);
	pipeline_cache cache(device);
	shader shader(
		renderpass,
		cache,
		file::read_all_bytes("res/triangle.vert.spv"),
		file::read_all_bytes("res/triangle.frag.spv"),
		model::vertex::description());

	descriptor_pool descriptorPool(device);
	buffer<ubo_type> uboBuffer(device, vk::BufferUsageFlagBits::eUniformBuffer, vector<ubo_type>(1, uboVS));
	descriptor_set descriptorSet(shader.descriptor_set_layouts(), descriptorPool, uboBuffer);

	std::vector<command_buffer> drawCmdBuffers;
	for (size_t i = 0; i < wnd.swap_chain().buffers().size(); i++)
		drawCmdBuffers.push_back(command_buffer(wnd.command_pool()));

	for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
	{
		drawCmdBuffers[i].begin();
		drawCmdBuffers[i].begin_render_pass(renderpass, wnd.swap_chain().framebuffers()[i], swapWidth, swapHeight);
		drawCmdBuffers[i].set_viewport(static_cast<float>(swapWidth), static_cast<float>(swapHeight));
		drawCmdBuffers[i].set_scissor(0, 0, swapWidth, swapHeight);
		drawCmdBuffers[i].bind_descriptor_sets(shader.pipeline_layout(), descriptorSet);
		drawCmdBuffers[i].bind_pipeline(shader.pipeline());
		drawCmdBuffers[i].bind_vertex_buffer(sphere.meshes()[0].vertex_buffer());
		drawCmdBuffers[i].bind_index_buffer(sphere.meshes()[0].index_buffer());
		drawCmdBuffers[i].draw_indexed(sphere.meshes()[0].index_count());
		drawCmdBuffers[i].end_render_pass();
		drawCmdBuffers[i].pipeline_barrier(wnd.swap_chain().buffers()[i].image);
		drawCmdBuffers[i].end();
	}

	/////////

	wnd.keyhit(keys::escape).add([&]() {
		wnd.close();
	});

	uint32_t currentBuffer = 0;
	command_buffer postPresentCmdBuffer(wnd.command_pool());
	wnd.draw().add([&](double delta) {
		device.wait_queue_idle();

		semaphore presentCompleteSemaphore(device);
		currentBuffer = wnd.swap_chain().acquireNextImage(presentCompleteSemaphore, currentBuffer);
		drawCmdBuffers[currentBuffer].submit(device);
		wnd.swap_chain().queuePresent(currentBuffer);

		postPresentCmdBuffer.begin();
		postPresentCmdBuffer.pipeline_barrier(wnd.swap_chain().buffers()[currentBuffer].image);
		postPresentCmdBuffer.end();
		postPresentCmdBuffer.submit(device);
	});
	wnd.run(60);
}
