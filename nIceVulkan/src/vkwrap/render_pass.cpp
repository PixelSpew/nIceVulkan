#include "stdafx.h"
#include "vkwrap/render_pass.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	render_pass::render_pass(const device &device)
		: device_(device)
	{
		vector<vk::AttachmentDescription> attachments = {
			vk::AttachmentDescription()
				.format(vk::Format::eB8G8R8A8Unorm)
				.samples(vk::SampleCountFlagBits::e1)
				.loadOp(vk::AttachmentLoadOp::eClear)
				.storeOp(vk::AttachmentStoreOp::eStore)
				.stencilLoadOp(vk::AttachmentLoadOp::eDontCare)
				.stencilStoreOp(vk::AttachmentStoreOp::eDontCare)
				.initialLayout(vk::ImageLayout::eColorAttachmentOptimal)
				.finalLayout(vk::ImageLayout::eColorAttachmentOptimal),
			vk::AttachmentDescription()
				.format(device.depth_format())
				.samples(vk::SampleCountFlagBits::e1)
				.loadOp(vk::AttachmentLoadOp::eClear)
				.storeOp(vk::AttachmentStoreOp::eStore)
				.stencilLoadOp(vk::AttachmentLoadOp::eDontCare)
				.stencilStoreOp(vk::AttachmentStoreOp::eDontCare)
				.initialLayout(vk::ImageLayout::eColorAttachmentOptimal)
				.finalLayout(vk::ImageLayout::eColorAttachmentOptimal)
		};

		auto colorReference = vk::AttachmentReference()
			.attachment(0)
			.layout(vk::ImageLayout::eColorAttachmentOptimal);
		auto depthReference = vk::AttachmentReference()
			.attachment(1)
			.layout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		auto subpass = vk::SubpassDescription()
			.colorAttachmentCount(1)
			.pColorAttachments(&colorReference)
			.pDepthStencilAttachment(&depthReference);

		handle_ = device.create_render_pass(
			vk::RenderPassCreateInfo()
				.attachmentCount(static_cast<uint32_t>(attachments.size()))
				.pAttachments(attachments.data())
				.subpassCount(1)
				.pSubpasses(&subpass));
	}

	render_pass::~render_pass()
	{
		device_.destroy_render_pass(handle_);
	}

	vk::RenderPass render_pass::handle() const
	{
		return handle_;
	}

	const device& render_pass::parent_device() const
	{
		return device_;
	}
}
