#pragma once

#include "../utils/netvar_manager.hpp"
#include <cstdint>

class entity_t
{
	std::uintptr_t base;

	char chunk[0xAA50];

	int index;
	int health;
	int team;
	int flags;

public:
	auto update() -> void;
	auto get_pointer() -> std::uintptr_t;
	auto get_index() -> int;
	auto get_health() -> int;
	auto get_team() -> int;
	auto is_on_ground() -> bool;
	auto is_alive() -> bool;
};

extern entity_t local;
extern entity_t entities[64];