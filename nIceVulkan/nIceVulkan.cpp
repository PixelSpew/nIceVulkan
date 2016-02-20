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
}

