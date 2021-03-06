#pragma once
#include "vkwrap/gpu_memory.h"
#include <memory>

namespace nif
{
	class image
	{
	public:
		image(const int width, const int height, const device &device);
		/**
		* The handle passed to this constructor must be deleted externally
		*/
		image(const device &device, const vk::Image handle);
		image(const image&) = delete;
		image(image &&old);
		~image();
		vk::Image handle() const;
		const device& parent_device() const;

	private:
		vk::Image handle_;
		gpu_memory gpumem_;
		const device &device_;
	};
}
