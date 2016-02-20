#include "stdafx.h"
#include "descriptor_set_layout.h"

namespace nif
{
	descriptor_set_layout::descriptor_set_layout(const device &device)
		: device_(device)
	{
		vk::DescriptorSetLayoutBinding layoutBinding;
		layoutBinding.descriptorType(vk::DescriptorType::eUniformBuffer);
		layoutBinding.descriptorCount(1);
		layoutBinding.stageFlags(vk::ShaderStageFlagBits::eVertex);

		vk::DescriptorSetLayoutCreateInfo descriptorLayout;
		descriptorLayout.bindingCount(1);
		descriptorLayout.pBindings(&layoutBinding);

		vk::createDescriptorSetLayout(device.handle(), &descriptorLayout, NULL, &handle_);
	}

	descriptor_set_layout::~descriptor_set_layout()
	{
		vk::destroyDescriptorSetLayout(device_.handle(), handle_, nullptr);
	}

	vk::DescriptorSetLayout descriptor_set_layout::handle() const
	{
		return handle_;
	}

	const device & descriptor_set_layout::parent_device() const
	{
		return device_;
	}
}
