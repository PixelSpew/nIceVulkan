#include "stdafx.h"
#include "vkwrap/image.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	image::image(const int width, const int height, const device &device) :
		device_(device)
	{
		vk::ImageCreateInfo createInfo;
		createInfo.imageType(vk::ImageType::e2D);
		createInfo.format(device.depth_format());
		createInfo.extent(vk::Extent3D(width, height, 1));
		createInfo.mipLevels(1);
		createInfo.arrayLayers(1);
		createInfo.samples(vk::SampleCountFlagBits::e1);
		createInfo.tiling(vk::ImageTiling::eOptimal);
		createInfo.usage(vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc);
		vk_try(vk::createImage(device.handle(), &createInfo, nullptr, &handle_));

		vk::MemoryRequirements memreqs;
		vk::getImageMemoryRequirements(device.handle(), handle_, &memreqs);
		gpumem_ = gpu_memory(device, memreqs, vk::MemoryPropertyFlagBits::eDeviceLocal, nullptr);
		vk_try(vk::bindImageMemory(device.handle(), handle_, gpumem_.handle(), 0));
	}

	image::image(const device &device, const vk::Image handle) :
		device_(device),
		handle_(handle)
	{
	}

	image::image(image &&old) :
		handle_(old.handle_),
		gpumem_(move(old.gpumem_)),
		device_(old.device_) {}

	image::~image()
	{
		if (gpumem_.handle())
			vk::destroyImage(device_.handle(), handle_, nullptr);
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
