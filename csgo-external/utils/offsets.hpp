#pragma once

#include "netvar_manager.hpp"
#include "singleton.hpp"
#include <cstdint>
#include <utility>

class offsets : public singleton<offsets>
{
public:
	void setup();

	std::pair<std::uintptr_t, std::size_t> client_dll;
	std::pair<std::uintptr_t, std::size_t> engine_dll;

	std::uintptr_t m_dwClientState;
	std::uintptr_t m_dwForceJump;
	std::uintptr_t m_dwForceAttack;
	std::uintptr_t m_dwClientState_State;
	std::uintptr_t m_dwClientState_GetLocalPlayer;
	std::uintptr_t m_bDormant;
	std::uintptr_t m_dwClientState_MaxPlayer;
	std::uintptr_t m_dwGlowObjectManager;
	std::uintptr_t m_dwEntityList;

	std::uintptr_t m_iTeamNum;
	std::uintptr_t m_fFlags;
	std::uintptr_t m_iHealth;
};
