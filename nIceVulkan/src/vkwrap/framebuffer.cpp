#include "stdafx.h"
#include "vkwrap/framebuffer.h"

using namespace std;

namespace nif
{
	framebuffer::framebuffer(const uint32_t width, const uint32_t height, const render_pass &pass, const std::initializer_list<std::reference_wrapper<const image_view>> views)
		: device_(pass.parent_device())
	{
		vector<vk::ImageView> handles;
		for (auto &view : views)
			handles.push_back(view.get().handle());

		vk::FramebufferCreateInfo frameBufferCreateInfo;
		frameBufferCreateInfo.renderPass(pass.handle());
		frameBufferCreateInfo.attachmentCount(static_cast<uint32_t>(views.size()));
		frameBufferCreateInfo.pAttachments(handles.data());
		frameBufferCreateInfo.width(width);
		frameBufferCreateInfo.height(height);
		frameBufferCreateInfo.layers(1);
		if (vk::createFramebuffer(pass.parent_device().handle(), &frameBufferCreateInfo, nullptr, &handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");
	}

	framebuffer::framebuffer(framebuffer &&old)
		: handle_(old.handle_), device_(move(old.device_))
	{
		old.handle_ = nullptr;
	}

	framebuffer::~framebuffer()
	{
		if (handle_)
			vk::destroyFramebuffer(device_.handle(), handle_, nullptr);
	}

	vk::Framebuffer framebuffer::handle() const
	{
		return handle_;
	}
}
