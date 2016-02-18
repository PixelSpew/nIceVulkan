#pragma once
#include <vector>
#include "math/ivec2.h"

namespace nif
{
	class irec
	{
	public:
		irec();
		irec(const int x, const int y, const int w, const int h);
		int left() const;
		int right() const;
		int top() const;
		int bottom() const;
		int width() const;
		int height() const;
		ivec2 size() const;

		std::vector<irec> sub(const irec &rhs) const;

	private:
		int x_;
		int y_;
		int w_;
		int h_;
	};
}
