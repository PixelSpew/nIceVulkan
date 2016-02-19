#include "stdafx.h"
#include "image_view.h"

namespace nif
{
	image_view::image_view(const image &image)
		: device_(image.parent_device())
	{
		vk::ImageViewCreateInfo depthStencilView;
		depthStencilView.viewType(vk::ImageViewType::e2D);
		depthStencilView.format(device_.depth_format());
		depthStencilView.subresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil, 0, 1, 0, 1));
		depthStencilView.image(image.handle());
		vk::createImageView(device_.handle(), &depthStencilView, nullptr, &handle_);
	}

	image_view::~image_view()
	{
		vk::destroyImageView(device_.handle(), handle_, nullptr);
	}

	vk::ImageView image_view::handle() const
	{
		return handle_;
	}
}