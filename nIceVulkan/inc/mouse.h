#pragma once
#include "util/dispatcher.h"
#include <Windows.h>
#include <vector>
#include <queue>

namespace nif {
	enum class buttons {
		left, right, middle
	};

	class mouse {
	public:
		using buttonevent = dispatcher<mouse, void(void)>;

		mouse();
		~mouse();

		void update();
		void set_button(const buttons button, WPARAM mods, LPARAM coords, const bool down);

		buttonevent& buttonhit(const buttons button);

	private:
		std::vector<bool> buttons_;
		std::queue<buttons> buttonhits_;
		std::vector<buttonevent> buttonhit_events_;
	};
}
