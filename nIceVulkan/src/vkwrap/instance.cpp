#include "stdafx.h"
#include "vkwrap/instance.h"
#include "util/file.h"
#include "util/directory.h"
#include "util/setops.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	VkBool32 messageCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t srcObject,
		size_t location,
		int32_t msgCode,
		const char* pLayerPrefix,
		const char* pMsg,
		void* pUserData)
	{
		if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
			file::append_all_text("log/log.txt", string("DEBUG: [") + pLayerPrefix + "] Code " + to_string(msgCode) + " : " + pMsg + "\n");
		else if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
			file::append_all_text("log/log.txt", string("ERROR: [") + pLayerPrefix + "] Code " + to_string(msgCode) + " : " + pMsg + "\n");
		else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
			file::append_all_text("log/log.txt", string("PERFORMANCE WARNING: [") + pLayerPrefix + "] Code " + to_string(msgCode) + " : " + pMsg + "\n");
		else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
			file::append_all_text("log/log.txt", string("WARNING: [") + pLayerPrefix + "] Code " + to_string(msgCode) + " : " + pMsg + "\n");
		/*else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
			file::append_all_text("log/log.txt", string("INFORMATION: [") + pLayerPrefix + "] Code " + to_string(msgCode) + " : " + pMsg + "\n");*/

		return false;
	}

	instance::instance(const string &name)
	{
		vk::ApplicationInfo appInfo;
		appInfo.pApplicationName(name.c_str())
			.pEngineName("nIce Framework")
			.apiVersion(VK_API_VERSION);

		vector<const char*> extensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
#ifdef _DEBUG
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif

		vk::InstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo
			.pApplicationInfo(&appInfo)
			.enabledExtensionCount(static_cast<uint32_t>(extensions.size()))
			.ppEnabledExtensionNames(extensions.data())
			.enabledLayerCount(static_cast<uint32_t>(layers().size()))
			.ppEnabledLayerNames(layers().data());
		vk_try(vk::createInstance(&instanceCreateInfo, nullptr, &handle_));

		uint32_t gpuCount;
		vk_try(vk::enumeratePhysicalDevices(handle_, &gpuCount, nullptr));
		std::vector<vk::PhysicalDevice> physicalHandles(gpuCount);
		vk_try(vk::enumeratePhysicalDevices(handle_, &gpuCount, physicalHandles.data()));
		physical_devices_ = set::from(physicalHandles)
			.select([](const vk::PhysicalDevice x) { return physical_device(x); })
			.to_vector();

#ifdef _DEBUG
		directory::create_directory("log");
		file::write_all_text("log/log.txt", "");

		PFN_vkCreateDebugReportCallbackEXT createDebugReport = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(handle_, "vkCreateDebugReportCallbackEXT");
		destroy_debug_report_ = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(handle_, "vkDestroyDebugReportCallbackEXT");

		VkDebugReportCallbackCreateInfoEXT dbgCreateInfo =
			vk::DebugReportCallbackCreateInfoEXT(
				vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning,
				reinterpret_cast<PFN_vkDebugReportCallbackEXT>(messageCallback),
				nullptr);

		createDebugReport(handle_, &dbgCreateInfo, nullptr, &debug_report_);
#endif
	}

	instance::instance(instance &&old)
		: handle_(old.handle_)
	{
		old.handle_ = nullptr;
	}

	instance::~instance()
	{
		if (handle_)
		{
#ifdef _DEBUG
			destroy_debug_report_(handle_, debug_report_, nullptr);
#endif

			vk::destroyInstance(handle_, nullptr);
		}
	}

	vk::Instance instance::handle() const
	{
		return handle_;
	}

	const vector<physical_device>& instance::physical_devices() const
	{
		return physical_devices_;
	}

	const vector<const char*>& instance::layers()
	{
#ifdef _DEBUG
		static const vector<const char*> layers = {
			"VK_LAYER_LUNARG_threading",
			"VK_LAYER_LUNARG_mem_tracker",
			"VK_LAYER_LUNARG_object_tracker",
			"VK_LAYER_LUNARG_draw_state",
			"VK_LAYER_LUNARG_param_checker",
			"VK_LAYER_LUNARG_swapchain",
			"VK_LAYER_LUNARG_device_limits",
			"VK_LAYER_LUNARG_image",
			"VK_LAYER_GOOGLE_unique_objects"
		};
#else
		static const vector<const char*> layers = {};
#endif

		return layers;
	}
}
