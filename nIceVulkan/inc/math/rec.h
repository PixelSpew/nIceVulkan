#pragma once
#include <vector>
#include "math/vec2.h"

namespace nif
{
	class rec
	{
	public:
		rec();
		rec(const float x, const float y, const float w, const float h);
		float width() const;
		float height() const;
		float left() const;
		float right() const;
		float top() const;
		float bottom() const;
		vec2 top_left() const;
		vec2 top_right() const;
		vec2 bottom_left() const;
		vec2 bottom_right() const;
		vec2 size() const;

		std::vector<rec> sub(const rec &rhs) const;

	private:
		float x_;
		float y_;
		float w_;
		float h_;
	};
}
