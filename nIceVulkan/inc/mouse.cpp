#include "stdafx.h"
#include "mouse.h"

namespace nif {
	mouse::mouse() : buttons_(3), buttonhit_events_(3) {
	}

	mouse::~mouse() {
	}

	void mouse::update() {
		while (!buttonhits_.empty()) {
			buttonhit_events_[static_cast<int>(buttonhits_.front())]();
			buttonhits_.pop();
		}
	}

	void mouse::set_button(const buttons button, WPARAM mods, LPARAM coords, const bool down) {
		int bnum = static_cast<int>(button);
		bool prev = buttons_[bnum];
		buttons_[bnum] = down;

		if (down && !prev)
			buttonhits_.push(button);
	}

	mouse::buttonevent& mouse::buttonhit(const buttons button) {
		return buttonhit_events_[static_cast<int>(button)];
	}
}
