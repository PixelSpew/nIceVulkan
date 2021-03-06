#pragma once
#include "vkwrap/image.h"

namespace nif
{
	class image_view
	{
	public:
		image_view(const image &image, const vk::Format format, vk::ImageAspectFlags aspectFlags);
		image_view(const image_view&) = delete;
		image_view(image_view &&old);
		~image_view();
		vk::ImageView handle() const;

	private:
		vk::ImageView handle_;
		const device &device_;
	};
}