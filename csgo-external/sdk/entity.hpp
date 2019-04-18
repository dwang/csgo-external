#pragma once

#include "../utils/netvar_manager.hpp"
#include <cstdint>
#include <vector>

class entity_t
{
	std::uintptr_t base;

	char chunk[0xAA50];

	int glow_index;
	int health;
	int team;
	int flags;
	bool dormant;

public:
	auto update() -> void;
	auto get_pointer() -> std::uintptr_t;
	auto get_glow_index() -> int;
	auto get_health() -> int;
	auto get_team() -> int;
	auto get_class_id() -> int;
	auto is_on_ground() -> bool;
	auto is_alive() -> bool;
	auto is_dormant() -> bool;
	auto is_valid() -> bool;
};

extern entity_t local;
extern entity_t entities[64];
