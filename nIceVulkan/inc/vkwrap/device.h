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
		const physical_device& physical_device() const;
		vk::Format depth_format() const;
		vk::Queue queue() const;

	private:
		vk::Device handle_;
		const instance &instance_;
		const nif::physical_device &physical_device_;
		vk::Format depth_format_;
		vk::Queue queue_;
	};
}
