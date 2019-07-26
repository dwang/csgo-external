#pragma once

#include "../utils/singleton.hpp"

class aim : public singleton<aim>
{
	auto force_attack() -> void;
public:
	auto triggerbot() -> void;
	bool triggerbot_enabled = true;
};
