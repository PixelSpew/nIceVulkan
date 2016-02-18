#include "stdafx.h"
#include "math/mat4.h"
#include <math.h>

namespace nif
{
	mat4::mat4() : cols{ vec4(0), vec4(0), vec4(0), vec4(0) } { }
	mat4::mat4(const float v00, const float v01, const float v02, const float v03,
		const float v10, const float v11, const float v12, const float v13,
		const float v20, const float v21, const float v22, const float v23,
		const float v30, const float v31, const float v32, const float v33)
		: cols{ vec4(v00, v01, v02, v03), vec4(v10, v11, v12, v13), vec4(v20, v21, v22, v23), vec4(v30, v31, v32, v33) } { }
	mat4::mat4(const vec4 &v0, const vec4 &v1, const vec4 &v2, const vec4 &v3) : cols{ v0, v1, v2, v3 } { }

	vec4 mat4::row(int idx)
	{
		return vec4(cols[0][idx], cols[1][idx], cols[2][idx], cols[3][idx]);
	}

	mat4 mat4::translation(const vec3 &val)
	{
		return mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			val.x, val.y, val.z, 1);
	}

	mat4 mat4::rotation(const quat &val)
	{
		return rotation(val.axis_angle());
	}

	mat4 mat4::rotation(const vec4 &axisAngle)
	{
		float cos = cosf(-axisAngle.w);
		float sin = sinf(-axisAngle.w);
		float t = 1 - cos;

		return mat4(
			t * axisAngle.x * axisAngle.x + cos, t * axisAngle.x * axisAngle.y - sin * axisAngle.z, t * axisAngle.x * axisAngle.z + sin * axisAngle.y, 0,
			t * axisAngle.x * axisAngle.y + sin * axisAngle.z, t * axisAngle.y * axisAngle.y + cos, t * axisAngle.y * axisAngle.z - sin * axisAngle.x, 0,
			t * axisAngle.x * axisAngle.z - sin * axisAngle.y, t * axisAngle.y * axisAngle.z + sin * axisAngle.x, t * axisAngle.z * axisAngle.z + cos, 0,
			0, 0, 0, 1);
	}

	mat4 mat4::scale(const vec3 &val)
	{
		return mat4(
			val.x, 0, 0, 0,
			0, val.y, 0, 0,
			0, 0, val.z, 0,
			0, 0, 0, 1);
	}

	mat4 mat4::lookat(const vec3 &eye, const vec3 &target, const vec3 &up)
	{
		vec3 zdir(eye - target);
		vec3 xdir = vec3::cross(up, zdir);
		vec3 ydir = vec3::cross(zdir, xdir);
		vec3 negeye = -eye;

		return mat4(
			xdir.x, ydir.x, zdir.x, 0.0f,
			xdir.y, ydir.y, zdir.y, 0.0f,
			xdir.z, ydir.z, zdir.z, 0.0f,
			vec3::dot(xdir, negeye), vec3::dot(ydir, negeye), vec3::dot(zdir, negeye), 1.0f);
	}

	mat4 mat4::perspective_fov(const float fovy, const float width, const float height, const float near, const float far)
	{
		float yscale = 1 / tanf(fovy / 2);
		float xscale = yscale * height / width;
		float zscale = far / (near - far);

		return mat4(
			xscale, 0, 0, 0,
			0, yscale, 0, 0,
			0, 0, zscale, -1,
			0, 0, 2 * near * zscale, 0);
	}

	mat4& mat4::operator*=(const mat4 &rhs)
	{
		cols[0][0] = vec4::dot(row(0), rhs[0]);
		cols[1][0] = vec4::dot(row(0), rhs[1]);
		cols[2][0] = vec4::dot(row(0), rhs[2]);
		cols[3][0] = vec4::dot(row(0), rhs[3]);
		cols[0][1] = vec4::dot(row(1), rhs[0]);
		cols[1][1] = vec4::dot(row(1), rhs[1]);
		cols[2][1] = vec4::dot(row(1), rhs[2]);
		cols[3][1] = vec4::dot(row(1), rhs[3]);
		cols[0][2] = vec4::dot(row(2), rhs[0]);
		cols[1][2] = vec4::dot(row(2), rhs[1]);
		cols[2][2] = vec4::dot(row(2), rhs[2]);
		cols[3][2] = vec4::dot(row(2), rhs[3]);
		cols[0][3] = vec4::dot(row(3), rhs[0]);
		cols[1][3] = vec4::dot(row(3), rhs[1]);
		cols[2][3] = vec4::dot(row(3), rhs[2]);
		cols[3][3] = vec4::dot(row(3), rhs[3]);

		return *this;
	}

	vec4& mat4::operator[](int idx)
	{
		return *(reinterpret_cast<vec4*>(this) + idx);
	}

	const vec4& mat4::operator[](int idx) const
	{
		return *(reinterpret_cast<const vec4*>(this) + idx);
	}
}