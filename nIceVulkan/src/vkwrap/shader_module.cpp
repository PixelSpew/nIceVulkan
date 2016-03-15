#include "stdafx.h"
#include "vkwrap/shader_module.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	shader_module::shader_module(const device &device, const vector<char> &source, const vk::ShaderStageFlagBits stage) :
		device_(device),
		stage_(stage)
	{
		device.create_shader_module(
			vk::ShaderModuleCreateInfo()
				.codeSize(source.size())
				.pCode(reinterpret_cast<const uint32_t*>(source.data())));
	}

	shader_module::shader_module(shader_module &&old) :
		handle_(old.handle_),
		device_(move(old.device_)),
		stage_(old.stage_)
	{
		old.handle_ = nullptr;
	}

	shader_module::~shader_module()
	{
		if (handle_)
			device_.destroy_shader_module(handle_);
	}

	vk::ShaderModule shader_module::handle() const
	{
		return handle_;
	}

	vk::ShaderStageFlagBits shader_module::stage() const
	{
		return stage_;
	}
}
