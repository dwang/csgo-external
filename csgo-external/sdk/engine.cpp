#include "engine.hpp"
#include "../utils/offsets.hpp"
#include "../utils/remote.hpp"

auto engine::update() -> void
{
	if (!base)
		base = remote::read<std::uintptr_t>(offsets::get().engine_dll.first + offsets::get().dwClientState);

	remote::raw_read(base, chunk, sizeof(chunk));
	sigon_state = *reinterpret_cast<int*>(chunk + offsets::get().dwClientState_State);
	max_clients = *reinterpret_cast<int*>(chunk + offsets::get().dwClientState_MaxPlayer);
	local_player = *reinterpret_cast<int*>(chunk + offsets::get().dwClientState_GetLocalPlayer);
}

auto engine::get_pointer() -> std::uintptr_t
{
	return base;
}

auto engine::is_in_game() -> bool
{
	return sigon_state == 6;
}

auto engine::get_max_clients() -> int
{
	return max_clients;
}

auto engine::get_local_player() -> int
{
	return local_player;
}

auto engine::is_window_focused() -> bool
{
	return window_focused;
}

auto engine::set_window_focused(bool focused) -> void
{
	window_focused = focused;
}

auto entity::get_client_entity(int index) -> entity_t
{
	return remote::read<entity_t>(offsets::get().client_dll.first + offsets::get().dwEntityList + index * 0x10);
}
