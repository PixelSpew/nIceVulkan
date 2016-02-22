#include "stdafx.h"
#include "vkwrap/shader_module.h"

using namespace std;

namespace nif
{
	shader_module::shader_module(const device &device, const string &source, const vk::ShaderStageFlagBits stage)
		: device_(device), stage_(stage)
	{
		vk::ShaderModuleCreateInfo moduleCreateInfo;
		moduleCreateInfo.codeSize(source.size());
		moduleCreateInfo.pCode(reinterpret_cast<const uint32_t*>(source.data()));

		if (vk::createShaderModule(device.handle(), &moduleCreateInfo, nullptr, &handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");
	}

	shader_module::shader_module(shader_module &&old)
		: handle_(old.handle_), device_(move(old.device_)), stage_(old.stage_)
	{
		old.handle_ = nullptr;
	}

	shader_module::~shader_module()
	{
		if (handle_)
			vk::destroyShaderModule(device_.handle(), handle_, nullptr);
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
