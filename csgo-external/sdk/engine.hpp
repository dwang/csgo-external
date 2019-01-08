#pragma once

#include "entity.hpp"

namespace engine
{
	auto get_local_player() -> int;
	auto get_max_clients() -> int;
	auto is_in_game() -> bool;
}

namespace entity
{
	auto get_client_entity(int index) -> entity_t;
}
