#include "stdafx.h"
#include "vkwrap/descriptor_set.h"
#include "util/setops.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	descriptor_set::descriptor_set(const std::vector<descriptor_set_layout> &setLayouts, const descriptor_pool &pool) :
		pool_(pool)
	{
		vector<vk::DescriptorSetLayout> handles = set::from(setLayouts)
			.select([](const descriptor_set_layout &x) { return x.handle(); })
			.to_vector();

		handle_ = pool.parent_device().allocate_descriptor_sets(
			vk::DescriptorSetAllocateInfo()
				.descriptorPool(pool.handle())
				.descriptorSetCount(static_cast<uint32_t>(handles.size()))
				.pSetLayouts(handles.data()))[0];
	}

	descriptor_set::~descriptor_set()
	{
		pool_.parent_device().free_descriptor_sets(pool_.handle(), { handle_ });
	}

	void descriptor_set::update_buffer(const vk::DescriptorType type, const uint32_t binding, const ibuffer &buffer)
	{
		auto descriptor = vk::DescriptorBufferInfo()
			.buffer(buffer.handle())
			.offset(0)
			.range(static_cast<vk::DeviceSize>(buffer.size()));

		pool_.parent_device().update_descriptor_sets({
			vk::WriteDescriptorSet()
				.dstSet(handle_)
				.descriptorCount(1)
				.descriptorType(type)
				.pBufferInfo(&descriptor)
				.dstBinding(binding)
		});
	}

	vk::DescriptorSet descriptor_set::handle() const
	{
		return handle_;
	}
}
