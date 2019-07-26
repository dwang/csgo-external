#include "menu.hpp"

#include "../features/aim.hpp"
#include "../features/visuals.hpp"
#include "../features/misc.hpp"
#include "../render/renderer.hpp"
#include "../render/window.hpp"
#include "../render/draw_manager.hpp"

auto menu::initialize() -> void
{
	items =
	{
		{ "glow", &visuals::get().glow_enabled },
		{ "chams", &visuals::get().chams_enabled },
		{ "triggerbot", &aim::get().triggerbot_enabled },
		{ "bunnyhop", &misc::get().bunnyhop_enabled }
	};
}

auto menu::render() -> void
{
	draw_manager::get().string(5, 0, FONT_LEFT, draw_manager::get().fonts.fnormal, false, WHITE(255), "csgo-external");

	if (!opened)
		return;

	for (int i = 0; i < items.size(); i++)
	{
		draw_manager::get().string(250, 30 + (i * 25), FONT_LEFT, draw_manager::get().fonts.fmenu, true, i == selected ? PINK(255) : WHITE(255), items[i].name.c_str());
		draw_manager::get().string(250 + 120, 30 + (i * 25), FONT_LEFT, draw_manager::get().fonts.fmenu, true, *items[i].container ? BLUE(255) : WHITE(255), *items[i].container ? "on" : "off");
	}
}