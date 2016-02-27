#pragma once
#include "vkwrap/buffer.h"
#include "vkwrap/descriptor_pool.h"
#include "vkwrap/descriptor_set_layout.h"

namespace nif
{
	class descriptor_set
	{
	public:
		explicit descriptor_set(const std::vector<descriptor_set_layout> &setLayouts, const descriptor_pool &pool, const ibuffer &buffer);
		descriptor_set(const descriptor_set&) = delete;
		~descriptor_set();
		size_t size() const;
		const std::vector<vk::DescriptorSet>& handles() const;

	private:
		std::vector<vk::DescriptorSet> handles_;
		const device &device_;
		const descriptor_pool &pool_;
	};
}
