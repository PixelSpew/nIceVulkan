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

		const HWND hwnd() const;
		const HINSTANCE hinstance() const;
		const command_pool& command_pool() const;
		const swap_chain& swap_chain() const;
		int width() const;
		int height() const;

	private:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		int width_ = 1440;
		int height_ = 810;
		HINSTANCE hinstance_;
		HWND hwnd_;
		win32_surface surface_;
		nif::command_pool cmdpool_;
		nif::swap_chain swap_;
		keyboard keyboard_;
		mouse mouse_;
		timeevent update_;
		timeevent draw_;
	};
}
