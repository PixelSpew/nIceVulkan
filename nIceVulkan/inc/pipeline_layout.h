#pragma once
#include "descriptor_set_layout.h"

namespace nif
{
	class pipeline_layout
	{
	public:
		pipeline_layout(const std::vector<descriptor_set_layout> &descSetLayouts);
		pipeline_layout(const pipeline_layout&) = delete;
		~pipeline_layout();
		vk::PipelineLayout handle() const;

	private:
		vk::PipelineLayout handle_;
		const device &device_;
	};
}