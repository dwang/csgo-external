#pragma once

#include "../utils/singleton.hpp"

class menu : public singleton<menu>
{
public:
	void update();
	void show_console_cursor(bool flag);
};
