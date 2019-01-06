#pragma once
#include <cstdint>
#include <type_traits>

// Implements FNV-1a hash algorithm
namespace detail
{
	template <typename Type, Type OffsetBasis, Type Prime>
	struct SizeDependantData
	{
		using type = Type;
		constexpr static auto k_offset_basis = OffsetBasis;
		constexpr static auto k_prime = Prime;
	};

	template <size_t Bits>
	struct SizeSelector;

	template <>
	struct SizeSelector<32>
	{
		using type = SizeDependantData<uint32_t, 0x811c9dc5ul, 16777619ul>;
	};

	template <>
	struct SizeSelector<64>
	{
		using type = SizeDependantData<uint64_t, 0xcbf29ce484222325ull, 1099511628211ull>;
	};

	template <size_t Size>
	class FnvHash
	{
	private:
		using data_t = typename SizeSelector<Size>::type;

	public:
		using hash = typename data_t::type;

	private:
		constexpr static auto k_offset_basis = data_t::k_offset_basis;
		constexpr static auto k_prime = data_t::k_prime;

	public:
		template <size_t N>
		static __forceinline constexpr auto hash_constexpr(const char(&str)[N], const size_t size = N) -> hash
		{
			return static_cast<hash>(1ull * (size == 1
				? (k_offset_basis ^ str[0])
				: (hash_constexpr(str, size - 1) ^ str[size - 1])) * k_prime);
		}

		static auto __forceinline hash_runtime_data(const void* data, const std::size_t sz) -> hash
		{
			const auto bytes = static_cast<const uint8_t*>(data);
			const auto end = data + sz;
			auto result = k_offset_basis;
			for (auto it = bytes; it < end; ++it)
			{
				result ^= *it;
				result *= k_prime;
			}

			return result;
		}

		static auto __forceinline hash_runtime(const char* str) -> hash
		{
			auto result = k_offset_basis;
			do
			{
				result ^= *str++;
				result *= k_prime;
			} while (*(str - 1) != '\0');

			return result;
		}
	};
}

using fnv = ::detail::FnvHash<sizeof(void*) * 8>;

#define FNV(str) (std::integral_constant<fnv::hash, fnv::hash_constexpr(str)>::value)
