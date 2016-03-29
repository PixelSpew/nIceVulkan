#include "stdafx.h"
#include "vkwrap/descriptor_pool.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	descriptor_pool::descriptor_pool(const device &device) :
		device_(device)
	{
		vk::DescriptorPoolSize poolSize = vk::DescriptorPoolSize()
			.type(vk::DescriptorType::eUniformBuffer)
			.descriptorCount(1);

		handle_ = device.create_descriptor_pool(
			vk::DescriptorPoolCreateInfo()
				.poolSizeCount(1)
				.pPoolSizes(&poolSize)
				.maxSets(1));
	}

	descriptor_pool::~descriptor_pool()
	{
		device_.destroy_descriptor_pool(handle_);
	}

	vk::DescriptorPool descriptor_pool::handle() const
	{
		return handle_;
	}

	const device & descriptor_pool::parent_device() const
	{
		return device_;
	}
}
