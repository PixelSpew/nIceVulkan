#include "stdafx.h"
#include "window.h"

namespace nif
{
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

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
			NULL, NULL, hinstance_, NULL
			);
		ShowWindow(hwnd_, SW_SHOW);
		UpdateWindow(hwnd_);
	}

	window::~window()
	{
		DestroyWindow(hwnd_);
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
}
