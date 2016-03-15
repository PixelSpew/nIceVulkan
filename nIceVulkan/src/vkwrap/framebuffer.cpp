#include "stdafx.h"
#include "vkwrap/framebuffer.h"
#include "util/setops.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	framebuffer::framebuffer(
		const uint32_t width,
		const uint32_t height,
		const render_pass &pass,
		const std::initializer_list<std::reference_wrapper<const image_view>> views) :
		device_(pass.parent_device())
	{
		vector<vk::ImageView> handles = set::from(views)
			.select([](const reference_wrapper<const image_view> &x) { return x.get().handle(); })
			.to_vector();

		handle_ = pass.parent_device().create_framebuffer(
			vk::FramebufferCreateInfo()
				.renderPass(pass.handle())
				.attachmentCount(static_cast<uint32_t>(handles.size()))
				.pAttachments(handles.data())
				.width(width)
				.height(height)
				.layers(1));
	}

	framebuffer::framebuffer(framebuffer &&old) :
		handle_(old.handle_),
		device_(move(old.device_))
	{
		old.handle_ = nullptr;
	}

	framebuffer::~framebuffer()
	{
		if (handle_)
			device_.destroy_framebuffer(handle_);
	}

	vk::Framebuffer framebuffer::handle() const
	{
		return handle_;
	}
}
