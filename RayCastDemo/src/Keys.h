#pragma once
#include "SDLex.h"
#include <algorithm>
template<size_t N = 3>
class Keys { //map multiple keys to single action. (eg. w + up + 8 for "walk forward")
	SDL_Scancode _keys[N];
	size_t _size = 0;
	using const_iter = SDL_Scancode const* const;
public:
	constexpr Keys(const std::initializer_list<SDL_Scancode> list) {
		SDL_assert(list.size() <= N && "Mapping too many to an action.");
		std::copy(list.begin(), list.end(), _keys);		
		_size = N;
	}
	inline constexpr const_iter end() const noexcept {
		return &_keys[_size];
	}
	inline constexpr const_iter begin() const noexcept {
		return &_keys[0];
	}
};