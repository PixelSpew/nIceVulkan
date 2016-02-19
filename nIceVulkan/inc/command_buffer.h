#pragma once
#include "command_pool.h"
#include "image.h"

namespace nif
{
	class command_buffer
	{
	public:
		command_buffer(const command_pool &pool);
		~command_buffer();
		void begin();
		void setImageLayout(const image &image, const vk::ImageAspectFlags &aspectMask, const vk::ImageLayout oldImageLayout, const vk::ImageLayout newImageLayout);
		vk::CommandBuffer handle() const;

	private:
		vk::CommandBuffer handle_;
		vk::CommandBufferBeginInfo begin_info_;
		const command_pool &pool_;
	};
}
