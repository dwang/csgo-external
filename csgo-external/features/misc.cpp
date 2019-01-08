#include "misc.hpp"
#include "../sdk/entity.hpp"
#include "../utils/offsets.hpp"
#include "../utils/remote.hpp"
#include <Windows.h>

auto misc::force_jump() -> void
{
	remote::write<int>(offsets::get().client_dll.first + offsets::get().dwForceJump, 6);
}

auto misc::bunnyhop() -> void
{
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (local.is_on_ground())
			force_jump();
	}
}
