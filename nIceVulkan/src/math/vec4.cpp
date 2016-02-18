#include "stdafx.h"
#include "math/vec4.h"

namespace nif
{
	vec4::vec4() : x(0), y(0), z(0), w(0) { }
	vec4::vec4(const float val) : x(val), y(val), z(val), w(val) { }
	vec4::vec4(const float x, const float y, const float z, const float w) : x(x), y(y), z(z), w(w) { }
	vec4::vec4(const vec2 xy, const float z, const float w) : x(xy.x), y(xy.y), z(z), w(w) { }
	vec4::vec4(const vec2 xy, const vec2 zw) : x(xy.x), y(xy.y), z(zw.x), w(zw.y) { }
	vec4::vec4(const vec3 xyz, const float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) { }

	void vec4::xyz(const vec3 &xyz)
	{
		x = xyz.x;
		y = xyz.y;
		z = xyz.z;
	}

	float& vec4::operator[](int idx)
	{
		return *(reinterpret_cast<float*>(this) + idx);
	}

	float vec4::operator[](int idx) const
	{
		return *(reinterpret_cast<const float*>(this) + idx);
	}

	float vec4::dot(const vec4 &lhs, const vec4 &rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
	}
}
