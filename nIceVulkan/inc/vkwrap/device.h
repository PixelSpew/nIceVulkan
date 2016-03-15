#pragma once
#include "vkwrap/instance.h"
#include "vkwrap/queue.h"

namespace nif
{
	class device
	{
	public:
		explicit device(const instance &instance);
		device(const device&) = delete;
		~device();

		std::vector<vk::CommandBuffer> allocate_command_buffers(const vk::CommandBufferAllocateInfo &allocInfo) const;
		void free_command_buffers(const vk::CommandPool cmdpool, const std::vector<vk::CommandBuffer> &cmdbuffer) const;

		std::vector<vk::DescriptorSet> allocate_descriptor_sets(const vk::DescriptorSetAllocateInfo &allocInfo) const;
		void free_descriptor_sets(const vk::DescriptorPool descpool, const std::vector<vk::DescriptorSet> &descsets) const;
		void update_descriptor_sets(const std::vector<vk::WriteDescriptorSet> &write) const;

		vk::DeviceMemory allocate_memory(const vk::MemoryAllocateInfo &allocInfo) const;
		void free_memory(const vk::DeviceMemory memory) const;
		void* map_memory(const vk::DeviceMemory memory, const vk::DeviceSize size) const;
		void unmap_memory(const vk::DeviceMemory memory) const;

		vk::Buffer create_buffer(const vk::BufferCreateInfo &createInfo) const;
		void destroy_buffer(const vk::Buffer buffer) const;
		vk::MemoryRequirements get_buffer_memory_requirements(const vk::Buffer buffer) const;
		void bind_buffer_memory(const vk::Buffer buffer, const vk::DeviceMemory memory, const vk::DeviceSize offset) const;

		vk::CommandPool create_command_pool(const vk::CommandPoolCreateInfo &createInfo) const;
		void destroy_command_pool(const vk::CommandPool cmdpool) const;

		vk::DescriptorPool create_descriptor_pool(const vk::DescriptorPoolCreateInfo &createInfo) const;
		void destroy_descriptor_pool(const vk::DescriptorPool descpool) const;

		vk::DescriptorSetLayout create_descriptor_set_layout(const vk::DescriptorSetLayoutCreateInfo &createInfo) const;
		void destroy_descriptor_set_layout(const vk::DescriptorSetLayout descSetLayout) const;

		vk::Framebuffer create_framebuffer(const vk::FramebufferCreateInfo &createInfo) const;
		void destroy_framebuffer(const vk::Framebuffer framebuffer) const;

		std::vector<vk::Pipeline> create_graphics_pipelines(const vk::PipelineCache cache, const std::vector<vk::GraphicsPipelineCreateInfo> &createInfos) const;
		void destroy_pipeline(const vk::Pipeline pipeline) const;

		vk::Image create_image(const vk::ImageCreateInfo &createInfo) const;
		void destroy_image(const vk::Image image) const;
		vk::MemoryRequirements get_image_memory_requirements(const vk::Image image) const;
		void bind_image_memory(const vk::Image image, const vk::DeviceMemory memory, const vk::DeviceSize offset) const;

		vk::ImageView create_image_view(const vk::ImageViewCreateInfo &createInfo) const;
		void destroy_image_view(const vk::ImageView view) const;
		
		vk::PipelineCache create_pipeline_cache(const vk::PipelineCacheCreateInfo &createInfo) const;
		void destroy_pipeline_cache(const vk::PipelineCache cache) const;

		vk::PipelineLayout create_pipeline_layout(const vk::PipelineLayoutCreateInfo &createInfo) const;
		void destroy_pipeline_layout(const vk::PipelineLayout layout) const;

		vk::RenderPass create_render_pass(const vk::RenderPassCreateInfo &createInfo) const;
		void destroy_render_pass(const vk::RenderPass pass) const;

		vk::Semaphore create_semaphore(const vk::SemaphoreCreateInfo &createInfo) const;
		void destroy_semaphore(const vk::Semaphore semaphore) const;

		vk::ShaderModule create_shader_module(const vk::ShaderModuleCreateInfo &createInfo) const;
		void destroy_shader_module(const vk::ShaderModule shaderModule) const;

		vk::SwapchainKHR create_swap_chain(const vk::SwapchainCreateInfoKHR &createInfo) const;
		void destroy_swap_chain(const vk::SwapchainKHR swap) const;
		std::vector<vk::Image> get_swap_chain_images(const vk::SwapchainKHR swap) const;
		uint32_t acquire_next_image(const vk::SwapchainKHR swap, const vk::Semaphore semaphore) const;

		vk::Queue get_queue(const uint32_t familyIndex) const;

		vk::Device handle() const;
		const instance& parent_instance() const;
		const physical_device& physdevice() const;
		vk::Format depth_format() const;
		queue queue() const;

	private:
		vk::Device handle_;
		const instance &instance_;
		const nif::physical_device &physical_device_;
		vk::Format depth_format_;
		nif::queue queue_;
	};
}
