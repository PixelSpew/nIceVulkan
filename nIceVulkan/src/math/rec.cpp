#include "stdafx.h"
#include "math/rec.h"

using namespace std;

namespace nif
{
	rec::rec() : x_(0), y_(0), w_(0), h_(0) { }
	rec::rec(const float x, const float y, const float w, const float h) : x_(x), y_(y), w_(w), h_(h) { }

	float rec::width() const { return w_; }
	float rec::height() const { return h_; }
	float rec::left() const { return x_; }
	float rec::right() const { return x_ + w_; }
	float rec::top() const { return y_; }
	float rec::bottom() const { return y_ + h_; }
	vec2 rec::top_left() const { return vec2(left(), top()); }
	vec2 rec::top_right() const { return vec2(right(), top()); }
	vec2 rec::bottom_left() const { return vec2(left(), bottom()); }
	vec2 rec::bottom_right() const { return vec2(right(), bottom()); }

	vec2 rec::size() const
	{
		return vec2(w_, h_);
	}

	vector<rec> rec::sub(const rec& rhs) const
	{
		if (left() > rhs.right() || right() < rhs.left() || top() > rhs.bottom() || bottom() < rhs.top())
			return vector<rec>(1, *this);

		vector<rec> ret;
		if (left() < rhs.left())
			ret.push_back(rec(left(), top(), rhs.left() - left(), height()));
		if (right() > rhs.right())
			ret.push_back(rec(rhs.right(), top(), right() - rhs.right(), height()));
		if (top() < rhs.top())
			ret.push_back(rec(left(), top(), width(), rhs.top() - top()));
		if (bottom() > rhs.bottom())
			ret.push_back(rec(left(), rhs.bottom(), width(), bottom() - rhs.bottom()));
		return ret;
	}
}
