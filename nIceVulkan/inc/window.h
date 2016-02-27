#pragma once
#include "keyboard.h"
#include "mouse.h"
#include "vkwrap/device.h"

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
		const HWND hwnd() const;
		const HINSTANCE hinstance() const;
		const instance& vk_instance() const;
		const device& vk_device() const;
		int width() const;
		int height() const;

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		HWND hwnd_;
		HINSTANCE hinstance_;
		instance instance_;
		device device_;
		keyboard keyboard_;
		mouse mouse_;
		int width_ = 1440;
		int height_ = 810;
		timeevent update_;
		timeevent draw_;
	};
}
