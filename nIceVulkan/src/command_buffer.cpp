#include "stdafx.h"
#include "command_buffer.h"

namespace nif
{
	command_buffer::command_buffer(const command_pool &pool)
		: pool_(pool)
	{
		vk::CommandBufferAllocateInfo allocateInfo;
		allocateInfo.commandPool(pool.handle());
		allocateInfo.level(vk::CommandBufferLevel::ePrimary);
		allocateInfo.commandBufferCount(1);

		vk::allocateCommandBuffers(pool.parent_device().handle(), &allocateInfo, &handle_);
	}

	command_buffer::~command_buffer()
	{
		vk::freeCommandBuffers(pool_.parent_device().handle(), pool_.handle(), 1, &handle_);
	}

	void command_buffer::begin()
	{
		vk::beginCommandBuffer(handle_, &begin_info_);
	}

	void command_buffer::setImageLayout(const image &image, const vk::ImageAspectFlags &aspectMask, const vk::ImageLayout oldImageLayout, const vk::ImageLayout newImageLayout)
	{
		vk::ImageMemoryBarrier imgMemBarrier;
		imgMemBarrier.srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		imgMemBarrier.dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		imgMemBarrier.oldLayout(oldImageLayout);
		imgMemBarrier.newLayout(newImageLayout);
		imgMemBarrier.image(image.handle());
		imgMemBarrier.subresourceRange(vk::ImageSubresourceRange(aspectMask, 0, 1, 0, 1));

		if (oldImageLayout == vk::ImageLayout::eUndefined)
			imgMemBarrier.srcAccessMask(vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite);
		else if (oldImageLayout == vk::ImageLayout::eColorAttachmentOptimal)
			imgMemBarrier.srcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
		else if (oldImageLayout == vk::ImageLayout::eTransferSrcOptimal)
			imgMemBarrier.srcAccessMask(vk::AccessFlagBits::eTransferRead);
		else if (oldImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
			imgMemBarrier.srcAccessMask(vk::AccessFlagBits::eShaderRead);

		if (newImageLayout == vk::ImageLayout::eTransferDstOptimal)
		{
			imgMemBarrier.dstAccessMask(vk::AccessFlagBits::eTransferWrite);
		}
		if (newImageLayout == vk::ImageLayout::eTransferSrcOptimal)
		{
			imgMemBarrier.srcAccessMask(imgMemBarrier.srcAccessMask() | vk::AccessFlagBits::eTransferRead);
			imgMemBarrier.dstAccessMask(vk::AccessFlagBits::eTransferRead);
		}
		if (newImageLayout == vk::ImageLayout::eColorAttachmentOptimal)
		{
			imgMemBarrier.dstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
			imgMemBarrier.srcAccessMask(vk::AccessFlagBits::eTransferRead);
		}
		if (newImageLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
		{
			imgMemBarrier.dstAccessMask(imgMemBarrier.dstAccessMask() | vk::AccessFlagBits::eDepthStencilAttachmentWrite);
		}
		if (newImageLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
		{
			imgMemBarrier.srcAccessMask(vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite);
			imgMemBarrier.dstAccessMask(vk::AccessFlagBits::eShaderRead);
		}

		vk::PipelineStageFlags srcStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;
		vk::PipelineStageFlags destStageFlags = vk::PipelineStageFlagBits::eTopOfPipe;

		// Put barrier inside setup command buffer
		vk::cmdPipelineBarrier(handle_, srcStageFlags, destStageFlags, vk::DependencyFlagBits::eByRegion, 0, nullptr, 0, nullptr, 1, &imgMemBarrier);
	}

	vk::CommandBuffer command_buffer::handle() const
	{
		return handle_;
	}
}
