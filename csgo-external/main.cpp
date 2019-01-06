#include "utils/netvar_manager.hpp"
#include "utils/remote.hpp"

auto main() -> int
{
	remote::attach_process(FNV("csgo.exe"));
	netvar_manager::get_offset_by_hash_cached<FNV("CBasePlayer->m_lifeState")>();
}
