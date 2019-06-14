#pragma once
#include "../utils/singleton.hpp"
#include <memory>
#include <d3d9.h>
#include <d3dx9.h>

class window : public singleton<window>
{
public:
	HWND window_hwnd = nullptr;
	HWND overlay = nullptr;
	WNDCLASSEXA wndclass;
	MSG msg;
	RECT rect{ };

	auto initialize() -> void;
	auto handle_messages() -> void;
};
