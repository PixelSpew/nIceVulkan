#include "stdafx.h"
#include "framebuffer.h"
#include "util/linq.h"

namespace nif
{
	framebuffer::framebuffer(const uint32_t width, const uint32_t height, const render_pass &pass, const std::initializer_list<std::reference_wrapper<const image_view>> views)
		: device_(pass.parent_device())
	{
		vk::FramebufferCreateInfo frameBufferCreateInfo;
		frameBufferCreateInfo.renderPass(pass.handle());
		frameBufferCreateInfo.attachmentCount(views.size());
		frameBufferCreateInfo.pAttachments(
			from(views)
				.select<vk::ImageView>([](const image_view &x) { return x.handle(); })
				.to_vector()
				.data()
		);
		frameBufferCreateInfo.width(width);
		frameBufferCreateInfo.height(height);
		frameBufferCreateInfo.layers(1);
		vk::createFramebuffer(pass.parent_device().handle(), &frameBufferCreateInfo, nullptr, &handle_);
	}

	framebuffer::~framebuffer()
	{
		vk::destroyFramebuffer(device_.handle(), handle_, nullptr);
	}
}
