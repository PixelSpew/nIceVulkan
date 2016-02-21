#include "stdafx.h"
#include "instance.h"
#include <vector>

using namespace std;

namespace nif
{
	instance::instance(const string &name)
	{
		vk::ApplicationInfo appInfo;
		appInfo.pApplicationName(name.c_str())
			.pEngineName("nIce Framework")
			.apiVersion(VK_API_VERSION);

		vector<const char*> extensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
		vk::InstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo
			.pApplicationInfo(&appInfo)
			.enabledExtensionCount(static_cast<uint32_t>(extensions.size()))
			.ppEnabledExtensionNames(extensions.data());

		if (vk::createInstance(&instanceCreateInfo, nullptr, &handle_) != vk::Result::eVkSuccess)
			throw runtime_error("fail");
	}

	instance::~instance()
	{
		vk::destroyInstance(handle_, nullptr);
	}

	vk::Instance instance::handle() const
	{
		return handle_;
	}
}
