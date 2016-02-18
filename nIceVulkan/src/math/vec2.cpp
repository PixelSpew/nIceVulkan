#include "stdafx.h"
#include "math/vec2.h"
#include <math.h>

namespace nif
{
	vec2::vec2() : x(0), y(0) { }
	vec2::vec2(const float val) : x(val), y(val) { }
	vec2::vec2(const float x, const float y) : x(x), y(y) { }

	float vec2::length() const
	{
		return sqrtf(x * x + y * y);
	}

	vec2 vec2::normalized() const
	{
		return *this / length();
	}

	vec2& vec2::operator+=(const vec2 &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	vec2& vec2::operator-=(const vec2 &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	vec2& vec2::operator*=(const float &rhs)
	{
		x *= rhs;
		y *= rhs;
		return *this;
	}

	vec2& vec2::operator/=(const float &rhs)
	{
		x /= rhs;
		y /= rhs;
		return *this;
	}

	float vec2::dot(const vec2 &lhs, const vec2 &rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y;
	}
}
