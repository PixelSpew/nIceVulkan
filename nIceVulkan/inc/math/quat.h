#pragma once
#include "vec4.h"

namespace nif
{
	class quat
	{
	public:
		quat();
		float length() const;
		float lengthsq() const;
		quat normalized() const;
		quat inverted() const;
		vec4 axis_angle() const;

	public:
		static quat from_axis_angle(const vec3 &axis, float angle);

	private:
		quat(const vec3 &xyz, const float w);
		quat(const float x, const float y, const float z, const float w);
		void normalize();

	private:
		vec3 xyz;
		float w;
	};
}