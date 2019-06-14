#include "menu.hpp"

#include "../features/visuals.hpp"
#include "../features/misc.hpp"

#include <iostream>

void menu::update()
{
	system("cls");

	printf(
		"\ncsgo-external\n\n"
		"[*] glow [F1] = %s\n\n"
		"[*] chams [F2] = %s\n\n"
		"[*] bunnyhop [F3] = %s\n\n"
		"[*] detach [END]"
		,
		visuals::get().glow_enabled ? "enabled" : "disabled",
		visuals::get().chams_enabled ? "enabled" : "disabled",
		misc::get().bunnyhop_enabled ? "enabled" : "disabled"
	);
}