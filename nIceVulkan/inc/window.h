#pragma once
#include "keyboard.h"
#include "mouse.h"
#include "vkwrap/swap_chain.h"

namespace nif
{
	class window
	{
	public:
		using timeevent = dispatcher<window, void(double delta)>;

		window(const device &device);
		window(const window&) = delete;
		~window();
		void run(double updateRate);
		void close();

		timeevent& update();
		timeevent& draw();
		keyboard::keyevent& keyhit(const keys key);
		mouse::buttonevent& buttonhit(const buttons button);

		const command_pool& command_pool() const;
		const swap_chain& swap_chain() const;
		int width() const;
		int height() const;

	private:
		int width_ = 1440;
		int height_ = 810;

#ifdef _WIN32
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		HINSTANCE hinstance_;
		HWND hwnd_;
		surface_win32 surface_;
#else
		Display *display_;
		Window handle_;
		surface_xlib surface_;
#endif

		nif::command_pool cmdpool_;
		nif::swap_chain swap_;
		keyboard keyboard_;
		mouse mouse_;
		timeevent update_;
		timeevent draw_;
	};
}
