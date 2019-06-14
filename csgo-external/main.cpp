#include "features/visuals.hpp"
#include "features/misc.hpp"
#include "sdk/entity.hpp"
#include "sdk/engine.hpp"
#include "menu/menu.hpp"
#include "utils/netvar_manager.hpp"
#include "utils/offsets.hpp"
#include "utils/remote.hpp"
#include "render/draw_manager.hpp"
#include "render/window.hpp"
#include "render/renderer.hpp"
#include <iostream>
#include <thread>
#include <Windows.h>

#define VC_EXTRALEAN

auto update() -> void
{
	while (true)
	{
		if (engine::get().is_window_focused())
		{
			engine::get().update();

			if (engine::get().is_in_game())
			{
				local = entity_t(entity::get_client_entity(engine::get().get_local_player()));
				local.update();

				for (int i = 0; i < engine::get().get_max_clients(); i++)
				{
					entities[i] = entity::get_client_entity(i);
					entities[i].update();
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

auto glow() -> void
{
	visuals::get().set_glow_update(false);

	while (true)
	{
		if (!visuals::get().glow_enabled)
			continue;

		if (engine::get().is_in_game() && engine::get().is_window_focused())
		{
			visuals::get().glow();
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

auto chams() -> void
{
	visuals::get().set_model_brightness(30.0f);

	while (true)
	{
		if (!visuals::get().chams_enabled)
			continue;

		if (engine::get().is_in_game() && engine::get().is_window_focused())
		{
			visuals::get().chams();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

auto misc() -> void
{
	visuals::get().set_tonemap_scale(0.5);

	while (true)
	{
		if (!misc::get().bunnyhop_enabled)
			continue;

		if (engine::get().is_in_game() && engine::get().is_window_focused())
		{
			if (local.is_alive())
				misc::get().bunnyhop();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

auto render() -> void
{
	window::get().initialize();

	while (true)
	{
		window::get().handle_messages();

		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
}

auto main() -> int
{
	std::cout << "csgo-external - " __DATE__ << std::endl << std::endl;

	std::cout << "> waiting for \"csgo.exe\"" << std::endl << std::endl;

	while (!window::get().window_hwnd)
		window::get().window_hwnd = FindWindow(0, "Counter-Strike: Global Offensive");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	remote::attach_process(FNV("csgo.exe"));

	std::cout << "> waiting for modules" << std::endl << std::endl;

	while (!remote::find_module(FNV("serverbrowser.dll")).first)
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	std::cout << "> scanning signatures" << std::endl << std::endl;

	offsets::get().setup();

	std::cout << "> setting up overlay" << std::endl << std::endl;

	std::thread t_render(render);
	t_render.detach();

	std::cout << "> settings up threads" << std::endl << std::endl;

	std::thread t_update(update);
	std::thread t_glow(glow);
	std::thread t_chams(chams);
	std::thread t_misc(misc);

	t_update.detach();
	t_glow.detach();
	t_chams.detach();
	t_misc.detach();

	std::cout << "> success" << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	menu::get().update();

	while (true)
	{
		engine::get().set_window_focused(window::get().window_hwnd == GetForegroundWindow());

		if (GetAsyncKeyState(VK_F1) & 0x8000)
		{
			visuals::get().glow_enabled = !visuals::get().glow_enabled;
			menu::get().update();
			Beep(330, 100);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		if (GetAsyncKeyState(VK_F2) & 0x8000)
		{
			visuals::get().chams_enabled = !visuals::get().chams_enabled;
			menu::get().update();
			Beep(330, 100);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		if (GetAsyncKeyState(VK_F3) & 0x8000)
		{
			misc::get().bunnyhop_enabled = !misc::get().bunnyhop_enabled;
			menu::get().update();
			Beep(330, 100);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		if (GetAsyncKeyState(VK_END) & 0x8000 || !FindWindow(NULL, "Counter-Strike: Global Offensive"))
		{
			visuals::get().set_glow_update(true);
			visuals::get().set_model_brightness(0.0f);
			visuals::get().set_tonemap_scale(1.0);

			t_update.~thread();
			t_glow.~thread();
			t_chams.~thread();
			t_misc.~thread();

			std::this_thread::sleep_for(std::chrono::milliseconds(100));

			remote::detach_process();

			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
