#include "stdafx.h"
#include "keyboard.h"

namespace nif {
	keyboard::keyboard() : keys_(256, false), keyhit_events_(256) {
	}

	keyboard::~keyboard() {
	}

	void keyboard::update() {
		while (!keyhits_.empty()) {
			keyhit_events_[static_cast<int>(keyhits_.front())]();
			keyhits_.pop();
		}
	}

	void keyboard::set_key(const WPARAM key, const LPARAM mod, const bool down)
	{
		UINT scancode = (mod & 0x00ff0000) >> 16;
		int extended = (mod & 0x01000000) != 0;

		WPARAM new_vk = key;
		if (key == VK_SHIFT)
			new_vk = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);
		else if (key == VK_CONTROL)
			new_vk = extended ? VK_RCONTROL : VK_LCONTROL;
		else if (key == VK_MENU)
			new_vk = extended ? VK_RMENU : VK_LMENU;

		bool prev = keys_[new_vk];
		keys_[new_vk] = down;

		if (down && !prev)
			keyhits_.push(static_cast<keys>(new_vk));
	}

	keyboard::keyevent& keyboard::keyhit(const keys key) {
		return keyhit_events_[static_cast<int>(key)];
	}
}
