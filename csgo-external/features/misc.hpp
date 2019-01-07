#pragma once

#include "../utils/singleton.hpp"

class misc : public singleton<misc>
{
	auto force_jump() -> void;
public:
	auto bunnyhop() -> void;
	bool bunnyhop_enabled = true;
};