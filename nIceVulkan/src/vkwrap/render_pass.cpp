#include "stdafx.h"
#include "vkwrap/render_pass.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	render_pass::render_pass(const device &device)
		: device_(device)
	{
		vk::AttachmentDescription attachments[2];
		attachments[0].format(vk::Format::eB8G8R8A8Unorm);
		attachments[0].samples(vk::SampleCountFlagBits::e1);
		attachments[0].loadOp(vk::AttachmentLoadOp::eClear);
		attachments[0].storeOp(vk::AttachmentStoreOp::eStore);
		attachments[0].stencilLoadOp(vk::AttachmentLoadOp::eDontCare);
		attachments[0].stencilStoreOp(vk::AttachmentStoreOp::eDontCare);
		attachments[0].initialLayout(vk::ImageLayout::eColorAttachmentOptimal);
		attachments[0].finalLayout(vk::ImageLayout::eColorAttachmentOptimal);

		attachments[1].format(device.depth_format());
		attachments[1].samples(vk::SampleCountFlagBits::e1);
		attachments[1].loadOp(vk::AttachmentLoadOp::eClear);
		attachments[1].storeOp(vk::AttachmentStoreOp::eStore);
		attachments[1].stencilLoadOp(vk::AttachmentLoadOp::eDontCare);
		attachments[1].stencilStoreOp(vk::AttachmentStoreOp::eDontCare);
		attachments[1].initialLayout(vk::ImageLayout::eColorAttachmentOptimal);
		attachments[1].finalLayout(vk::ImageLayout::eColorAttachmentOptimal);

		vk::AttachmentReference colorReference(0, vk::ImageLayout::eColorAttachmentOptimal);
		vk::AttachmentReference depthReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

		vk::SubpassDescription subpass;
		subpass.colorAttachmentCount(1);
		subpass.pColorAttachments(&colorReference);
		subpass.pDepthStencilAttachment(&depthReference);

		vk::RenderPassCreateInfo renderPassInfo;
		renderPassInfo.attachmentCount(2);
		renderPassInfo.pAttachments(attachments);
		renderPassInfo.subpassCount(1);
		renderPassInfo.pSubpasses(&subpass);

		vk_try(vk::createRenderPass(device.handle(), &renderPassInfo, nullptr, &handle_));
	}

	render_pass::~render_pass()
	{
		vk::destroyRenderPass(device_.handle(), handle_, nullptr);
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
