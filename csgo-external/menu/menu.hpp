#pragma once

#include "../utils/singleton.hpp"
#include "../render/renderer.hpp"
#include <vector>
#include <string>


struct item
{
	std::string name;
	bool* container;
};

class menu : public singleton<menu>
{
public:
	bool opened = true;
	int selected;
	std::vector<item> items;

	auto initialize() -> void;
	auto update() -> void;
	auto render() -> void;
};
