#pragma once
#include <Windows.h>

class window
{
public:
	window();
	~window();
	HWND hwnd();
	HINSTANCE hinstance();
	int width() const;
	int height() const;

private:
	HWND hwnd_;
	HINSTANCE hinstance_;
	int width_ = 1440;
	int height_ = 910;
};
