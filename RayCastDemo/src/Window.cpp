#include "Window.h"
#include "SDLSystem.h"
#include <stdexcept>
Window::Window(const std::string_view title, int width, int height):
	_ptr(SDL_CreateWindow(title.data(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL)){
	if (!_ptr) {
		throw SDLError();
	}
}
void Window::setTitle(const std::string_view title) const noexcept {
	SDL_SetWindowTitle(_ptr.get(), title.data());
}

SDL_Window* Window::getRawPtr() const noexcept {
	SDL_assert(_ptr != nullptr && "Window: invalid pointer. Use after delete?");
	return _ptr.get();
}
bool Window::getWindowGrab() const noexcept {
	return SDLex::toBool(SDL_GetWindowGrab(_ptr.get()));
}
void Window::setWindowGrab(bool on) const noexcept {
	SDL_SetWindowGrab(_ptr.get(), SDLex::fromBool(on));
}
void Window::hideCursor() const noexcept {
	SDL_ShowCursor(SDL_DISABLE);
}
void Window::showCursor() const noexcept {
	SDL_ShowCursor(SDL_ENABLE);
}