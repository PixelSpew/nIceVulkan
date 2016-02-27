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
		const command_pool& vk_command_pool() const;
		const image_view& depth_stencil_view() const;
		uint32_t vk_width() const;
		uint32_t vk_height() const;
		const swap_chain& vk_swap_chain() const;
		int width() const;
		int height() const;

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		HWND hwnd_;
		HINSTANCE hinstance_;
		instance instance_;
		device device_;
		std::unique_ptr<swap_chain> swap_;
		std::unique_ptr<command_pool> cmdpool_;
		std::unique_ptr<image> depth_stencil_image_;
		std::unique_ptr<image_view> depth_stencil_view_;
		keyboard keyboard_;
		mouse mouse_;
		int width_ = 1440;
		int height_ = 810;
		uint32_t vk_width_ = 1440;
		uint32_t vk_height_ = 810;
		timeevent update_;
		timeevent draw_;
	};
}
