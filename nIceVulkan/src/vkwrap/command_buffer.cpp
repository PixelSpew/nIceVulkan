#include "stdafx.h"
#include "vkwrap/command_buffer.h"
#include "util/shortcuts.h"
#include "util/setops.h"

using namespace std;

namespace nif
{
	command_buffer::command_buffer(const command_pool &pool) :
		pool_(pool)
	{
		handle_ = pool.parent_device().allocate_command_buffers(
			vk::CommandBufferAllocateInfo()
				.commandPool(pool.handle())
				.level(vk::CommandBufferLevel::ePrimary)
				.commandBufferCount(1))[0];
	}

	command_buffer::command_buffer(command_buffer &&old) :
		handle_(old.handle_),
		begin_info_(old.begin_info_),
		pool_(move(old.pool_))
	{
		old.handle_ = nullptr;
	}

	command_buffer::~command_buffer()
	{
		if (handle_)
			pool_.parent_device().free_command_buffers(pool_.handle(), { handle_ });
	}

	void command_buffer::begin()
	{
		vk_try(handle_.begin(&begin_info_));
	}

	void command_buffer::end()
	{
		vk_try(handle_.end());
	}

	void command_buffer::submit(const device &device, const vector<reference_wrapper<semaphore>> &semaphores)
	{
		vector<vk::Semaphore> handles = set::from(semaphores)
			.select([](const reference_wrapper<semaphore> &x) { return x.get().handle(); })
			.to_vector();

		device.queue().submit(
		{
			vk::SubmitInfo()
				.waitSemaphoreCount(static_cast<uint32_t>(handles.size()))
				.pWaitSemaphores(handles.data())
				.commandBufferCount(1)
				.pCommandBuffers(&handle_)
		});
	}

	void command_buffer::begin_render_pass(const render_pass &pass, const framebuffer &framebuffer, uint32_t width, uint32_t height)
	{
		array<float, 4> clearColor = { .2f, .2f, .2f, 1 };
		vk::ClearValue clearValues[2];
		clearValues[0].color(vk::ClearColorValue(clearColor));
		clearValues[1].depthStencil(vk::ClearDepthStencilValue(1.0f, 0));

		vk::RenderPassBeginInfo beginInfo = vk::RenderPassBeginInfo()
			.renderPass(pass.handle())
			.renderArea(vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(width, height)))
			.clearValueCount(2)
			.pClearValues(clearValues)
			.framebuffer(framebuffer.handle());

		handle_.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
	}

	void command_buffer::end_render_pass()
	{
		vkCmdEndRenderPass(handle_);
	}

	void command_buffer::set_viewport(const float width, const float height)
	{
		vk::Viewport viewport = vk::Viewport()
			.height(height)
			.width(width)
			.minDepth(0.0f)
			.maxDepth(1.0f);

		handle_.setViewport(0, 1, &viewport);
	}

	void command_buffer::set_scissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
	{
		vk::Rect2D scissor = vk::Rect2D()
			.offset(vk::Offset2D(x, y))
			.extent(vk::Extent2D(width, height));

		handle_.setScissor(0, 1, &scissor);
	}

	void command_buffer::bind_descriptor_set(const pipeline_layout &pipelayout, const descriptor_set &descset)
	{
		handle_.bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			pipelayout.handle(),
			0,
			1, &descset.handle(),
			0, nullptr);
	}

	void command_buffer::bind_pipeline(const pipeline &pipeline)
	{
		handle_.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.handle());
	}

	void command_buffer::bind_vertex_buffer(const ibuffer &buffer)
	{
		vector<vk::DeviceSize> offsets = { 0 };
		handle_.bindVertexBuffers(0, 1, &buffer.handle(), offsets.data());
	}

	void command_buffer::bind_index_buffer(const buffer<uint32_t> &buffer)
	{
		handle_.bindIndexBuffer(buffer.handle(), 0, vk::IndexType::eUint32);
	}

	void command_buffer::draw_indexed(const uint32_t indexCount)
	{
		handle_.drawIndexed(indexCount, 1, 0, 0, 1);
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

		handle_.pipelineBarrier(
			vk::PipelineStageFlagBits::eAllCommands,
			vk::PipelineStageFlagBits::eTopOfPipe,
			vk::DependencyFlags(),
			0, nullptr,
			0, nullptr,
			1, &barrier);
	}

	void command_buffer::setImageLayout(const image &image, const vk::ImageAspectFlags &aspectMask, const vk::ImageLayout oldImageLayout, const vk::ImageLayout newImageLayout)
	{
		vk::ImageMemoryBarrier imgMemBarrier;
		imgMemBarrier.oldLayout(oldImageLayout);
		imgMemBarrier.newLayout(newImageLayout);
		imgMemBarrier.srcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		imgMemBarrier.dstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
		imgMemBarrier.subresourceRange(vk::ImageSubresourceRange(aspectMask, 0, 1, 0, 1));
		imgMemBarrier.image(image.handle());

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
		handle_.pipelineBarrier(
			srcStageFlags,
			destStageFlags,
			vk::DependencyFlagBits::eByRegion,
			0, nullptr,
			0, nullptr,
			1, &imgMemBarrier);
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
