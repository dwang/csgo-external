#pragma once

#include "entity.hpp"
#include "../utils/singleton.hpp"

class engine : public singleton<engine>
{
	std::uintptr_t base;

	char chunk[0xAA50];

	int sigon_state;
	int local_player;
	int max_clients;

public:
	auto update() -> void;
	auto get_pointer() -> std::uintptr_t;
	auto get_local_player() -> int;
	auto get_max_clients() -> int;
	auto is_in_game() -> bool;
};

namespace entity
{
	auto get_client_entity(int index) -> entity_t;
}
