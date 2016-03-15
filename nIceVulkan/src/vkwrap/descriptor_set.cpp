#include "stdafx.h"
#include "vkwrap/descriptor_set.h"
#include "util/setops.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	descriptor_set::descriptor_set(const std::vector<descriptor_set_layout> &setLayouts, const descriptor_pool &pool, const ibuffer &buffer) :
		pool_(pool)
	{
		vector<vk::DescriptorSetLayout> handles = set::from(setLayouts)
			.select([](const descriptor_set_layout &x) { return x.handle(); })
			.to_vector();

		handle_ = pool.parent_device().allocate_descriptor_sets(
			vk::DescriptorSetAllocateInfo()
				.descriptorPool(pool.handle())
				.descriptorSetCount(1)
				.pSetLayouts(handles.data()))[0];

		auto descriptor = vk::DescriptorBufferInfo()
			.buffer(buffer.handle())
			.offset(0)
			.range(static_cast<vk::DeviceSize>(buffer.size()));

		pool.parent_device().update_descriptor_sets({ 
			vk::WriteDescriptorSet()
				.dstSet(handle_)
				.descriptorCount(1)
				.descriptorType(vk::DescriptorType::eUniformBuffer)
				.pBufferInfo(&descriptor)
				.dstBinding(0)
		});
	}

	descriptor_set::~descriptor_set()
	{
		pool_.parent_device().free_descriptor_sets(pool_.handle(), { handle_ });
	}

	vk::DescriptorSet descriptor_set::handle() const
	{
		return handle_;
	}
}
