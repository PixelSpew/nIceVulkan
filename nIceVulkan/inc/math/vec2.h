#pragma once

namespace nif
{
	struct vec2
	{
		float x;
		float y;

		vec2();
		vec2(const float val);
		vec2(const float x, const float y);

		float length() const;
		vec2 normalized() const;

		vec2& operator+=(const vec2 &rhs);
		vec2& operator-=(const vec2 &rhs);
		vec2& operator*=(const float &rhs);
		vec2& operator/=(const float &rhs);

		static float dot(const vec2 &lhs, const vec2 &rhs);
	};

	inline vec2 operator-(const vec2 &rhs) { return vec2(0) -= rhs; }
	inline vec2 operator+(vec2 lhs, const vec2 &rhs) { return lhs += rhs; }
	inline vec2 operator-(vec2 lhs, const vec2 &rhs) { return lhs -= rhs; }
	inline vec2 operator*(vec2 lhs, const float rhs) { return lhs *= rhs; }
	inline vec2 operator/(vec2 lhs, const float rhs) { return lhs /= rhs; }
}
