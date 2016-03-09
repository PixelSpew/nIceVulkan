#pragma once
#include "vkwrap/device.h"

namespace nif
{
	class shader_module
	{
	public:
		shader_module(const device &device, const std::vector<char> &source, const vk::ShaderStageFlagBits stage);
		shader_module(const shader_module&) = delete;
		shader_module(shader_module &&old);
		~shader_module();
		vk::ShaderModule handle() const;
		vk::ShaderStageFlagBits stage() const;

	private:
		vk::ShaderModule handle_;
		const device &device_;
		vk::ShaderStageFlagBits stage_;
	};
}