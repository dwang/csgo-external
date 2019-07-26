#include "visuals.hpp"
#include "../utils/remote.hpp"
#include "../sdk/engine.hpp"
#include "../utils/offsets.hpp"
#include <thread>

auto visuals::glow() -> void
{
	auto set_glow_clr = [](glow_object_t* object, color color, int glow_style, float bloom = 1.0f)
	{
		object->m_rwo = true;
		object->m_rwuo = false;
		object->m_color = color;
		object->m_bloom_amt = bloom;
		object->m_full_bloom = false;
		object->m_glow_style = glow_style;
	};

	static std::uintptr_t obj_manager = remote::read<std::uintptr_t>(offsets::get().client_dll.first + offsets::get().dwGlowObjectManager);

	for (auto entity : entities)
	{
		glow_object_t object = remote::read<glow_object_t>(obj_manager + entity.get_glow_index() * 0x38);

		if (!object.m_ent || object.m_ent == local.get_pointer() || !entity.is_valid())
			continue;

		set_glow_clr(&object, entity.get_team() == local.get_team() ? color(0.17f, 0.67f, 0.8f, 0.7f) : color(1.0f, 0.35f, 0.70f, 0.7f), chams_enabled ? 1 : 0);
		remote::write<glow_object_t>(obj_manager + entity.get_glow_index() * 0x38, object);
	}
}

auto visuals::set_glow_update(bool update) -> void
{
	remote::write_protected<unsigned char>(offsets::get().client_dll.first + offsets::get().dwGlowUpdate, update ? 0x74 : 0xEB);
}

auto visuals::set_model_brightness(float brightness) -> void
{
	remote::write<int>(offsets::get().engine_dll.first + offsets::get().dwModelAmbientMin, *(int*)& brightness ^ (offsets::get().engine_dll.first + offsets::get().dwModelAmbientMin - 0x2c));
}

auto visuals::set_tonemap_scale(float amount) -> void
{
	//remote::write<int>(offsets::get().engine_dll.first + 0x58af1c, *(int*)&amount ^ (offsets::get().engine_dll.first + 0x58af1c - 0x2c));
}

auto visuals::chams() -> void
{
	for (auto entity : entities)
	{
		if (entity.get_pointer() == local.get_pointer() || !entity.is_valid())
			continue;

		remote::write<model_color>(entity.get_pointer() + 0x70, entity.get_team() == local.get_team() ? model_color(43, 92, 204) : model_color(255, 43, 94));
	}
}
