#include "stdafx.h"
#include "shader_module.h"
#include <vector>

using namespace std;

namespace nif
{
	shader_module::shader_module(const device &device, const string &source, const vk::ShaderStageFlagBits stage)
		: device_(device), stage_(stage)
	{
		vector<uint32_t> pcode;
		pcode.push_back(0x07230203);
		pcode.push_back(0);
		pcode.push_back(static_cast<uint32_t>(stage));
		pcode.insert(pcode.end(), source.begin(), source.end());

		vk::ShaderModuleCreateInfo moduleCreateInfo;
		moduleCreateInfo.codeSize(pcode.size() * sizeof(uint32_t));
		moduleCreateInfo.pCode(pcode.data());

		if (vk::createShaderModule(device.handle(), &moduleCreateInfo, nullptr, &handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");
	}

	shader_module::shader_module(shader_module &&old)
		: handle_(old.handle_), device_(move(old.device_)), stage_(old.stage_)
	{
	}

	shader_module::~shader_module()
	{
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
