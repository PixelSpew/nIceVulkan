#pragma once
#include "vkwrap/descriptor_set_layout.h"

namespace nif
{
	class pipeline_layout
	{
	public:
		pipeline_layout();
		explicit pipeline_layout(const std::vector<descriptor_set_layout> &descSetLayouts);
		pipeline_layout(const pipeline_layout&) = delete;
		pipeline_layout(pipeline_layout &&old);
		~pipeline_layout();
		vk::PipelineLayout handle() const;

		pipeline_layout& operator=(pipeline_layout &&rhs);

	private:
		vk::PipelineLayout handle_;
		const device *device_;
	};
}
