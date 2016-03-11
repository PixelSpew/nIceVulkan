#pragma once
#include "vkwrap/device.h"

namespace nif
{
	class sampler
	{
	public:
		sampler(const device &device);
		sampler(const sampler&) = delete;
		~sampler();
		vk::Sampler handle() const;

	private:
		vk::Sampler handle_;
		const device &device_;
	};
}