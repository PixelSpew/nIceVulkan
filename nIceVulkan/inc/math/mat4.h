#pragma once
#include "math/quat.h"

namespace nif
{
	class mat4
	{
	public:
		mat4();
		mat4(const float v00, const float v01, const float v02, const float v03,
			const float v10, const float v11, const float v12, const float v13,
			const float v20, const float v21, const float v22, const float v23,
			const float v30, const float v31, const float v32, const float v33);
		mat4(const vec4 &v0, const vec4 &v1, const vec4 &v2, const vec4 &v3);
		vec4 row(int idx);

	public:
		static mat4 translation(const vec3 &val);
		static mat4 rotation(const quat &val);
		static mat4 rotation(const vec4 &axisAngle);
		static mat4 scale(const vec3 &val);
		/**
		* Assumes all parameters are normalized
		*/
		static mat4 lookat(const vec3 &eye, const vec3 &target, const vec3 &up);
		static mat4 perspective_fov(const float fov, const float width, const float height, const float near, const float far);

	public:
		mat4& operator*=(const mat4 &rhs);
		vec4& operator[](int idx);
		const vec4& operator[](int idx) const;

	private:
		vec4 cols[4];
	};

	inline mat4 operator*(mat4 lhs, const mat4 &rhs) { return lhs *= rhs; }
}
