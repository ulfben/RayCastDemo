#pragma once
#include <string_view>
#include "SDLex.h"
struct SDL_Window;
class Window {	
	SDLex::WindowPtr _ptr;
	Window(const Window&) = delete; //disable copy constructor
	Window& operator=(Window&) = delete; //disable copy assignment

public:
	Window(const std::string_view title, int width = 1280, int height = 720);
	void setTitle(const std::string_view title) const noexcept;
	SDL_Window* getRawPtr() const noexcept;
	void setWindowGrab(bool on) const noexcept;
	bool getWindowGrab() const noexcept;
	void hideCursor() const noexcept;
	void showCursor() const noexcept;
};