#include "stdafx.h"
#include "vkwrap/device.h"
#include "util/setops.h"

using namespace std;

namespace nif
{
	device::device(const instance &instance)
		: instance_(instance)
	{
		uint32_t gpuCount;
		if (vk::enumeratePhysicalDevices(instance.handle(), &gpuCount, &physical_handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		uint32_t queueCount;
		vk::getPhysicalDeviceQueueFamilyProperties(physical_handle_, &queueCount, nullptr);
		vector<vk::QueueFamilyProperties> queueProps(queueCount);
		vk::getPhysicalDeviceQueueFamilyProperties(physical_handle_, &queueCount, queueProps.data());

		uint32_t graphicsQueueIndex = static_cast<uint32_t>(
			set::from(queueProps)
				.first_index([](const vk::QueueFamilyProperties &x) {
					return x.queueFlags() & VK_QUEUE_GRAPHICS_BIT;
				}));

		vector<float> queuePriorities = { 0.0f };
		vk::DeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo
			.queueFamilyIndex(graphicsQueueIndex)
			.queueCount(static_cast<uint32_t>(queuePriorities.size()))
			.pQueuePriorities(queuePriorities.data());

		vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		vk::DeviceCreateInfo deviceCreateInfo;
		deviceCreateInfo
			.queueCreateInfoCount(1)
			.pQueueCreateInfos(&queueCreateInfo)
			.enabledExtensionCount(static_cast<uint32_t>(extensions.size()))
			.ppEnabledExtensionNames(extensions.data())
			.enabledLayerCount(static_cast<uint32_t>(instance.layers().size()))
			.ppEnabledLayerNames(instance.layers().data());

		if (vk::createDevice(physical_handle_, &deviceCreateInfo, nullptr, &handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");

		//get memory properties
		vk::getPhysicalDeviceMemoryProperties(physical_handle_, &memory_properties_);

		//get depth format
		depth_format_ = set::from({ vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm })
			.first([&](auto &format) {
				vk::FormatProperties formatProps;
				vk::getPhysicalDeviceFormatProperties(physical_handle_, format, &formatProps);
				return formatProps.optimalTilingFeatures() & vk::FormatFeatureFlagBits::eDepthStencilAttachment;
			});

		//get graphics queue
		vk::getDeviceQueue(handle_, graphicsQueueIndex, 0, &queue_);
	}

	device::~device()
	{
		vk::destroyDevice(handle_, nullptr);
	}

	void device::wait_queue_idle() const
	{
		if (vk::queueWaitIdle(queue_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");
	}

	vk::Device device::handle() const
	{
		return handle_;
	}

	const instance& device::parent_instance() const
	{
		return instance_;
	}

	vk::PhysicalDevice device::physical_handle() const
	{
		return physical_handle_;
	}

	const vk::PhysicalDeviceMemoryProperties& device::memory_properties() const
	{
		return memory_properties_;
	}

	vk::Format device::depth_format() const
	{
		return depth_format_;
	}

	vk::Queue device::queue() const
	{
		return queue_;
	}
}
