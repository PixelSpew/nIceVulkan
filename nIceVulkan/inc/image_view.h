#pragma once
#include "image.h"

namespace nif
{
	class image_view
	{
	public:
		image_view(const image &image, const vk::Format format, vk::ImageAspectFlags aspectFlags);
		~image_view();
		vk::ImageView handle() const;

	private:
		vk::ImageView handle_;
		const device &device_;
	};
}