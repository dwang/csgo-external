#include "aim.hpp"

#include "../sdk/entity.hpp"
#include "../sdk/engine.hpp"
#include "../utils/offsets.hpp"
#include "../utils/remote.hpp"
#include <Windows.h>
#include <thread>
#include <iostream>

auto aim::force_attack() -> void
{
	remote::write<int>(offsets::get().client_dll.first + offsets::get().dwForceAttack, 6);
}

auto aim::triggerbot() -> void
{
	if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000)
	{
		std::cout << local.get_crosshair_id() << std::endl;

		if (local.get_crosshair_id() > 0 && local.get_crosshair_id() < 64)
		{
			

			auto entity = entities[local.get_crosshair_id()];

			if (local.get_team() == entity.get_team())
				return;

			if (entity.is_valid())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
				force_attack();
			}
		}
	}
}