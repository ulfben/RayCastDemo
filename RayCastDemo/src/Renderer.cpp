#include "Renderer.h"
#include "SDLSystem.h"
#include "Window.h"
#include <stdexcept>
Renderer::Renderer(const Window& w): 
	_ptr{ SDL_CreateRenderer(w.getRawPtr(), -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC)}{
	if (!_ptr) {		
		throw SDLError();
	}
}
SDL_Renderer* Renderer::getRawPtr() const noexcept {
	SDL_assert(_ptr != nullptr && "Renderer: invalid pointer. Use after delete?");
	return _ptr.get();
}
void Renderer::setLogicalSize(int w, int h) const noexcept {
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(getRawPtr(), w, h);
}

void Renderer::clear() const noexcept {
	int res = SDL_RenderClear(_ptr.get());
	SDL_assert(res == 0);
}
void Renderer::setColor(const SDL_Color& c) const noexcept {
	int res = SDL_SetRenderDrawColor(_ptr.get(), c.r, c.g, c.b, c.a);
	SDL_assert(res == 0);
}
void Renderer::present() const noexcept {
	SDL_RenderPresent(_ptr.get());
}
void Renderer::drawLine(int x1, int y1, int x2, int y2) const noexcept {
	int res = SDL_RenderDrawLine(_ptr.get(), x1, y1, x2, y2);
	SDL_assert(res == 0);
}
void Renderer::drawPoint(int x, int y) const  noexcept {
	int res = SDL_RenderDrawPoint(_ptr.get(), x, y);
	SDL_assert(res == 0);
}
void Renderer::drawRect(const SDL_Rect& rect) const noexcept {
	int res = SDL_RenderDrawRect(_ptr.get(), &rect);
	SDL_assert(res == 0);
}
void Renderer::drawFilledRect(const SDL_Rect& rect) const noexcept {
	int res = SDL_RenderFillRect(_ptr.get(), &rect);
	SDL_assert(res == 0);
}
bool Renderer::isClipEnabled() const noexcept {
	return SDL_RenderIsClipEnabled(_ptr.get()) == SDL_TRUE;
}