#pragma once
#include "pipeline_layout.h"
#include "shader_module.h"
#include "render_pass.h"
#include "pipeline_cache.h"
#include <vector>
#include <memory>

namespace nif
{
	class pipeline
	{
		pipeline(const pipeline&) = delete;

	public:
		pipeline(
			const pipeline_layout &layout,
			const render_pass &pass,
			const std::vector<std::unique_ptr<shader_module>> &shaderModules,
			const vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo,
			const pipeline_cache &cache);
		~pipeline();
		vk::Pipeline handle() const;

	private:
		vk::Pipeline handle_;
		const device &device_;
	};
}
