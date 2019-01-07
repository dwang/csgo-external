#include "offsets.hpp"
#include "remote.hpp"

void offsets::setup()
{
	static auto find_ptr = [](const std::pair<std::uintptr_t, std::size_t> mod, const char* sig, std::uintptr_t sig_add, std::uintptr_t off_add, bool sub_base = true) -> std::uintptr_t
	{
		auto off = remote::find_pattern(mod, sig);
		auto sb = sub_base ? mod.first : 0;
		off = remote::read<std::uintptr_t>(off + sig_add);

		return (!off ? 0 : off + off_add - sb);
	};

	client_dll = remote::find_module_by_name(FNV("client_panorama.dll"));
	engine_dll = remote::find_module_by_name(FNV("engine.dll"));

	m_dwClientState = find_ptr(engine_dll, "A1 ? ? ? ? 33 D2 6A 00 6A 00 33 C9 89 B0", 0x1, 0);
	m_dwClientState_State = find_ptr(engine_dll, "83 B8 ? ? ? ? 06 0F 94 C0 C3", 0x2, 0, false);
	m_dwClientState_GetLocalPlayer = find_ptr(engine_dll, "8B 80 ? ? ? ? 40 C3", 0x2, 0, false);
	m_dwClientState_MaxPlayer = find_ptr(engine_dll, "A1 ? ? ? ? 8B 80 ? ? ? ? C3 CC CC CC CC 55 8B EC 8A 45 08", 0x7, 0, false);
	m_bDormant = find_ptr(client_dll, "88 9E ? ? ? ? E8 ? ? ? ? 53 8D 8E ? ? ? ? E8 ? ? ? ? 8B 06 8B CE 53 FF 90 ? ? ? ? 8B 46 64 0F B6 CB 5E 5B 66 89 0C C5 ? ? ? ? 5D C2 04 00", 0x2, 0, false);
	m_dwForceJump = find_ptr(client_dll, "89 0D ? ? ? ? 8B 0D ? ? ? ? 8B F2 8B C1 83 CE 08", 0x2, 0);
	m_dwForceAttack = find_ptr(client_dll, "89 0D ? ? ? ? 8B 0D ? ? ? ? 8B F2 8B C1 83 CE 04", 0x2, 0);
	m_dwGlowObjectManager = find_ptr(client_dll, "A1 ? ? ? ? A8 01 75 4B", 0x1, 0x4);
	m_dwEntityList = find_ptr(client_dll, "BB ? ? ? ? 83 FF 01 0F 8C ? ? ? ? 3B F8", 0x1, 0);

	m_iTeamNum = netvar_manager::get_offset_by_hash_cached<FNV("CBaseEntity->m_iTeamNum")>();
	m_fFlags = netvar_manager::get_offset_by_hash_cached<FNV("CBasePlayer->m_fFlags")>();
	m_iHealth = netvar_manager::get_offset_by_hash_cached<FNV("CBasePlayer->m_iHealth")>();

#ifdef _DEBUG
	printf(
		"[*] modules:\n"
		"    client_panorama.dll:             0x%x\n"
		"    engine.dll:                      0x%x\n"
		"[*] offsets:\n"
		"    m_dwClientState:                 0x%x\n"
		"    m_dwClientState_State:           0x%x\n"
		"    m_dwClientState_MaxPlayer:       0x%x\n"
		"    m_dwClientState_GetLocalPlayer:  0x%x\n"
		"    m_dwEntityList:                  0x%x\n"
		"    m_dwGlowObjectManager:           0x%x\n"
		"    m_bDormant:                      0x%x\n"
		"    m_dwForceJump:                   0x%x\n"
		"    m_dwForceAttack:                 0x%x\n"
		"[*] netvars:\n"
		"   CBaseEntity->m_iTeamNum:          0x%x\n"
		"   CBasePlayer->m_fFlags:            0x%x\n"
		"   CBasePlayer->m_iHealth:           0x%x\n\n"
		,
		client_dll.first,
		engine_dll.first,
		m_dwClientState,
		m_dwClientState_State,
		m_dwClientState_MaxPlayer,
		m_dwClientState_GetLocalPlayer,
		m_dwEntityList,
		m_dwGlowObjectManager,
		m_bDormant,
		m_dwForceJump,
		m_dwForceAttack,
		m_iTeamNum,
		m_fFlags,
		m_iHealth
	);
#endif
}
