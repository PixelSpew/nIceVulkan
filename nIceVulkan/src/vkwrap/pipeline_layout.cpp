#include "stdafx.h"
#include "vkwrap/pipeline_layout.h"
#include "util/setops.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	pipeline_layout::pipeline_layout()
	{
	}

	pipeline_layout::pipeline_layout(const vector<descriptor_set_layout> &descSetLayouts) :
		device_(&descSetLayouts[0].parent_device())
	{
		auto handles = set::from(descSetLayouts)
			.select([](const descriptor_set_layout &x) { return x.handle(); })
			.to_vector();

		device_->create_pipeline_layout(
			vk::PipelineLayoutCreateInfo()
				.setLayoutCount(static_cast<uint32_t>(handles.size()))
				.pSetLayouts(handles.data()));
	}

	pipeline_layout::pipeline_layout(pipeline_layout &&old) :
		handle_(old.handle_),
		device_(old.device_)
	{
		old.handle_ = nullptr;
	}

	pipeline_layout::~pipeline_layout()
	{
		if (handle_)
			device_->destroy_pipeline_layout(handle_);
	}

	vk::PipelineLayout pipeline_layout::handle() const
	{
		return handle_;
	}

	pipeline_layout& pipeline_layout::operator=(pipeline_layout &&rhs)
	{
		if (handle_ != rhs.handle_) {
			handle_ = rhs.handle_;
			device_ = rhs.device_;
			rhs.handle_ = nullptr;
		}

		return *this;
	}
}
