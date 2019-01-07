#pragma once
#include "fnv.hpp"
#include <utility>

namespace remote
{
	auto attach_process(const fnv::hash name) -> void;
	auto detach_process() -> void;
	auto raw_read(const std::uintptr_t dest, void* src, const std::size_t size) -> void;
	auto raw_write(const std::uintptr_t dest, const void* src, const std::size_t size) -> void;
	auto read_data(void* dest, void* src, std::size_t size) -> void;
	auto write_data(void* dest, void* src, std::size_t size) -> void;
	auto find_module_by_name(const fnv::hash name) -> std::pair<std::uintptr_t, std::size_t>;
	auto find_pattern(const std::pair<std::uintptr_t, std::size_t> mod, const char* pattern) -> std::uintptr_t;

	template <typename T>
	auto read(const std::uintptr_t src) -> T
	{
		T v;
		raw_read(src, &v, sizeof(v));
		return v;
	}

	template <typename T>
	auto write(const std::uintptr_t dst, const T& v) -> void
	{
		raw_write(dst, &v, sizeof(T));
	}

	template <typename T>
	auto read(void* src) -> T
	{
		T v;
		read_data(&v, src, sizeof(v));
		return v;
	}

	template <typename T>
	auto write(void* dst, const T& v) -> void
	{
		write_data(dst, &v, sizeof(T));
	}

	template <typename T>
	class variable
	{
	private:
		using my_type = variable<T>;

		variable() = delete;
		variable(const my_type&) = delete;

	public:
		auto get() -> T { return read<T>(&remote_value); }
		auto get_raw() -> T* { return &remote_value; }
		auto set(const T& v) -> void { write(&remote_value, v); }

		operator T() { return get(); }
		auto operator =(const T& v) -> my_type& { set(v); return *this; }

	private:
		T remote_value;
	};
}

