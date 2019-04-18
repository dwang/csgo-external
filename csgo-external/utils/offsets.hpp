#pragma once

#include "netvar_manager.hpp"
#include "singleton.hpp"
#include <cstdint>
#include <utility>

class offsets : public singleton<offsets>
{
public:
	auto setup() -> void;

	std::pair<std::uintptr_t, std::size_t> client_dll;
	std::pair<std::uintptr_t, std::size_t> engine_dll;

	std::uintptr_t dwClientState;
	std::uintptr_t dwForceJump;
	std::uintptr_t dwForceAttack;
	std::uintptr_t dwClientState_State;
	std::uintptr_t dwClientState_GetLocalPlayer;
	std::uintptr_t m_bDormant;
	std::uintptr_t dwClientState_MaxPlayer;
	std::uintptr_t dwGlowObjectManager;
	std::uintptr_t dwEntityList;
	std::uintptr_t dwGetAllClasses;
	std::uintptr_t dwModelAmbientMin;
	std::uintptr_t dwGlowUpdate;

	std::uintptr_t m_iTeamNum;
	std::uintptr_t m_fFlags;
	std::uintptr_t m_iHealth;
	std::uintptr_t m_flFlashDuration;
	std::uintptr_t m_iGlowIndex;
};
