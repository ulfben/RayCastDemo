#pragma once
#include "SDLex.h"
struct SDL_Renderer;
struct SDL_Color;
struct SDL_Rect;
class Window;
class Renderer {		
	SDLex::RendererPtr _ptr;
	Renderer(const Renderer&) = delete; //disable copy constructor
	Renderer& operator=(Renderer&) = delete; //disable copy assignment
public:
	Renderer(const Window& w);	
	void clear() const noexcept;
	inline SDL_Renderer* getRawPtr() const noexcept;
	void setLogicalSize(int w, int h) const noexcept;
	void setColor(const SDL_Color& c) const noexcept;
	void present() const noexcept;
	void drawLine(int x1, int y1, int x2, int y2) const noexcept;
	void drawPoint(int x, int y) const noexcept;
	void drawRect(const SDL_Rect& r) const noexcept;
	void drawFilledRect(const SDL_Rect& rect) const noexcept;
	bool isClipEnabled() const noexcept; 
};