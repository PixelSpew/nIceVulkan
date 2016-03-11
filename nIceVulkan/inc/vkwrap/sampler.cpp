#include "stdafx.h"
#include "sampler.h"

namespace nif
{
	sampler::sampler(const device &device) :
		device_(device)
	{
		vk::SamplerCreateInfo createInfo;
		createInfo.magFilter(vk::Filter::eLinear);
		createInfo.minFilter(vk::Filter::eLinear);
		createInfo.mipmapMode(vk::SamplerMipmapMode::eLinear);

		vk::createSampler(device.handle(), &createInfo, nullptr, &handle_);
	}

	sampler::~sampler()
	{
		vk::destroySampler(device_.handle(), handle_, nullptr);
	}

	vk::Sampler sampler::handle() const
	{
		return handle_;
	}
}
