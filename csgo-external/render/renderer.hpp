#pragma once
#include "../utils/singleton.hpp"
#include <memory>
#include <d3d9.h>
#include <d3dx9.h>

class renderer : public singleton<renderer>
{
public:
	bool renderer_init = false;
	IDirect3D9* d3d;
	IDirect3DDevice9* d3d_device;
	D3DPRESENT_PARAMETERS params;
	D3DVIEWPORT9 viewport;

	void init();
	void reset();
	void render();
};
