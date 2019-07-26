#include <deque>
#include <ctime>
#include <chrono>
#include "renderer.hpp"
#include "draw_manager.hpp"
#include "window.hpp"
#include "..//menu/menu.hpp"
#include "../sdk/engine.hpp"

void renderer::init()
{
	memset(&params, 0, sizeof(params));

	params.EnableAutoDepthStencil = true;
	params.AutoDepthStencilFormat = D3DFMT_D16;
	params.Windowed = true;
	params.BackBufferCount = 1;
	params.BackBufferFormat = D3DFMT_A8R8G8B8;
	params.BackBufferWidth = window::get().rect.right - window::get().rect.left;
	params.BackBufferHeight = window::get().rect.bottom - window::get().rect.top;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	params.hDeviceWindow = window::get().overlay;

	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return;

	if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window::get().overlay, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &params, &d3d_device) != D3D_OK)
		return;

	if (!d3d_device)
		return;
}

void renderer::reset()
{
	if (d3d_device)
	{
		params.BackBufferWidth = window::get().rect.left - window::get().rect.right;
		params.BackBufferHeight = window::get().rect.bottom - window::get().rect.top;

		d3d_device->Reset(&params);
		draw_manager::get().initialize(d3d_device);
	}
}

void renderer::render()
{
	static bool once = false;

	if (!once)
	{
		init();
		draw_manager::get().initialize(d3d_device);
		once = true;
	}

	if (!d3d_device)
		return;

	d3d_device->Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);
	d3d_device->BeginScene();

	draw_manager::get().reset(d3d_device);

	if (engine::get().is_window_focused())
	{
		menu::get().render();
	}

	d3d_device->EndScene();
	d3d_device->Present(nullptr, nullptr, nullptr, nullptr);
}
