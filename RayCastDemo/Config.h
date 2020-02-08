#pragma once
#include <string_view>
namespace Config {
	using namespace std::literals::string_view_literals;
	constexpr std::string_view TITLE = "Ray Caster Demo (4th iteration)"sv;
	constexpr int WIN_WIDTH = 640;
	constexpr int WIN_HEIGHT = 480;
	
	//compile time feature-flags
	constexpr bool hasMinimap() noexcept { return true; } 
};