#include "stdafx.h"
#include "render_pass.h"
#include <vector>

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

		vk::AttachmentReference colorReference;
		colorReference.attachment(0);
		colorReference.layout(vk::ImageLayout::eColorAttachmentOptimal);

		vk::AttachmentReference depthReference;
		depthReference.attachment(1);
		depthReference.layout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		vk::SubpassDescription subpass;
		subpass.pipelineBindPoint(vk::PipelineBindPoint::eGraphics);
		subpass.flags(0);
		subpass.inputAttachmentCount(0);
		subpass.pInputAttachments(nullptr);
		subpass.colorAttachmentCount(1);
		subpass.pColorAttachments(&colorReference);
		subpass.pResolveAttachments(nullptr);
		subpass.pDepthStencilAttachment(&depthReference);
		subpass.preserveAttachmentCount(0);
		subpass.pPreserveAttachments(nullptr);

		vk::RenderPassCreateInfo renderPassInfo;
		renderPassInfo.attachmentCount(2);
		renderPassInfo.pAttachments(attachments);
		renderPassInfo.subpassCount(1);
		renderPassInfo.pSubpasses(&subpass);
		renderPassInfo.dependencyCount(0);
		renderPassInfo.pDependencies(nullptr);

		vk::createRenderPass(device.handle(), &renderPassInfo, nullptr, &handle_);
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
