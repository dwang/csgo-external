#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace detail
{
	template <typename T, typename Enable = void>
	struct recursive_decay
	{
		using type = std::decay_t<T>;
	};

	template <typename T>
	struct recursive_decay<T, std::enable_if_t<std::is_pointer_v<T>>>
	{
		using type = std::add_pointer_t<typename recursive_decay<std::remove_pointer_t<T>>::type>;
	};

	template <typename T>
	using recursive_decay_t = typename recursive_decay<T>::type;
}

template <typename T>
static auto offset_ptr(T ptr, const std::ptrdiff_t offset) -> detail::recursive_decay_t<T>
{
	auto address = (std::uintptr_t)(ptr) + offset;
	return (detail::recursive_decay_t<T>)(address);
}

template <typename T, typename Old, typename New>
static auto rebase_ptr(T ptr, Old old_base, New new_base) -> detail::recursive_decay_t<T>
{
	return (detail::recursive_decay_t<T>)(
		(std::uintptr_t)(ptr)
		- (std::uintptr_t)(old_base)
		+ (std::uintptr_t)(new_base));
}
