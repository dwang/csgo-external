#include "offsets.hpp"
#include "remote.hpp"

auto offsets::setup() -> void
{
	static auto find_ptr = [](const std::pair<std::uintptr_t, std::size_t> mod, const char* sig, std::uintptr_t sig_add, std::uintptr_t off_add, bool sub_base = true) -> std::uintptr_t
	{
		auto off = remote::find_pattern(mod, sig);
		auto sb = sub_base ? mod.first : 0;
		off = remote::read<std::uintptr_t>(off + sig_add);

		return (!off ? 0 : off + off_add - sb);
	};

	client_dll = remote::find_module(FNV("client_panorama.dll"));
	engine_dll = remote::find_module(FNV("engine.dll"));

	dwClientState = find_ptr(engine_dll, "A1 ? ? ? ? 33 D2 6A 00 6A 00 33 C9 89 B0", 0x1, 0);
	dwClientState_State = find_ptr(engine_dll, "83 B8 ? ? ? ? 06 0F 94 C0 C3", 0x2, 0, false);
	dwClientState_GetLocalPlayer = find_ptr(engine_dll, "8B 80 ? ? ? ? 40 C3", 0x2, 0, false);
	dwClientState_MaxPlayer = find_ptr(engine_dll, "A1 ? ? ? ? 8B 80 ? ? ? ? C3 CC CC CC CC 55 8B EC 8A 45 08", 0x7, 0, false);
	m_bDormant = find_ptr(client_dll, "88 9E ? ? ? ? E8 ? ? ? ? 53 8D 8E ? ? ? ? E8 ? ? ? ? 8B 06 8B CE 53 FF 90 ? ? ? ? 8B 46 64 0F B6 CB 5E 5B 66 89 0C C5 ? ? ? ? 5D C2 04 00", 0x2, 0, false);
	dwForceJump = find_ptr(client_dll, "89 0D ? ? ? ? 8B 0D ? ? ? ? 8B F2 8B C1 83 CE 08", 0x2, 0);
	dwForceAttack = find_ptr(client_dll, "89 0D ? ? ? ? 8B 0D ? ? ? ? 8B F2 8B C1 83 CE 04", 0x2, 0);
	dwGlowObjectManager = find_ptr(client_dll, "A1 ? ? ? ? A8 01 75 4B", 0x1, 0x4);
	dwEntityList = find_ptr(client_dll, "BB ? ? ? ? 83 FF 01 0F 8C ? ? ? ? 3B F8", 0x1, 0);
	dwGetAllClasses = find_ptr(client_dll, "A1 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC A1 ? ? ? ? B9", 0x1, 0, false);

	m_iTeamNum = netvar_manager::get_offset_by_hash_cached<FNV("CBaseEntity->m_iTeamNum")>();
	m_fFlags = netvar_manager::get_offset_by_hash_cached<FNV("CBasePlayer->m_fFlags")>();
	m_iHealth = netvar_manager::get_offset_by_hash_cached<FNV("CBasePlayer->m_iHealth")>();
	m_flFlashDuration = netvar_manager::get_offset_by_hash_cached<FNV("CCSPlayer->m_flFlashDuration")>();
	m_iGlowIndex = m_flFlashDuration + 24;

#ifdef _DEBUG
	printf(
		"[*] modules:\n"
		"    client_panorama.dll:          0x%x\n"
		"    engine.dll:                   0x%x\n"
		"[*] offsets:\n"
		"    dwClientState:                0x%x\n"
		"    dwClientState_State:          0x%x\n"
		"    dwClientState_MaxPlayer:      0x%x\n"
		"    dwClientState_GetLocalPlayer: 0x%x\n"
		"    dwEntityList:                 0x%x\n"
		"    dwGlowObjectManager:          0x%x\n"
		"    m_bDormant:                   0x%x\n"
		"    dwForceJump:                  0x%x\n"
		"    dwForceAttack:                0x%x\n"
		"    dwGetAllClasses:              0x%x\n"
		"    m_iGlowIndex:                 0x%x\n"
		"[*] netvars:\n"
		"   CBaseEntity->m_iTeamNum:       0x%x\n"
		"   CBasePlayer->m_fFlags:         0x%x\n"
		"   CBasePlayer->m_iHealth:        0x%x\n"
		"   CCSPlayer->m_flFlashDuration:  0x%x\n\n"
		,
		client_dll.first,
		engine_dll.first,
		dwClientState,
		dwClientState_State,
		dwClientState_MaxPlayer,
		dwClientState_GetLocalPlayer,
		dwEntityList,
		dwGlowObjectManager,
		m_bDormant,
		dwForceJump,
		dwForceAttack,
		dwGetAllClasses,
		m_iGlowIndex,
		m_iTeamNum,
		m_fFlags,
		m_iHealth,
		m_flFlashDuration
	);
#endif
}
