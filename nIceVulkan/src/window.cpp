#include "stdafx.h"
#include "window.h"
#include <chrono>
#include <map>

using namespace std;

namespace nif
{
	map<HWND, reference_wrapper<window>> windows;

	namespace window_static {
		HINSTANCE get_hinstance()
		{
			HINSTANCE ret;
			GetModuleHandleEx(
				GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
				(LPCTSTR)get_hinstance,
				&ret);
			return ret;
		}

		HWND make_window(HINSTANCE hinstance, WNDPROC wndProc, int width, int height)
		{
			GetModuleHandleEx(
				GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
				reinterpret_cast<LPCTSTR>(wndProc),
				&hinstance);

			LPCTSTR class_name = L"nIce Framework";
			WNDCLASSEXW wcex;
			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW;
			wcex.lpfnWndProc = wndProc;
			wcex.cbClsExtra = 0;
			wcex.cbWndExtra = 0;
			wcex.hInstance = hinstance;
			wcex.hIcon = nullptr;
			wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
			wcex.lpszMenuName = nullptr;
			wcex.lpszClassName = class_name;
			wcex.hIconSm = nullptr;
			RegisterClassExW(&wcex);

			return CreateWindowEx(
				0, class_name, L"nIce Framework", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
				CW_USEDEFAULT, CW_USEDEFAULT, width, height,
				NULL, NULL, hinstance, NULL);
		}
	}

#define S window_static

	window::window(const device &device) :
		hinstance_(S::get_hinstance()),
		hwnd_(S::make_window(hinstance_, WndProc, width_, height_)),
		surface_(device, hinstance_, hwnd_),
		swap_(device, surface_, hinstance_, hwnd_),
		cmdpool_(swap_.surface()),
		depth_stencil_image_(width_, height_, device),	// todo: use vk_width_ and vk_height_
		depth_stencil_view_(depth_stencil_image_, device.depth_format(), vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil)
	{
		ShowWindow(hwnd_, SW_SHOW);
		UpdateWindow(hwnd_);

		windows.insert(pair<const HWND, reference_wrapper<window>>(hwnd_, *this));

		command_buffer setupCmdBuffer(cmdpool_);
		setupCmdBuffer.begin();
		swap_.setup(setupCmdBuffer);
		setupCmdBuffer.setImageLayout(depth_stencil_image_, vk::ImageAspectFlagBits::eDepth, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
		setupCmdBuffer.end();
		setupCmdBuffer.submit(device);
		device.wait_queue_idle();
	}

	window::~window()
	{
		DestroyWindow(hwnd_);
	}

	void window::run(double updateRate)
	{
		typedef chrono::high_resolution_clock clock;

		updateRate = 1 / updateRate;
		auto previous = clock::now();
		double lag = 0.0;
		while (true) {
			auto current = clock::now();
			double elapsed = chrono::duration_cast<chrono::nanoseconds>(current - previous).count() / 1000000000.0;
			previous = current;
			lag += elapsed;

			while (lag >= updateRate) {
				MSG msg;
				while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
					if (msg.message == WM_QUIT)
						return;

					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				keyboard_.update();

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

	mouse::buttonevent & window::buttonhit(const buttons button)
	{
		return mouse_.buttonhit(button);
	}

	const HWND window::hwnd() const
	{
		return hwnd_;
	}

	const HINSTANCE window::hinstance() const
	{
		return hinstance_;
	}

	const command_pool& window::vk_command_pool() const
	{
		return cmdpool_;
	}

	const image_view& window::depth_stencil_view() const
	{
		return depth_stencil_view_;
	}

	const swap_chain& window::vk_swap_chain() const
	{
		return swap_;
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
		switch (message) {
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: Add any drawing code that uses hdc here...
			EndPaint(hWnd, &ps);
		} break;
		case WM_KEYDOWN:
		{
			auto win = windows.find(hWnd);
			if (win != windows.end())
				win->second.get().keyboard_.set_key(wParam, lParam, true);
		} break;
		case WM_KEYUP:
		{
			auto win = windows.find(hWnd);
			if (win != windows.end())
				win->second.get().keyboard_.set_key(wParam, lParam, false);
		}
		break;
		case WM_LBUTTONDOWN:
		{
			auto win = windows.find(hWnd);
			if (win != windows.end())
				win->second.get().mouse_.set_button(buttons::left, wParam, lParam, true);
		}
		break;
		case WM_LBUTTONUP:
		{
			auto win = windows.find(hWnd);
			if (win != windows.end())
				win->second.get().mouse_.set_button(buttons::left, wParam, lParam, false);
		}
		break;
		case WM_RBUTTONDOWN:
		{
			auto win = windows.find(hWnd);
			if (win != windows.end())
				win->second.get().mouse_.set_button(buttons::right, wParam, lParam, true);
		}
		break;
		case WM_RBUTTONUP:
		{
			auto win = windows.find(hWnd);
			if (win != windows.end())
				win->second.get().mouse_.set_button(buttons::right, wParam, lParam, false);
		}
		break;
		case WM_MBUTTONDOWN:
		{
			auto win = windows.find(hWnd);
			if (win != windows.end())
				win->second.get().mouse_.set_button(buttons::middle, wParam, lParam, true);
		}
		break;
		case WM_MBUTTONUP:
		{
			auto win = windows.find(hWnd);
			if (win != windows.end())
				win->second.get().mouse_.set_button(buttons::middle, wParam, lParam, false);
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
