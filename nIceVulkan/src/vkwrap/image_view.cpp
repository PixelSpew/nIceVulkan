#include "stdafx.h"
#include "vkwrap/image_view.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	image_view::image_view(const image &image, const vk::Format format, vk::ImageAspectFlags aspectFlags)
		: device_(image.parent_device())
	{
		vk::ImageViewCreateInfo createInfo;
		createInfo.viewType(vk::ImageViewType::e2D);
		createInfo.format(format);
		createInfo.subresourceRange(vk::ImageSubresourceRange(aspectFlags, 0, 1, 0, 1));
		createInfo.image(image.handle());
		vk_try(vk::createImageView(device_.handle(), &createInfo, nullptr, &handle_));
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
			vk::destroyImageView(device_.handle(), handle_, nullptr);
	}

	vk::ImageView image_view::handle() const
	{
		return handle_;
	}
}
