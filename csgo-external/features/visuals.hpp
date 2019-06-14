#pragma once
#include "../utils/singleton.hpp"
#include "../utils/color.hpp"
#include <cstdint>

class visuals : public singleton<visuals>
{
	struct glow_object_t
	{
		std::uintptr_t m_ent;
		color m_color;
		uint8_t	pad_01[8];
		float m_bloom_amt;
		uint8_t	pad_02[4];
		bool m_rwo;
		bool m_rwuo;
		bool m_full_bloom;
		uint8_t pad_03[5];
		int32_t	m_glow_style;
	};
public:
	auto glow() -> void;
	auto chams() -> void;
	auto set_model_brightness(float brightness) -> void;
	auto set_glow_update(bool update) -> void;
	auto set_tonemap_scale(float amount) -> void;
	bool glow_enabled = true;
	bool chams_enabled = true;
};
