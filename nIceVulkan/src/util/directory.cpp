#include "stdafx.h"
#include "util/directory.h"
#include "util/strext.h"
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

namespace nif
{
	void directory::create_directory(const std::string &name)
	{
#ifdef _WIN32
		CreateDirectory(strext::to_wstring(name).c_str(), nullptr);
#else
		mkdir(name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
	}
}
