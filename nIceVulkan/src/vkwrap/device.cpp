#include "stdafx.h"
#include "vkwrap/device.h"
#include "util/setops.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	device::device(const instance &instance) :
		instance_(instance),
		physical_device_(instance.enumerate_physical_devices()[0])
	{
		uint32_t graphicsQueueIndex = static_cast<uint32_t>(
			set::from(physical_device_.get_queue_family_properties())
				.first_index([](const vk::QueueFamilyProperties &x) {
					return x.queueFlags() & VK_QUEUE_GRAPHICS_BIT;
				}));

		vector<float> queuePriorities = { 0.0f };
		auto queueCreateInfo = vk::DeviceQueueCreateInfo()
			.queueFamilyIndex(graphicsQueueIndex)
			.queueCount(static_cast<uint32_t>(queuePriorities.size()))
			.pQueuePriorities(queuePriorities.data());

		vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		handle_ = physical_device_.create_device(
			vk::DeviceCreateInfo()
				.queueCreateInfoCount(1)
				.pQueueCreateInfos(&queueCreateInfo)
				.enabledExtensionCount(static_cast<uint32_t>(extensions.size()))
				.ppEnabledExtensionNames(extensions.data())
				.enabledLayerCount(static_cast<uint32_t>(instance.layers().size()))
				.ppEnabledLayerNames(instance.layers().data()));

		depth_format_ = set::from({ vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm })
			.first([&](auto &format) {
				return physical_device_.get_format_properties(format).optimalTilingFeatures() & vk::FormatFeatureFlagBits::eDepthStencilAttachment;
			});

		queue_ = get_queue(graphicsQueueIndex);
	}

	device::~device()
	{
		handle_.destroy(nullptr);
	}

	vector<vk::CommandBuffer> device::allocate_command_buffers(const vk::CommandBufferAllocateInfo &allocInfo) const
	{
		vector<vk::CommandBuffer> ret(allocInfo.commandBufferCount());
		vk_try(handle_.allocateCommandBuffers(&allocInfo, ret.data()));
		return ret;
	}

	void device::free_command_buffers(const vk::CommandPool cmdpool, const vector<vk::CommandBuffer> &cmdbuffer) const
	{
		handle_.freeCommandBuffers(cmdpool, static_cast<uint32_t>(cmdbuffer.size()), cmdbuffer.data());
	}

	vector<vk::DescriptorSet> device::allocate_descriptor_sets(const vk::DescriptorSetAllocateInfo &allocInfo) const
	{
		vector<vk::DescriptorSet> ret(allocInfo.descriptorSetCount());
		vk_try(handle_.allocateDescriptorSets(&allocInfo, ret.data()));
		return ret;
	}

	void device::free_descriptor_sets(const vk::DescriptorPool descpool, const vector<vk::DescriptorSet> &descsets) const
	{
		vk_try(handle_.freeDescriptorSets(descpool, static_cast<uint32_t>(descsets.size()), descsets.data()));
	}

	void device::update_descriptor_sets(const vector<vk::WriteDescriptorSet> &write) const
	{
		handle_.updateDescriptorSets(static_cast<uint32_t>(write.size()), write.data(), 0, nullptr);
	}

	vk::DeviceMemory device::allocate_memory(const vk::MemoryAllocateInfo &allocInfo) const
	{
		vk::DeviceMemory ret;
		vk_try(handle_.allocateMemory(&allocInfo, nullptr, &ret));
		return ret;
	}

	void device::free_memory(const vk::DeviceMemory memory) const
	{
		handle_.freeMemory(memory, nullptr);
	}

	void* device::map_memory(const vk::DeviceMemory memory, const vk::DeviceSize size) const
	{
		void *ret;
		vk_try(handle_.mapMemory(memory, 0, size, vk::MemoryMapFlags(), &ret));
		return ret;
	}

	void device::unmap_memory(const vk::DeviceMemory memory) const
	{
		handle_.unmapMemory(memory);
	}

	vk::Buffer device::create_buffer(const vk::BufferCreateInfo &createInfo) const
	{
		vk::Buffer ret;
		vk_try(handle_.createBuffer(&createInfo, nullptr, &ret));
		return ret;
	}

	void device::destroy_buffer(const vk::Buffer buffer) const
	{
		handle_.destroyBuffer(buffer, nullptr);
	}

	vk::MemoryRequirements device::get_buffer_memory_requirements(const vk::Buffer buffer) const
	{
		vk::MemoryRequirements ret;
		handle_.getBufferMemoryRequirements(buffer, &ret);
		return ret;
	}

	void device::bind_buffer_memory(const vk::Buffer buffer, const vk::DeviceMemory memory, const vk::DeviceSize offset) const
	{
		vk_try(handle_.bindBufferMemory(buffer, memory, offset));
	}

	vk::CommandPool device::create_command_pool(const vk::CommandPoolCreateInfo &createInfo) const
	{
		vk::CommandPool ret;
		vk_try(handle_.createCommandPool(&createInfo, nullptr, &ret));
		return ret;
	}

	void device::destroy_command_pool(const vk::CommandPool cmdpool) const
	{
		handle_.destroyCommandPool(cmdpool, nullptr);
	}

	vk::DescriptorPool device::create_descriptor_pool(const vk::DescriptorPoolCreateInfo &createInfo) const
	{
		vk::DescriptorPool ret;
		vk_try(handle_.createDescriptorPool(&createInfo, nullptr, &ret));
		return ret;
	}

	void device::destroy_descriptor_pool(const vk::DescriptorPool descpool) const
	{
		handle_.destroyDescriptorPool(descpool, nullptr);
	}

	vk::DescriptorSetLayout device::create_descriptor_set_layout(const vk::DescriptorSetLayoutCreateInfo &createInfo) const
	{
		vk::DescriptorSetLayout ret;
		vk_try(handle_.createDescriptorSetLayout(&createInfo, nullptr, &ret));
		return ret;
	}

	void device::destroy_descriptor_set_layout(const vk::DescriptorSetLayout descSetLayout) const
	{
		handle_.destroyDescriptorSetLayout(descSetLayout, nullptr);
	}

	vk::Framebuffer device::create_framebuffer(const vk::FramebufferCreateInfo &createInfo) const
	{
		vk::Framebuffer ret;
		vk_try(handle_.createFramebuffer(&createInfo, nullptr, &ret));
		return ret;
	}

	void device::destroy_framebuffer(const vk::Framebuffer framebuffer) const
	{
		handle_.destroyFramebuffer(framebuffer, nullptr);
	}

	std::vector<vk::Pipeline> device::create_graphics_pipelines(const vk::PipelineCache cache, const std::vector<vk::GraphicsPipelineCreateInfo>& createInfos) const
	{
		vector<vk::Pipeline> ret(createInfos.size());
		vk_try(handle_.createGraphicsPipelines(
			cache,
			static_cast<uint32_t>(createInfos.size()),
			createInfos.data(),
			nullptr,
			ret.data()));
		return ret;
	}

	void device::destroy_pipeline(const vk::Pipeline pipeline) const
	{
		handle_.destroyPipeline(pipeline, nullptr);
	}

	vk::Image device::create_image(const vk::ImageCreateInfo &createInfo) const
	{
		vk::Image ret;
		vk_try(handle_.createImage(&createInfo, nullptr, &ret));
		return ret;
	}

	void device::destroy_image(const vk::Image image) const
	{
		handle_.destroyImage(image, nullptr);
	}

	vk::MemoryRequirements device::get_image_memory_requirements(const vk::Image image) const
	{
		vk::MemoryRequirements ret;
		handle_.getImageMemoryRequirements(image, &ret);
		return ret;
	}

	void device::bind_image_memory(const vk::Image image, const vk::DeviceMemory memory, const vk::DeviceSize offset) const
	{
		vk_try(handle_.bindImageMemory(image, memory, offset));
	}

	vk::ImageView device::create_image_view(const vk::ImageViewCreateInfo &createInfo) const
	{
		vk::ImageView ret;
		vk_try(handle_.createImageView(&createInfo, nullptr, &ret));
		return ret;
	}

	void device::destroy_image_view(const vk::ImageView view) const
	{
		handle_.destroyImageView(view, nullptr);
	}

	vk::PipelineCache device::create_pipeline_cache(const vk::PipelineCacheCreateInfo &createInfo) const
	{
		vk::PipelineCache ret;
		vk_try(handle_.createPipelineCache(&createInfo, nullptr, &ret));
		return ret;
	}

	void device::destroy_pipeline_cache(const vk::PipelineCache cache) const
	{
		handle_.destroyPipelineCache(cache, nullptr);
	}

	vk::PipelineLayout device::create_pipeline_layout(const vk::PipelineLayoutCreateInfo &createInfo) const
	{
		vk::PipelineLayout ret;
		vk_try(handle_.createPipelineLayout(&createInfo, nullptr, &ret));
		return ret;
	}

	void device::destroy_pipeline_layout(const vk::PipelineLayout layout) const
	{
		handle_.destroyPipelineLayout(layout, nullptr);
	}

	vk::RenderPass device::create_render_pass(const vk::RenderPassCreateInfo &createInfo) const
	{
		vk::RenderPass ret;
		vk_try(handle_.createRenderPass(&createInfo, nullptr, &ret));
		return ret;
	}

	void device::destroy_render_pass(const vk::RenderPass pass) const
	{
		handle_.destroyRenderPass(pass, nullptr);
	}

	vk::Semaphore device::create_semaphore(const vk::SemaphoreCreateInfo &createInfo) const
	{
		vk::Semaphore ret;
		vk_try(handle_.createSemaphore(&createInfo, nullptr, &ret));
		return ret;
	}

	void device::destroy_semaphore(const vk::Semaphore semaphore) const
	{
		handle_.destroySemaphore(semaphore, nullptr);
	}

	vk::ShaderModule device::create_shader_module(const vk::ShaderModuleCreateInfo &createInfo) const
	{
		vk::ShaderModule ret;
		vk_try(handle_.createShaderModule(&createInfo, nullptr, &ret));
		return ret;
	}

	void device::destroy_shader_module(const vk::ShaderModule shaderModule) const
	{
		handle_.destroyShaderModule(shaderModule, nullptr);
	}

	vk::SwapchainKHR device::create_swap_chain(const vk::SwapchainCreateInfoKHR &createInfo) const
	{
		vk::SwapchainKHR ret;
		vk_try(handle_.createSwapchainKHR(&createInfo, nullptr, &ret));
		return ret;
	}

	void device::destroy_swap_chain(const vk::SwapchainKHR swap) const
	{
		handle_.destroySwapchainKHR(swap, nullptr);
	}

	std::vector<vk::Image> device::get_swap_chain_images(const vk::SwapchainKHR swap) const
	{
		uint32_t imageCount;
		vk_try(handle_.getSwapchainImagesKHR(swap, &imageCount, nullptr));
		vector<vk::Image> ret(imageCount);
		vk_try(handle_.getSwapchainImagesKHR(swap, &imageCount, ret.data()));
		return ret;
	}

	uint32_t device::acquire_next_image(const vk::SwapchainKHR swap, const vk::Semaphore semaphore) const
	{
		uint32_t ret;
		handle_.acquireNextImageKHR(swap, UINT64_MAX, semaphore, nullptr, &ret);
		return ret;
	}

	vk::Queue device::get_queue(const uint32_t familyIndex) const
	{
		vk::Queue ret;
		handle_.getQueue(familyIndex, 0, &ret);
		return ret;
	}

	vk::Device device::handle() const
	{
		return handle_;
	}

	const instance& device::parent_instance() const
	{
		return instance_;
	}

	const physical_device& device::physdevice() const
	{
		return physical_device_;
	}

	vk::Format device::depth_format() const
	{
		return depth_format_;
	}

	queue device::queue() const
	{
		return queue_;
	}
}
