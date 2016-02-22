#include "stdafx.h"
#include "util/strext.h"

using namespace std;

namespace nif
{
	std::wstring strext::to_wstring(const std::string &text)
	{
		return std::wstring(text.begin(), text.end());
	}
}
