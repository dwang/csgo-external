#include "features/visuals.hpp"
#include "features/misc.hpp"
#include "sdk/entity.hpp"
#include "sdk/engine.hpp"
#include "utils/netvar_manager.hpp"
#include "utils/offsets.hpp"
#include "utils/remote.hpp"
#include <iostream>
#include <thread>
#include <Windows.h>

auto update_entities() -> void
{
	while (true)
	{
		if (engine::is_in_game())
		{
			local = entity::get_client_entity(engine::get_local_player());
			local.update();
				
			for (int i = 0; i < 64; ++i)
			{
				entities[i] = entity::get_client_entity(i);
				entities[i].update();
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

auto visuals() -> void
{
	while (true)
	{
		if (engine::is_in_game())
		{
			visuals::get().glow();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

auto misc() -> void
{
	while (true)
	{
		if (engine::is_in_game())
		{
			if (local.is_alive())
				misc::get().bunnyhop();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

auto main() -> int
{
	std::cout << "csgo-external" << std::endl << std::endl;

	std::cout << "> waiting for \"csgo.exe\"" << std::endl << std::endl;

	while (!FindWindow(0, "Counter-Strike: Global Offensive"))
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	remote::attach_process(FNV("csgo.exe"));

	std::cout << "> waiting for modules" << std::endl << std::endl;

	while (!remote::find_module(FNV("serverbrowser.dll")).first)
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	std::cout << "> scanning signatures" << std::endl << std::endl;

	offsets::get().setup();

	std::cout << "> settings up threads" << std::endl << std::endl;

	std::thread t_update(update_entities);
	std::thread t_visuals(visuals);
	std::thread t_misc(misc);

	t_update.detach();
	t_visuals.detach();
	t_misc.detach();

	std::cout << "> success" << std::endl;	

	while (true)
	{
		if (GetAsyncKeyState(VK_END) & 0x8000 || !FindWindow(NULL, "Counter-Strike: Global Offensive"))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remote::detach_process();
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}
