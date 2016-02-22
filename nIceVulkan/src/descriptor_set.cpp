#include "stdafx.h"
#include "descriptor_set.h"

#define DESCRIPTOR_SET_COUNT 1

using namespace std;

namespace nif
{
	descriptor_set::descriptor_set(const std::vector<descriptor_set_layout> &setLayouts, const descriptor_pool &pool, const ibuffer &buffer)
		: device_(pool.parent_device()), pool_(pool)
	{
		vector<vk::DescriptorSetLayout> handles;
		for (auto &layout : setLayouts)
			handles.push_back(layout.handle());

		vk::DescriptorSetAllocateInfo dsAllocInfo;
		dsAllocInfo.descriptorPool(pool.handle());
		dsAllocInfo.descriptorSetCount(DESCRIPTOR_SET_COUNT);
		dsAllocInfo.pSetLayouts(handles.data());

		handles_.resize(DESCRIPTOR_SET_COUNT);
		if (vk::allocateDescriptorSets(pool.parent_device().handle(), &dsAllocInfo, handles_.data()) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

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

	size_t descriptor_set::size() const
	{
		return handles_.size();
	}

	const std::vector<vk::DescriptorSet>& descriptor_set::handles() const
	{
		return handles_;
	}
}
