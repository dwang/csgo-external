#include "engine.hpp"
#include "../utils/offsets.hpp"
#include "../utils/remote.hpp"

auto engine::is_in_game() -> bool
{
	auto client_state = remote::read<std::uintptr_t>(offsets::get().engine_dll.first + offsets::get().dwClientState);
	int sigon_state = remote::read<int>(client_state + offsets::get().dwClientState_State);
	return (sigon_state == 6);
}

auto engine::get_max_clients() -> int
{
	auto client_state = remote::read<std::uintptr_t>(offsets::get().engine_dll.first + offsets::get().dwClientState);
	return remote::read<int>(client_state + offsets::get().dwClientState_MaxPlayer);
}

auto engine::get_local_player() -> int
{
	auto client_state = remote::read<std::uintptr_t>(offsets::get().engine_dll.first + offsets::get().dwClientState);
	return remote::read<int>(client_state + offsets::get().dwClientState_GetLocalPlayer);
}

auto entity::get_client_entity(int index) -> entity_t
{
	return remote::read<entity_t>(offsets::get().client_dll.first + offsets::get().dwEntityList + index * 0x10);
}
