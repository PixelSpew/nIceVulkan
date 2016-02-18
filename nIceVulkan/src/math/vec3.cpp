#include "stdafx.h"
#include "math/vec3.h"
#include <math.h>

namespace nif
{
	vec3::vec3() : x(0), y(0), z(0) { }
	vec3::vec3(const float val) : x(val), y(val), z(val) { }
	vec3::vec3(const float x, const float y, const float z) : x(x), y(y), z(z) { }
	vec3::vec3(const vec2 xy, const float z) : x(xy.x), y(xy.y), z(z) { }

	float vec3::length() const
	{
		return sqrtf(lengthsq());
	}

	float vec3::lengthsq() const
	{
		return x * x + y * y + z * z;
	}

	vec3 vec3::normalized() const
	{
		return *this / length();
	}

	vec3& vec3::operator+=(const vec3 &rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	vec3& vec3::operator-=(const vec3 &rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	vec3& vec3::operator*=(const float &rhs)
	{
		x *= rhs;
		y *= rhs;
		z *= rhs;
		return *this;
	}

	vec3& vec3::operator/=(const float &rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		return *this;
	}

	vec3 vec3::cross(const vec3 &lhs, const vec3 &rhs)
	{
		return vec3(
			lhs.y * rhs.z - lhs.z * rhs.y,
			lhs.z * rhs.x - lhs.x * rhs.z,
			lhs.x * rhs.y - lhs.y * rhs.x);
	}

	float vec3::dot(const vec3 &lhs, const vec3 &rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}
}
