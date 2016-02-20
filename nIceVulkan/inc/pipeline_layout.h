#pragma once
#include "descriptor_set_layout.h"

namespace nif
{
	class pipeline_layout
	{
	public:
		pipeline_layout(const std::initializer_list<std::reference_wrapper<const descriptor_set_layout>> descSetLayouts);
		~pipeline_layout();
		vk::PipelineLayout handle() const;

	private:
		vk::PipelineLayout handle_;
		const device &device_;
	};
}