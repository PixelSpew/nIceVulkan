#include "stdafx.h"
#include "descriptor_pool.h"

namespace nif
{
	descriptor_pool::descriptor_pool(const device &device)
		: device_(device)
	{
		vk::DescriptorPoolSize typeCounts[1];
		typeCounts[0].type(vk::DescriptorType::eUniformBuffer);
		typeCounts[0].descriptorCount(1);

		vk::DescriptorPoolCreateInfo descriptorPoolInfo;
		descriptorPoolInfo.poolSizeCount(1);
		descriptorPoolInfo.pPoolSizes(typeCounts);
		descriptorPoolInfo.maxSets(1);

		vk::createDescriptorPool(device.handle(), &descriptorPoolInfo, nullptr, &handle_);
	}

	descriptor_pool::~descriptor_pool()
	{
		vk::destroyDescriptorPool(device_.handle(), handle_, nullptr);
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
