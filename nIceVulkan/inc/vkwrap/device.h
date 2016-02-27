#pragma once
#include "vkwrap/instance.h"

namespace nif
{
	class device
	{
	public:
		explicit device(const instance &instance);
		device(const device&) = delete;
		~device();
		void wait_queue_idle() const;

		vk::Device handle() const;
		const instance& parent_instance() const;
		vk::PhysicalDevice physical_handle() const;
		const vk::PhysicalDeviceMemoryProperties& memory_properties() const;
		vk::Format depth_format() const;
		vk::Queue queue() const;

	private:
		vk::Device handle_;
		const instance &instance_;
		vk::PhysicalDevice physical_handle_;
		vk::PhysicalDeviceMemoryProperties memory_properties_;
		vk::Format depth_format_;
		vk::Queue queue_;
	};
}
