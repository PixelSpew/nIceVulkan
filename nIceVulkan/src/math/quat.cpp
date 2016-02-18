#include "stdafx.h"
#include "math/quat.h"
#include <math.h>

namespace nif
{
	quat::quat() : w(1) { }

	float quat::length() const
	{
		return sqrtf(lengthsq());
	}

	float quat::lengthsq() const
	{
		return w * w + xyz.lengthsq();
	}

	quat quat::normalized() const
	{
		float scale = 1.0f / length();
		vec3 xyznorm = xyz * scale;
		return quat(xyznorm.x, xyznorm.y, xyznorm.z, w * scale);
	}

	quat quat::inverted() const
	{
		float lensq = lengthsq();
		if (lensq != 0)
		{
			float i = 1 / lensq;
			return quat(xyz * -i, w * i);
		}
		else
		{
			return *this;
		}
	}

	vec4 quat::axis_angle() const
	{
		float resw = 2 * acosf(w);
		float den = sqrtf(1 - w * w);
		if (den > 0.0001f)
			return vec4(xyz / den, resw);
		else
			return vec4(1, 0, 0, resw);
	}

	quat quat::from_axis_angle(const vec3 &axis, float angle)
	{
		angle *= .5f;
		return quat(axis.normalized() * sinf(angle), cosf(angle));
	}

	quat::quat(const vec3 &xyz, float w) : xyz(xyz), w(w) { }
	quat::quat(const float x, const float y, const float z, const float w) : xyz(vec3(x, y, z)), w(w) { }

	void quat::normalize()
	{
		float scale = 1.0f / length();
		xyz *= scale;
		w *= scale;
	}
}