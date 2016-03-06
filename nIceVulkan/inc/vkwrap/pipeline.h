#pragma once
#include "vkwrap/pipeline_layout.h"
#include "vkwrap/shader_module.h"
#include "vkwrap/render_pass.h"
#include "vkwrap/pipeline_cache.h"
#include <memory>

namespace nif
{
	class pipeline
	{
	public:
		pipeline();
		pipeline(
			const pipeline_layout &layout,
			const render_pass &pass,
			const std::vector<shader_module> &shaderModules,
			const vk::PipelineVertexInputStateCreateInfo &vertexInputStateCreateInfo,
			const pipeline_cache &cache);
		pipeline(const pipeline&) = delete;
		pipeline(pipeline &&old);
		~pipeline();
		vk::Pipeline handle() const;

		pipeline& operator=(pipeline &&rhs);

	private:
		vk::Pipeline handle_;
		const device *device_;
	};
}
