#pragma once
#include <type_traits>
namespace Utils {
	using namespace std::literals::string_literals;
	template <typename Enum>
	static constexpr typename std::underlying_type<Enum>::type to_underlying(Enum e) noexcept {
		return static_cast<typename std::underlying_type<Enum>::type>(e);
	}

	template <typename T>
	static constexpr T clamp(T val, T min, T max) noexcept {
		return (val > max) ? max : (val < min) ? min : val;
	}	
	
	template <typename Real>
	constexpr int32_t ceil(Real num){
		return (static_cast<Real>(static_cast<int32_t>(num)) == num)
			? static_cast<int32_t>(num)
			: static_cast<int32_t>(num) + ((num > 0) ? 1 : 0);
	}

	template<typename T>
	constexpr inline bool isInRectangle(T x, T y, T left, T top, T right, T bottom) noexcept {
		return (x > left && x < right) && (y > top && y < bottom);
	}

}