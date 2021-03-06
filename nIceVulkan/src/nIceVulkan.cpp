#include "stdafx.h"
#include "math/mat4.h"
#include "window.h"
#include "util/file.h"
#include "vkwrap/swap_chain.h"
#include "model.h"
#include "shader.h"
#include "tiny_obj_loader.h"
#include "config.h"
#include <iostream>

using namespace std;
using namespace nif;
using namespace tinyobj;

int main()
{
	instance instance("nIce Framework");
	device device(instance);
	window wnd(device);

	model sphere(device, res_path() + "sphere.obj");

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
		file::read_all_bytes(res_path() + "triangle.vert.spv"),
		file::read_all_bytes(res_path() + "triangle.frag.spv"),
		model::vertex::description());

	descriptor_pool descriptorPool(device);
	buffer<ubo_type> uboBuffer(device, vk::BufferUsageFlagBits::eUniformBuffer, vector<ubo_type>(1, uboVS));
	descriptor_set descriptorSet(shader.descriptor_set_layouts(), descriptorPool);
	descriptorSet.update_buffer(vk::DescriptorType::eUniformBuffer, 0, uboBuffer);

	std::vector<command_buffer> drawCmdBuffers;
	for (size_t i = 0; i < wnd.swap_chain().buffers().size(); i++)
		drawCmdBuffers.push_back(command_buffer(wnd.command_pool()));

	for (int32_t i = 0; i < drawCmdBuffers.size(); ++i)
	{
		drawCmdBuffers[i].begin();
		drawCmdBuffers[i].begin_render_pass(renderpass, wnd.swap_chain().framebuffers()[i], swapWidth, swapHeight);
		drawCmdBuffers[i].set_viewport(static_cast<float>(swapWidth), static_cast<float>(swapHeight));
		drawCmdBuffers[i].set_scissor(0, 0, swapWidth, swapHeight);
		drawCmdBuffers[i].bind_descriptor_set(shader.pipeline_layout(), descriptorSet);
		drawCmdBuffers[i].bind_pipeline(shader.pipeline());
		drawCmdBuffers[i].bind_vertex_buffer(sphere.meshes()[0].vertex_buffer());
		drawCmdBuffers[i].bind_index_buffer(sphere.meshes()[0].index_buffer());
		drawCmdBuffers[i].draw_indexed(sphere.meshes()[0].index_count());
		drawCmdBuffers[i].end_render_pass();
		drawCmdBuffers[i].pipeline_barrier(
			vk::ImageMemoryBarrier()
				.srcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
				.dstAccessMask(static_cast<vk::AccessFlagBits>(0))
				.oldLayout(vk::ImageLayout::eColorAttachmentOptimal)
				.newLayout(vk::ImageLayout::ePresentSrcKHR)
				.srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
				.dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
				.subresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))
				.image(wnd.swap_chain().buffers()[i].image.handle()));
		drawCmdBuffers[i].end();
	}

	/////////

	wnd.keyhit(keys::escape).add([&]() {
		wnd.close();
	});

	command_buffer postPresentCmdBuffer(wnd.command_pool());
	semaphore presentCompleteSemaphore(device);
	semaphore renderCompleteSemaphore(device);
	wnd.draw().add([&](double delta) {
		device.queue().wait_idle();

		uint32_t currentBuffer = wnd.swap_chain().acquire_next_image(presentCompleteSemaphore);
		drawCmdBuffers[currentBuffer].submit(device, { presentCompleteSemaphore }, { vk::PipelineStageFlagBits::eBottomOfPipe }, { renderCompleteSemaphore });
		wnd.swap_chain().queuePresent(currentBuffer, { renderCompleteSemaphore });

		postPresentCmdBuffer.begin();
		postPresentCmdBuffer.pipeline_barrier(
			vk::ImageMemoryBarrier()
				.srcAccessMask(static_cast<vk::AccessFlagBits>(0))
				.dstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
				.oldLayout(vk::ImageLayout::ePresentSrcKHR)
				.newLayout(vk::ImageLayout::eColorAttachmentOptimal)
				.srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
				.dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
				.subresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1))
				.image(wnd.swap_chain().buffers()[currentBuffer].image.handle()));
		postPresentCmdBuffer.end();
		postPresentCmdBuffer.submit(device);
	});
	wnd.run(60);
}
