#include "stdafx.h"
#include "descriptor_set.h"
#include "util/linq.h"

#define DESCRIPTOR_SET_COUNT 1

namespace nif
{
	descriptor_set::descriptor_set(const std::initializer_list<std::reference_wrapper<const descriptor_set_layout>> setLayouts, const descriptor_pool &pool, const ibuffer &buffer)
		: device_(pool.parent_device()), pool_(pool)
	{
		vk::DescriptorSetAllocateInfo dsAllocInfo;
		dsAllocInfo.descriptorPool(pool.handle());
		dsAllocInfo.descriptorSetCount(DESCRIPTOR_SET_COUNT);
		dsAllocInfo.pSetLayouts(
			from(setLayouts)
			.select<vk::DescriptorSetLayout>([](const std::reference_wrapper<const nif::descriptor_set_layout> &x) { return x.get().handle(); })
			.to_vector()
			.data()
		);

		handles_.resize(DESCRIPTOR_SET_COUNT);
		vk::allocateDescriptorSets(pool.parent_device().handle(), &dsAllocInfo, handles_.data());

		vk::DescriptorBufferInfo descriptor;
		descriptor.buffer(buffer.handle());
		descriptor.offset(0);
		descriptor.range(static_cast<vk::DeviceSize>(buffer.size()));

		vk::WriteDescriptorSet writeDescriptorSet;
		writeDescriptorSet.dstSet(handles_[0]);
		writeDescriptorSet.descriptorCount(1);
		writeDescriptorSet.descriptorType(vk::DescriptorType::eUniformBuffer);
		writeDescriptorSet.pBufferInfo(&descriptor);
		writeDescriptorSet.dstBinding(0);

		vk::updateDescriptorSets(pool.parent_device().handle(), 1, &writeDescriptorSet, 0, NULL);
	}

	descriptor_set::~descriptor_set()
	{
		vk::freeDescriptorSets(device_.handle(), pool_.handle(), DESCRIPTOR_SET_COUNT, handles_.data());
	}
}
