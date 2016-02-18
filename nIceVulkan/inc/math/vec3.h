#pragma once
#include "vec2.h"

namespace nif
{
	struct vec3
	{
		float x;
		float y;
		float z;

		vec3();
		vec3(const float val);
		vec3(const float x, const float y, const float z);
		vec3(const vec2 xy, const float z);
		
		float length() const;
		float lengthsq() const;
		vec3 normalized() const;

		vec3& operator+=(const vec3 &rhs);
		vec3& operator-=(const vec3 &rhs);
		vec3& operator*=(const float &rhs);
		vec3& operator/=(const float &rhs);

		static vec3 cross(const vec3 &lhs, const vec3 &rhs);
		static float dot(const vec3 &lhs, const vec3 &rhs);
	};

	inline vec3 operator-(const vec3 &rhs) { return vec3(0) -= rhs; }
	inline vec3 operator+(vec3 lhs, const vec3 &rhs) { return lhs += rhs; }
	inline vec3 operator-(vec3 lhs, const vec3 &rhs) { return lhs -= rhs; }
	inline vec3 operator*(vec3 lhs, const float rhs) { return lhs *= rhs; }
	inline vec3 operator/(vec3 lhs, const float rhs) { return lhs /= rhs; }
}
