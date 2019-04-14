#include "entity.hpp"
#include "engine.hpp"
#include "../utils/offsets.hpp"
#include "../utils/remote.hpp"

entity_t local;
entity_t entities[64];

#define FL_ONGROUND	(1 << 0)
#define FL_PARTIALGROUND (1 << 18)

auto entity_t::update() -> void
{
	if (!base)
		return;

	remote::raw_read(base, chunk, sizeof(chunk));

	glow_index = *reinterpret_cast<int*>(chunk + offsets::get().m_iGlowIndex);
	health = *reinterpret_cast<int*>(chunk + offsets::get().m_iHealth);
	team = *reinterpret_cast<int*>(chunk + offsets::get().m_iTeamNum);
	flags = *reinterpret_cast<int*>(chunk + offsets::get().m_fFlags);
	dormant = *reinterpret_cast<bool*>(chunk + offsets::get().m_bDormant);
}

auto entity_t::get_pointer() -> std::uintptr_t
{
	return base;
}

auto entity_t::get_glow_index() -> int
{
	return glow_index;
}

auto entity_t::is_on_ground() -> bool
{
	return flags & FL_ONGROUND || flags & FL_PARTIALGROUND;
}

auto entity_t::get_health() -> int
{
	return health;
}

auto entity_t::get_team() -> int
{
	return team;
}

auto entity_t::is_alive() -> bool
{
	return health > 0;
}

auto entity_t::is_dormant() -> bool
{
	return dormant;
}

auto entity_t::is_valid() -> bool
{
	return base && is_alive() && !is_dormant();
}
