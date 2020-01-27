#pragma once
#include <type_traits>
namespace Utils {
	using namespace std::literals::string_literals;
	template <typename Enum>
	static constexpr typename std::underlying_type<Enum>::type to_underlying(Enum e) noexcept {
		return static_cast<typename std::underlying_type<Enum>::type>(e);
	}
	template <typename T>
	static constexpr T wrap(T val, T min, T max) noexcept {
		return (val > max) ? min : (val < min) ? max : val;
	}
	template <typename T>
	static constexpr T clamp(T val, T min, T max) noexcept {
		return (val > max) ? max : (val < min) ? min : val;
	}	
}