#include "stdafx.h"
#include "math/ivec2.h"

namespace nif
{
	ivec2::ivec2() : x(0), y(0) { }
	ivec2::ivec2(const int val) : x(val), y(val) { }
	ivec2::ivec2(const int x, const int y) : x(x), y(y) { }
}
