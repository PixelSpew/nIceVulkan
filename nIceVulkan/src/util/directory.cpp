#include "stdafx.h"
#include "util/directory.h"
#include "util/strext.h"
#include <Windows.h>

namespace nif
{
	void directory::create_directory(const std::string &name)
	{
		CreateDirectory(strext::to_wstring(name).c_str(), nullptr);
	}
}
