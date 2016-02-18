#include "stdafx.h"
#include "buffer.h"
#include "math/vec3.h"
#include "swap_chain.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace nif;

struct vertex
{
	vec3 pos;
};

int exit(string message)
{
	cout << message << endl;
	cin.ignore();
	return 1;
}

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

int main()
{
	vector<vertex> vertices = { { vec3(1, 1, 0) }, { vec3(-1, 1, 0) }, { vec3(0, -1, 0) } };
	std::vector<unsigned int> indices = { 0, 1, 2 };

	instance vkinstance("nIce Framework");
	device vkdevice(vkinstance);
	buffer<vertex> vbuffer(vkdevice, vk::BufferUsageFlagBits::eVertexBuffer, vertices);
	buffer<uint32_t> ibuffer(vkdevice, vk::BufferUsageFlagBits::eIndexBuffer, indices);

	vbuffer.bind_descs().resize(1);
	vbuffer.bind_descs()[0].stride(sizeof(vertex));

	vbuffer.attrib_descs().resize(1);
	vbuffer.attrib_descs()[0].location(0);
	vbuffer.attrib_descs()[0].format(vk::Format::eR32G32B32Sfloat);
	vbuffer.attrib_descs()[0].offset(0);

	vbuffer.pipeline_info().vertexBindingDescriptionCount(static_cast<uint32_t>(vbuffer.bind_descs().size()));
	vbuffer.pipeline_info().pVertexBindingDescriptions(vbuffer.bind_descs().data());
	vbuffer.pipeline_info().vertexAttributeDescriptionCount(static_cast<uint32_t>(vbuffer.attrib_descs().size()));
	vbuffer.pipeline_info().pVertexAttributeDescriptions(vbuffer.attrib_descs().data());

	//make window
	HINSTANCE hInstance;
	GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
		(LPCTSTR)main,
		&hInstance);

	LPCTSTR class_name = L"nIce Framework";
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = nullptr;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = class_name;
	wcex.hIconSm = nullptr;
	RegisterClassExW(&wcex);

	WNDCLASS wc;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = class_name;
	HWND hwnd = CreateWindowEx(
		0, class_name, L"nIce Framework", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL
	);
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	vk::SemaphoreCreateInfo presentCompleteSemaphoreCreateInfo;
	vk::Semaphore presentCompleteSemaphore;
	vk::createSemaphore(vkdevice.handle(), &presentCompleteSemaphoreCreateInfo, nullptr, &presentCompleteSemaphore);

	swap_chain swap(vkinstance, vkdevice, hInstance, hwnd);
	//swap.acquireNextImage(presentCompleteSemaphore, &currentBuffer);
}

