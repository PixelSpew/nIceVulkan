#include "stdafx.h"
#include "instance.h"
#include <vector>

using namespace std;

namespace nif
{
	instance::instance(const string &name)
	{
		vk::ApplicationInfo appInfo;
		appInfo.pApplicationName(name.c_str());
		appInfo.pEngineName("nIce Framework");
		appInfo.apiVersion(VK_API_VERSION);

		vector<const char*> extensions = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
		vk::InstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo.pApplicationInfo(&appInfo);
		instanceCreateInfo.enabledExtensionCount(static_cast<uint32_t>(extensions.size()));
		instanceCreateInfo.ppEnabledExtensionNames(extensions.data());

		vk::createInstance(&instanceCreateInfo, nullptr, &handle_);
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
