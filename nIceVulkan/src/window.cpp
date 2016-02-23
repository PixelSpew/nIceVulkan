#include "stdafx.h"
#include "window.h"
#include <chrono>
#include <map>

using namespace std;

namespace nif
{
	map<HWND, reference_wrapper<window>> windows;

	window::window()
	{
		GetModuleHandleEx(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			(LPCTSTR)WndProc,
			&hinstance_);

		LPCTSTR class_name = L"nIce Framework";
		WNDCLASSEXW wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hinstance_;
		wcex.hIcon = nullptr;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = nullptr;
		wcex.lpszClassName = class_name;
		wcex.hIconSm = nullptr;
		RegisterClassExW(&wcex);

		WNDCLASS wc;
		wc.lpfnWndProc = WndProc;
		wc.hInstance = hinstance_;
		wc.lpszClassName = class_name;
		hwnd_ = CreateWindowEx(
			0, class_name, L"nIce Framework", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			CW_USEDEFAULT, CW_USEDEFAULT, width_, height_,
			NULL, NULL, hinstance_, NULL);
		ShowWindow(hwnd_, SW_SHOW);
		UpdateWindow(hwnd_);

		windows.insert(pair<const HWND, reference_wrapper<window>>(hwnd_, *this));
	}

	window::~window()
	{
		DestroyWindow(hwnd_);
	}

	void window::run(double updateRate)
	{
		typedef chrono::high_resolution_clock clock;

		auto previous = clock::now();
		double lag = 0.0;
		while (true)
		{
			auto current = clock::now();
			double elapsed = chrono::duration_cast<chrono::nanoseconds>(current - previous).count() / 1000000000.0;
			previous = current;
			lag += elapsed;

			MSG msg;
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					return;

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			keyboard_.update();

			while (lag >= updateRate)
			{
				update_(updateRate);
				lag -= updateRate;
			}

			elapsed += chrono::duration_cast<chrono::nanoseconds>(clock::now() - previous).count() / 1000000000.0;

			draw_(elapsed);
		}
	}

	void window::close()
	{
		PostMessage(hwnd_, WM_CLOSE, 0, 0);
	}

	window::timeevent& window::update()
	{
		return update_;
	}

	window::timeevent & window::draw()
	{
		return draw_;
	}

	keyboard::keyevent &window::keyhit(const keys key)
	{
		return keyboard_.keyhit(key);
	}

	HWND window::hwnd()
	{
		return hwnd_;
	}

	HINSTANCE window::hinstance()
	{
		return hinstance_;
	}

	int window::width() const
	{
		return width_;
	}

	int window::height() const
	{
		return height_;
	}

	LRESULT CALLBACK window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		}
		break;
		case WM_KEYDOWN:
		{
			auto win = windows.find(hWnd);
			if (win != windows.end())
				win->second.get().keyboard_.set_key(wParam, lParam, true);
		}
		break;
		case WM_KEYUP:
		{
			auto win = windows.find(hWnd);
			if (win != windows.end())
				win->second.get().keyboard_.set_key(wParam, lParam, false);
		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}
}
