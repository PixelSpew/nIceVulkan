#pragma once
#include "buffer.h"
#include "descriptor_pool.h"
#include "descriptor_set_layout.h"

namespace nif
{
	class descriptor_set
	{
	public:
		descriptor_set(const std::initializer_list<std::reference_wrapper<const descriptor_set_layout>> setLayouts, const descriptor_pool &pool, const ibuffer &buffer);
		~descriptor_set();
		size_t size() const;
		const std::vector<vk::DescriptorSet>& handles() const;

	private:
		std::vector<vk::DescriptorSet> handles_;
		const device &device_;
		const descriptor_pool &pool_;
	};
}
