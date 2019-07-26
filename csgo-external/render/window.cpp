#include "window.hpp"
#include "renderer.hpp"
#include "../menu/menu.hpp"
#include <d3d9.h>
#include <d3dx9.h>
#include <dwmapi.h>

#pragma comment(lib, "dwmapi.lib")

auto __stdcall wndproc(HWND wnd, std::uint32_t msg, std::uint32_t wparam, std::uint32_t lparam) -> std::uint32_t
{
	switch (msg)
	{
	case WM_PAINT:
		renderer::get().render();
		break;
	case WM_SIZE:
		renderer::get().reset();
		break;
	case WM_DESTROY:
		PostQuitMessage(1);
		break;
	default:
		return DefWindowProcA(wnd, msg, wparam, lparam);
	}

	return 0;
}

auto window::initialize() -> void
{
	memset(&wndclass, 0, sizeof(WNDCLASSEXA));

	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.cbSize = sizeof(WNDCLASSEXA);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = (WNDPROC)wndproc;
	wndclass.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	wndclass.hCursor = LoadCursorA(nullptr, IDC_ARROW);
	wndclass.hIcon = LoadIconA(0, IDI_APPLICATION);
	wndclass.hIconSm = LoadIconA(0, IDI_APPLICATION);
	wndclass.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	wndclass.lpszClassName = "csgo-external";
	wndclass.lpszMenuName = "csgo-external";

	RegisterClassExA(&wndclass);

	overlay = CreateWindowExA(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT, "csgo-external", "csgo-external", WS_POPUP | WS_VISIBLE, 0, 0, 3840, 2160, nullptr, nullptr, nullptr, nullptr);

	MARGINS margin = { -1, -1, -1, -1 };
	DwmExtendFrameIntoClientArea(overlay, &margin);

	SetLayeredWindowAttributes(overlay, RGB(0, 0, 0), 255, LWA_COLORKEY);

	GetWindowRect(window_hwnd, &rect);
	MoveWindow(overlay, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, true);
}

void window::handle_messages()
{
	while (PeekMessageA(&msg, overlay, 0, 0, PM_REMOVE))
	{
		DispatchMessageA(&msg);
		TranslateMessage(&msg);
	}
}