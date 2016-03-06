#pragma once
#include "vkwrap/pipeline.h"

namespace nif
{
	class shader
	{
	public:
		shader(
			const render_pass &pass,
			const pipeline_cache &cache,
			const std::string &vertsrc,
			const std::string &fragsrc,
			vk::PipelineVertexInputStateCreateInfo vertinfo);
		~shader();

		const std::vector<shader_module>& modules() const;
		const pipeline& pipeline() const;
		const pipeline_layout& pipeline_layout() const;
		const std::vector<descriptor_set_layout>& descriptor_set_layouts() const;

	private:
		std::vector<shader_module> modules_;
		nif::pipeline pipeline_;
		nif::pipeline_layout pipeline_layout_;
		std::vector<descriptor_set_layout> descriptor_set_layouts_;
	};
}
