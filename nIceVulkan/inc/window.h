#pragma once
#include "keyboard.h"
#include "mouse.h"

namespace nif
{
	class window
	{
	public:
		using timeevent = dispatcher<window, void(double delta)>;

		window();
		window(const window&) = delete;
		~window();
		void run(double updateRate);
		void close();

		timeevent& update();
		timeevent& draw();
		keyboard::keyevent& keyhit(const keys key);
		mouse::buttonevent& buttonhit(const buttons button);
		HWND hwnd();
		HINSTANCE hinstance();
		int width() const;
		int height() const;

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		HWND hwnd_;
		HINSTANCE hinstance_;
		keyboard keyboard_;
		mouse mouse_;
		int width_ = 1440;
		int height_ = 810;
		timeevent update_;
		timeevent draw_;
	};
}
