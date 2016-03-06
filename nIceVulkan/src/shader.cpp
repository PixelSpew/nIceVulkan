#include "stdafx.h"
#include "shader.h"

using namespace std;

namespace nif
{
	shader::shader(
		const render_pass &pass,
		const pipeline_cache& cache,
		const std::string &vertsrc,
		const std::string &fragsrc,
		const vk::PipelineVertexInputStateCreateInfo &vertinfo)
	{
		modules_.push_back(shader_module(pass.parent_device(), vertsrc, vk::ShaderStageFlagBits::eVertex));
		modules_.push_back(shader_module(pass.parent_device(), fragsrc, vk::ShaderStageFlagBits::eFragment));
		
		descriptor_set_layouts_.push_back(descriptor_set_layout(pass.parent_device()));
		pipeline_layout_ = nif::pipeline_layout(descriptor_set_layouts_);
		pipeline_ = nif::pipeline(pipeline_layout_, pass, modules_, vertinfo, cache);
	}

	shader::~shader()
	{
	}

	const pipeline_layout& shader::pipeline_layout() const
	{
		return pipeline_layout_;
	}

	const std::vector<shader_module>& shader::modules() const
	{
		return modules_;
	}

	const pipeline& shader::pipeline() const
	{
		return pipeline_;
	}

	const vector<descriptor_set_layout>& shader::descriptor_set_layouts() const
	{
		return descriptor_set_layouts_;
	}
}
