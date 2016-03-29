#include "stdafx.h"
#include "vkwrap/image_view.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	image_view::image_view(const image &image, const vk::Format format, vk::ImageAspectFlags aspectFlags)
		: device_(image.parent_device())
	{
		handle_ = device_.create_image_view(
			vk::ImageViewCreateInfo()
				.viewType(vk::ImageViewType::e2D)
				.format(format)
				.subresourceRange(vk::ImageSubresourceRange(aspectFlags, 0, 1, 0, 1))
				.image(image.handle()));
	}

	image_view::image_view(image_view &&old)
		: handle_(old.handle_),
		  device_(old.device_)
	{
		old.handle_ = nullptr;
	}

	image_view::~image_view()
	{
		if (handle_)
			device_.destroy_image_view(handle_);
	}

	vk::ImageView image_view::handle() const
	{
		return handle_;
	}
}
