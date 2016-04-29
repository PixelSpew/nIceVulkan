#include "stdafx.h"
#include "vkwrap/descriptor_set_layout.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	descriptor_set_layout::descriptor_set_layout(const device &device, const vector<vk::DescriptorSetLayoutBinding> &bindings) :
		device_(device)
	{
		handle_ = device.create_descriptor_set_layout(
			vk::DescriptorSetLayoutCreateInfo()
				.bindingCount(static_cast<uint32_t>(bindings.size()))
				.pBindings(bindings.data()));
	}

	descriptor_set_layout::descriptor_set_layout(descriptor_set_layout &&old) :
		handle_(old.handle_),
		device_(move(old.device_))
	{
		old.handle_ = nullptr;
	}

	descriptor_set_layout::~descriptor_set_layout()
	{
		if (handle_)
			device_.destroy_descriptor_set_layout(handle_);
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
