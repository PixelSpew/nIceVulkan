#include "stdafx.h"
#include "vkwrap/image.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	image::image(const int width, const int height, const device &device) :
		device_(device)
	{
		handle_ = device.create_image(
			vk::ImageCreateInfo()
				.imageType(vk::ImageType::e2D)
				.format(device.depth_format())
				.extent(vk::Extent3D(width, height, 1))
				.mipLevels(1)
				.arrayLayers(1)
				.samples(vk::SampleCountFlagBits::e1)
				.tiling(vk::ImageTiling::eOptimal)
				.usage(vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc));

		gpumem_ = gpu_memory(device,
			device.get_image_memory_requirements(handle_),
			vk::MemoryPropertyFlagBits::eDeviceLocal,
			nullptr);
		device.bind_image_memory(handle_, gpumem_.handle(), 0);
	}

	image::image(const device &device, const vk::Image handle) :
		device_(device),
		handle_(handle)
	{
	}

	image::image(image &&old) :
		handle_(old.handle_),
		gpumem_(move(old.gpumem_)),
		device_(old.device_)
	{
	}

	image::~image()
	{
		if (gpumem_.handle())
			device_.destroy_image(handle_);
	}

	vk::Image image::handle() const
	{
		return handle_;
	}

	const device& image::parent_device() const
	{
		return device_;
	}
}
