#pragma once
#include "vec3.h"

namespace nif
{
	struct vec4
	{
		float x;
		float y;
		float z;
		float w;

		vec4();
		vec4(const float val);
		vec4(const float x, const float y, const float z, const float w);
		vec4(const vec2 xy, const float z, const float w);
		vec4(const vec2 xy, const vec2 zw);
		vec4(const vec3 xyz, const float w);

		void xyz(const vec3 &xyz);

		float& operator[](int idx);
		float operator[](int idx) const;

		static float dot(const vec4 &lhs, const vec4 &rhs);
	};
}
