#include "draw_manager.hpp"

void draw_manager::initialize(IDirect3DDevice9* device)
{
	this->device = device;

	D3DXCreateFontA(this->device, 18, 0, FW_LIGHT, 1, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLEARTYPE_QUALITY | ANTIALIASED_QUALITY, DEFAULT_PITCH, "Verdana", &this->fonts.fnormal);
}

void draw_manager::reset(IDirect3DDevice9* _device)
{
	device = _device;

	D3DVIEWPORT9 scrn;
	this->device->GetViewport(&scrn);

	screen.width = scrn.Width;
	screen.height = scrn.Height;
	screen.x_center = scrn.Width / 2;
	screen.y_center = scrn.Height / 2;

	this->device->SetVertexShader(nullptr);
	this->device->SetPixelShader(nullptr);
	this->device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	this->device->SetRenderState(D3DRS_LIGHTING, false);
	this->device->SetRenderState(D3DRS_FOGENABLE, false);
	this->device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	this->device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	this->device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	this->device->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
	this->device->SetRenderState(D3DRS_ZWRITEENABLE, false);
	this->device->SetRenderState(D3DRS_STENCILENABLE, false);

	this->device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, false);
	this->device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, false);

	this->device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	this->device->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	this->device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, true);
	this->device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	this->device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
	this->device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	this->device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

	this->device->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
	this->device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
}

int draw_manager::string_width(ID3DXFont* font, char* string)
{
	RECT rect = RECT();
	font->DrawTextA(nullptr, string, strlen(string), &rect, DT_CALCRECT, D3DCOLOR_RGBA(0, 0, 0, 0));
	return rect.right - rect.left;
}

void draw_manager::line(int x, int y, int x2, int y2, DWORD color)
{
	vertex vtx[2] = { { x, y, 0.0f, 1.0f, color }, { x2, y2, 0.0f, 1.0f, color } };

	this->device->SetTexture(0, nullptr);
	this->device->DrawPrimitiveUP(D3DPT_LINELIST, 1, &vtx, sizeof(vertex));
}

void draw_manager::filled_box(int x, int y, int width, int height, DWORD color)
{
	vertex pVertex[4] = { { x, y + height, 0.0f, 1.0f, color }, { x, y, 0.0f, 1.0f, color }, { x + width, y + height, 0.0f, 1.0f, color }, { x + width, y, 0.0f, 1.0f, color } };
	this->device->SetTexture(0, nullptr);
	this->device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, false);
	this->device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, pVertex, sizeof(vertex));
	this->device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, true);
}

void draw_manager::filled_box_outlined(int x, int y, int width, int height, DWORD color, DWORD outlinecolor, int thickness)
{
	this->filled_box(x, y, width, height, color);
	this->bordered_box(x, y, width, height, outlinecolor);
}

void draw_manager::bordered_box(int x, int y, int width, int height, DWORD color, int thickness)
{
	this->filled_box(x, y, width, thickness, color);
	this->filled_box(x, y, thickness, height, color);
	this->filled_box(x + width - thickness, y, thickness, height, color);
	this->filled_box(x, y + height - thickness, width, thickness, color);
}

void draw_manager::bordered_box_outlined(int x, int y, int width, int height, DWORD color, DWORD outlinecolor, int thickness)
{
	this->bordered_box(x, y, width, height, outlinecolor, thickness);
	this->bordered_box(x + thickness, y + thickness, width - (thickness * 2), height - (thickness * 2), color, thickness);
	this->bordered_box(x + (thickness * 2), y + (thickness * 2), width - (thickness * 4), height - (thickness * 4), outlinecolor, thickness);
}

void draw_manager::gradient_box(int x, int y, int width, int height, DWORD color, DWORD color2, bool vertical)
{
	vertex vtx[4] = { { x, y, 0.0f, 1.0f, color }, { x + width, y, 0.0f, 1.0f, vertical ? color : color2 }, { x, y + height, 0.0f, 1.0f, vertical ? color2 : color }, { x + width, y + height, 0.0f, 1.0f, color2 } };
	this->device->SetTexture(0, nullptr);
	this->device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, false);
	this->device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vtx, sizeof(vertex));
	this->device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, true);
}

void draw_manager::gradient_box_outlined(int x, int y, int width, int height, int thickness, DWORD color, DWORD color2, DWORD outlinecolor, bool vertical)
{
	this->gradient_box(x, y, width, height, color, color2, vertical);
	this->bordered_box(x, y, width, height, outlinecolor, thickness);
}

void draw_manager::text(char* text, float x, float y, int orientation, ID3DXFont* pFont, bool bordered, DWORD color, DWORD bcolor)
{
	RECT rect;

	switch (orientation)
	{
	case FONT_LEFT:
	{
		if (bordered)
		{
			SetRect(&rect, x - 1, y, x - 1, y);
			pFont->DrawTextA(nullptr, text, -1, &rect, DT_LEFT | DT_NOCLIP, bcolor);
			SetRect(&rect, x + 1, y, x + 1, y);
			pFont->DrawTextA(nullptr, text, -1, &rect, DT_LEFT | DT_NOCLIP, bcolor);
			SetRect(&rect, x, y - 1, x, y - 1);
			pFont->DrawTextA(nullptr, text, -1, &rect, DT_LEFT | DT_NOCLIP, bcolor);
			SetRect(&rect, x, y + 1, x, y + 1);
			pFont->DrawTextA(nullptr, text, -1, &rect, DT_LEFT | DT_NOCLIP, bcolor);
		}
		SetRect(&rect, x, y, x, y);
		pFont->DrawTextA(nullptr, text, -1, &rect, DT_LEFT | DT_NOCLIP, color);
	}
	break;
	case FONT_CENTER:
	{
		if (bordered)
		{
			SetRect(&rect, x - 1, y, x - 1, y);
			pFont->DrawTextA(nullptr, text, -1, &rect, DT_CENTER | DT_NOCLIP, bcolor);
			SetRect(&rect, x + 1, y, x + 1, y);
			pFont->DrawTextA(nullptr, text, -1, &rect, DT_CENTER | DT_NOCLIP, bcolor);
			SetRect(&rect, x, y - 1, x, y - 1);
			pFont->DrawTextA(nullptr, text, -1, &rect, DT_CENTER | DT_NOCLIP, bcolor);
			SetRect(&rect, x, y + 1, x, y + 1);
			pFont->DrawTextA(nullptr, text, -1, &rect, DT_CENTER | DT_NOCLIP, bcolor);
		}
		SetRect(&rect, x, y, x, y);
		pFont->DrawTextA(nullptr, text, -1, &rect, DT_CENTER | DT_NOCLIP, color);
	}
	break;
	case FONT_RIGHT:
	{
		if (bordered)
		{
			SetRect(&rect, x - 1, y, x - 1, y);
			pFont->DrawTextA(nullptr, text, -1, &rect, DT_RIGHT | DT_NOCLIP, bcolor);
			SetRect(&rect, x + 1, y, x + 1, y);
			pFont->DrawTextA(nullptr, text, -1, &rect, DT_RIGHT | DT_NOCLIP, bcolor);
			SetRect(&rect, x, y - 1, x, y - 1);
			pFont->DrawTextA(nullptr, text, -1, &rect, DT_RIGHT | DT_NOCLIP, bcolor);
			SetRect(&rect, x, y + 1, x, y + 1);
			pFont->DrawTextA(nullptr, text, -1, &rect, DT_RIGHT | DT_NOCLIP, bcolor);
		}
		SetRect(&rect, x, y, x, y);
		pFont->DrawTextA(nullptr, text, -1, &rect, DT_RIGHT | DT_NOCLIP, color);
	}
	break;
	}
}

void draw_manager::string(float x, float y, int orientation, ID3DXFont* pFont, bool bordered, DWORD color, const char* input, ...)
{
	char szBuffer[MAX_PATH];

	if (!input)
		return;

	DWORD bcolor = BLACK(*(byte*)(&color + 0x3));

	vsprintf_s(szBuffer, input, (char*)& input + strlen(input));

	RECT rect;

	switch (orientation)
	{
	case FONT_LEFT:
	{
		if (bordered)
		{
			SetRect(&rect, x - 1, y, x - 1, y);
			pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_LEFT | DT_NOCLIP, bcolor);
			SetRect(&rect, x + 1, y, x + 1, y);
			pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_LEFT | DT_NOCLIP, bcolor);
			SetRect(&rect, x, y - 1, x, y - 1);
			pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_LEFT | DT_NOCLIP, bcolor);
			SetRect(&rect, x, y + 1, x, y + 1);
			pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_LEFT | DT_NOCLIP, bcolor);
		}
		SetRect(&rect, x, y, x, y);
		pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_LEFT | DT_NOCLIP, color);
	}
	break;
	case FONT_CENTER:
	{
		if (bordered)
		{
			SetRect(&rect, x - 1, y, x - 1, y);
			pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_CENTER | DT_NOCLIP, bcolor);
			SetRect(&rect, x + 1, y, x + 1, y);
			pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_CENTER | DT_NOCLIP, bcolor);
			SetRect(&rect, x, y - 1, x, y - 1);
			pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_CENTER | DT_NOCLIP, bcolor);
			SetRect(&rect, x, y + 1, x, y + 1);
			pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_CENTER | DT_NOCLIP, bcolor);
		}
		SetRect(&rect, x, y, x, y);
		pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_CENTER | DT_NOCLIP, color);
	}
	break;
	case FONT_RIGHT:
	{
		if (bordered)
		{
			SetRect(&rect, x - 1, y, x - 1, y);
			pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_RIGHT | DT_NOCLIP, bcolor);
			SetRect(&rect, x + 1, y, x + 1, y);
			pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_RIGHT | DT_NOCLIP, bcolor);
			SetRect(&rect, x, y - 1, x, y - 1);
			pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_RIGHT | DT_NOCLIP, bcolor);
			SetRect(&rect, x, y + 1, x, y + 1);
			pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_RIGHT | DT_NOCLIP, bcolor);
		}
		SetRect(&rect, x, y, x, y);
		pFont->DrawTextA(nullptr, szBuffer, -1, &rect, DT_RIGHT | DT_NOCLIP, color);
	}
	break;
	}
}

void draw_manager::message(char* text, float x, float y, ID3DXFont* pFont, int orientation)
{
	RECT rect = { x, y, x, y };

	switch (orientation)
	{
	case FONT_LEFT:
	{
		pFont->DrawTextA(nullptr, text, -1, &rect, DT_CALCRECT | DT_LEFT, BLACK(255));

		this->bordered_box(x - 5, rect.top - 5, rect.right - x + 10, rect.bottom - rect.top + 10, 5, SKYBLUE(255));

		SetRect(&rect, x, y, x, y);
		pFont->DrawTextA(nullptr, text, -1, &rect, DT_LEFT | DT_NOCLIP, ORANGE(255));
	} break;
	case FONT_CENTER:
	{
		pFont->DrawTextA(nullptr, text, -1, &rect, DT_CALCRECT | DT_CENTER, BLACK(255));

		this->bordered_box(rect.left - 5, rect.top - 5, rect.right - rect.left + 10, rect.bottom - rect.top + 10, 5, SKYBLUE(255));

		SetRect(&rect, x, y, x, y);
		pFont->DrawTextA(nullptr, text, -1, &rect, DT_CENTER | DT_NOCLIP, ORANGE(255));
	}
	break;
	case FONT_RIGHT:
	{
		pFont->DrawTextA(nullptr, text, -1, &rect, DT_CALCRECT | DT_RIGHT, BLACK(255));

		this->bordered_box(rect.left - 5, rect.top - 5, rect.right - rect.left + 10, rect.bottom - rect.top + 10, 5, SKYBLUE(255));

		SetRect(&rect, x, y, x, y);
		pFont->DrawTextA(nullptr, text, -1, &rect, DT_RIGHT | DT_NOCLIP, ORANGE(255));
	}
	break;
	}
}

void draw_manager::draw_sprite(IDirect3DTexture9* tex, float x, float y, float resolution_x, float resolution_y, float scale_x, float scale_y, float rotation, DWORD color) {
	float screen_pos_x = x;
	float screen_pos_y = y;

	D3DXVECTOR2 trans = D3DXVECTOR2(screen_pos_x, screen_pos_y);

	D3DXMATRIX mat;

	scale_x = 64 / resolution_x;
	scale_y = 64 / resolution_y;

	D3DXVECTOR2 spriteCentre = D3DXVECTOR2(64 / 2, 64 / 2);

	D3DXVECTOR2 scaling(scale_x, scale_y);

	D3DXMatrixTransformation2D(&mat, nullptr, 0.0, &scaling, &spriteCentre, rotation, &trans);

	this->device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	this->device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	this->device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	this->device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	this->device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	this->device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	this->device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	this->device->SetPixelShader(nullptr);
	this->sprite->Begin(0);
	this->sprite->SetTransform(&mat);
	this->sprite->Draw(tex, nullptr, nullptr, nullptr, color);
	this->sprite->End();
}

void draw_manager::release()
{
	if (this->sprite)
		this->sprite->Release();

	if (this->white_tex)
		this->white_tex->Release();

	if (this->fonts.fnormal)
		this->fonts.fnormal->Release();
}
