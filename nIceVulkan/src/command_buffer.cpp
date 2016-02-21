#include "stdafx.h"
#include "command_buffer.h"
#include "util/linq.h"

using namespace std;

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

	void command_buffer::end()
	{
		vk::endCommandBuffer(handle_);
	}

	void command_buffer::submit(const device &device)
	{
		vk::CommandBuffer cmdbufferHandle = handle_;

		vk::SubmitInfo submitInfo;
		submitInfo.commandBufferCount(1);
		submitInfo.pCommandBuffers(&cmdbufferHandle);

		vk::queueSubmit(device.queue(), 1, &submitInfo, VK_NULL_HANDLE);
	}

	void command_buffer::submit(const device &device, const semaphore &semaphore)
	{
		vk::Semaphore semaphoreHandle = semaphore.handle();
		vk::CommandBuffer cmdbufferHandle = handle_;

		vk::SubmitInfo submitInfo;
		submitInfo.waitSemaphoreCount(1);
		submitInfo.pWaitSemaphores(&semaphoreHandle);
		submitInfo.commandBufferCount(1);
		submitInfo.pCommandBuffers(&cmdbufferHandle);

		vk::queueSubmit(device.queue(), 1, &submitInfo, VK_NULL_HANDLE);
	}

	void command_buffer::begin_render_pass(const render_pass &pass, const framebuffer &framebuffer, uint32_t width, uint32_t height)
	{
		vk::ClearValue clearValues[2];
		array<float, 4> clearColor = { .2f, .2f, .2f, 1 };
		clearValues[0].color(vk::ClearColorValue(clearColor));
		clearValues[1].depthStencil(vk::ClearDepthStencilValue(1.0f, 0));

		vk::RenderPassBeginInfo beginInfo;
		beginInfo.renderPass(pass.handle());
		beginInfo.renderArea(vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(width, height)));
		beginInfo.clearValueCount(2);
		beginInfo.pClearValues(clearValues);
		beginInfo.framebuffer(framebuffer.handle());

		vk::cmdBeginRenderPass(handle_, &beginInfo, vk::SubpassContents::eInline);
	}

	void command_buffer::end_render_pass()
	{
		vkCmdEndRenderPass(handle_);
	}

	void command_buffer::set_viewport(const float width, const float height)
	{
		vk::Viewport viewport;
		viewport.height(height);
		viewport.width(width);
		viewport.minDepth(0.0f);
		viewport.maxDepth(1.0f);
		vk::cmdSetViewport(handle_, 0, 1, &viewport);
	}

	void command_buffer::set_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
	{
		vk::Rect2D scissor(vk::Offset2D(x, y), vk::Extent2D(width, height));
		vk::cmdSetScissor(handle_, 0, 1, &scissor);
	}

	void command_buffer::bind_descriptor_sets(const pipeline_layout &pipelayout, const descriptor_set &descset)
	{
		vk::cmdBindDescriptorSets(handle_, vk::PipelineBindPoint::eGraphics, pipelayout.handle(), 0, static_cast<uint32_t>(descset.size()), descset.handles().data(), 0, nullptr);
	}

	void command_buffer::bind_pipeline(const pipeline &pipeline)
	{
		vk::cmdBindPipeline(handle_, vk::PipelineBindPoint::eGraphics, pipeline.handle());
	}

	void command_buffer::bind_vertex_buffer(const ibuffer &buffer)
	{
		vk::Buffer bufhandle = buffer.handle();
		vector<vk::DeviceSize> offsets = { 0 };
		vk::cmdBindVertexBuffers(handle_, 0, 1, &bufhandle, offsets.data());
	}

	void command_buffer::bind_index_buffer(const buffer<uint32_t> &buffer)
	{
		vk::cmdBindIndexBuffer(handle_, buffer.handle(), 0, vk::IndexType::eUint32);
	}

	void command_buffer::draw_indexed(const uint32_t indexCount)
	{
		vk::cmdDrawIndexed(handle_, indexCount, 1, 0, 0, 1);
	}

	void command_buffer::pipeline_barrier(const image &image)
	{
		vk::ImageMemoryBarrier barrier;
		barrier.srcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
		barrier.dstAccessMask(static_cast<vk::AccessFlagBits>(0));
		barrier.oldLayout(vk::ImageLayout::eColorAttachmentOptimal);
		barrier.newLayout(static_cast<vk::ImageLayout>(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR));
		barrier.srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		barrier.dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		barrier.subresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
		barrier.image(image.handle());

		vk::cmdPipelineBarrier(
			handle_,
			vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlagBits::eTopOfPipe,
			static_cast<vk::DependencyFlagBits>(0),
			0, nullptr,
			0, nullptr,
			1, &barrier);
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

	const device& command_buffer::parent_device() const
	{
		return pool_.parent_device();
	}
}
