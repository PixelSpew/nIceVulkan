#pragma once
#include "gpu_memory.h"
#include <memory>

namespace nif
{
	class image
	{
	public:
		image(const int width, const int height, const device &device);
		~image();
		vk::Image handle() const;
		const device& parent_device() const;

		static image wrap(const device &device, const vk::Image handle);

	private:
		image(const device &device, const vk::Image handle);
		image(const image& image);

		vk::Image handle_;
		std::unique_ptr<gpu_memory> gpumem_;
		const device &device_;
	};
}
