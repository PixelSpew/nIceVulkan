#pragma once
#include <Windows.h>
#include "util/dispatcher.h"

namespace nif
{
	class window
	{
	public:
		using timeevent = dispatcher<window, void(double delta)>;

	public:
		window();
		window(const window&) = delete;
		~window();
		void run(double updateRate);

		timeevent& update();
		timeevent& draw();
		HWND hwnd();
		HINSTANCE hinstance();
		int width() const;
		int height() const;

	private:
		HWND hwnd_;
		HINSTANCE hinstance_;
		int width_ = 1440;
		int height_ = 810;
		timeevent update_;
		timeevent draw_;
	};
}
