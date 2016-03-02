#include "stdafx.h"
#include "vkwrap/device.h"
#include "util/setops.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	device::device(const instance &instance) :
		instance_(instance),
		physical_device_(instance.physical_devices()[0])
	{
		uint32_t graphicsQueueIndex = static_cast<uint32_t>(
			set::from(physical_device_.queue_props())
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

		vk_try(vk::createDevice(physical_device_.handle(), &deviceCreateInfo, nullptr, &handle_));

		//get depth format
		depth_format_ = set::from({ vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm })
			.first([&](auto &format) {
				return physical_device_.query_format_properties(format).optimalTilingFeatures() & vk::FormatFeatureFlagBits::eDepthStencilAttachment;
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

	const physical_device & device::physical_device() const
	{
		return physical_device_;
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
