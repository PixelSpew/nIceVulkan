#include "stdafx.h"
#include "math/irec.h"

using namespace std;

namespace nif
{
	irec::irec() : x_(0), y_(0), w_(0), h_(0) { }
	irec::irec(const int x, const int y, const int w, const int h) : x_(x), y_(y), w_(w), h_(h) { }

	int irec::left() const { return x_; }
	int irec::right() const { return x_ + w_; }
	int irec::top() const { return y_; }
	int irec::bottom() const { return y_ + h_; }
	int irec::width() const { return w_; }
	int irec::height() const { return h_; }

	ivec2 irec::size() const
	{
		return ivec2(w_, h_);
	}

	vector<irec> irec::sub(const irec& rhs) const
	{
		if (left() > rhs.right() || right() < rhs.left() || top() > rhs.bottom() || bottom() < rhs.top())
			return vector<irec>(1, *this);

		vector<irec> ret;
		if (left() < rhs.left())
			ret.push_back(irec(left(), top(), rhs.left() - left(), height()));
		if (right() > rhs.right())
			ret.push_back(irec(rhs.right(), top(), right() - rhs.right(), height()));
		if (top() < rhs.top())
			ret.push_back(irec(left(), top(), width(), rhs.top() - top()));
		if (bottom() > rhs.bottom())
			ret.push_back(irec(left(), rhs.bottom(), width(), bottom() - rhs.bottom()));
		return ret;
	}
}
