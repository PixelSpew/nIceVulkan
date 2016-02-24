#pragma once
#include "util/dispatcher.h"
#include <Windows.h>
#include <vector>
#include <queue>

namespace nif
{
	enum class keys
	{
		mouse_left = 1, mouse_right, cancel, mouse_middle, mouse_x1, mouse_x2,
		backspace = 8, tab, clear = 12, enter, pause = 19, capslock,
		escape = 27, space = 32, pageup, pagedown, end, home, left, up, right,
		down, select, print, execute, printsc, insert, del, help, k0, k1, k2,
		k3, k4, k5, k6, k7, k8, k9, a = 65, b, c, d, e, f, g, h, i, j, k, l, m,
		n, o, p, q, r, s, t, u, v, w, x, y, z, lwin, rwin, apps, sleep = 95,
		n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, multiply, add, separator,
		subtract, decimal, divide, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10,
		f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24,
		numlock = 144, scrolllock, lshift = 160, rshift, lcontrol, rcontrol,
		lmenu, rmenu, browserback, browserforward, browserrefresh, browserstop,
		browsersearch, browserfavorites, browserhome, volmute, voldown, volup,
		medianext, mediaprev, mediastop, mediaplaypause, launchmail,
		launchmedia, launchapp1, launchapp2, semicolon = 186, oemplus,
		oemcomma, oemminus, oemperiod, slash, tilde, lbracket = 219, backslash,
		rbracket, quote, oem1 = semicolon, oem_2 = slash, oem_3 = tilde,
		oem_4 = lbracket, oem_5 = backslash, oem_6 = rbracket, oem_7 = quote,
		oem_8, play = 250, zoom
	};

	class keyboard
	{
	public:
		using keyevent = dispatcher<keyboard, void(void)>;

		keyboard();
		~keyboard();
		void update();
		void set_key(const WPARAM key, const LPARAM mod, const bool down);

		keyevent& keyhit(const keys key);

	private:
		std::vector<bool> keys_;
		std::queue<keys> keyhits_;
		std::vector<keyevent> keyhit_events_;
	};
}
