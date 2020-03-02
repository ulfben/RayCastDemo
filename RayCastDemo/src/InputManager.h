#pragma once
#include "SDLSystem.h"
#include "Keys.h"
#include <array>
#include <algorithm>
enum class MouseButton : size_t {
	LEFT = SDL_BUTTON_LEFT,	   //1
	MIDDLE = SDL_BUTTON_MIDDLE,//2
	RIGHT = SDL_BUTTON_RIGHT,  //3
	X1 =  SDL_BUTTON_X1,       //4
	X2 = SDL_BUTTON_X2         //5
};
struct MouseData {
	int x = 0;
	int y = 0;
	int xrel = 0;
	int yrel = 0;
	int scrollX = 0; //horizontal scroll, positive to the right, negative to the left
	int scrollY = 0;  //vertical scroll, positive up, negative down 	
	void reset() noexcept { scrollX = 0; scrollY = 0; }
};

class InputManager {
	InputManager(const InputManager&) = delete;	
	InputManager& operator=(InputManager&) = delete;	
	void onWindowEvent(const SDL_WindowEvent& e) noexcept;	
	void onMouseMove(const SDL_MouseMotionEvent& e) noexcept;
	void onMouseButtonUp(const SDL_MouseButtonEvent& e) noexcept;
	void onMouseButtonDown(const SDL_MouseButtonEvent& e) noexcept;
	void onMouseWheel(const SDL_MouseWheelEvent& e) noexcept;
	const Uint8* _keyStates = nullptr;	
	MouseData _mouse{};
	std::array<bool, 6> _buttonStates{};
	bool _wantExit = false;
	bool _wantPause = false;				

public:
	InputManager();
	~InputManager();
	void update() noexcept;
	bool isKeyDown(const SDL_Scancode& key) const noexcept;
	bool isButtonDown(MouseButton b) const noexcept;
	bool quitRequested() const noexcept;
	bool pauseRequested() const noexcept;
	void setRelativeMouseMode(bool on) const noexcept;
	int mouseX() const noexcept;
	int mouseY() const noexcept;
	int scrollX() const noexcept;
	int scrollY() const noexcept;

	//TODO: with C++20 this should take a std::span and not be templated... 
	template<typename Container>
	bool isAnyKeyDown(const Container& keys) const noexcept {
		for (const auto& scanCode : keys) {
			if (_keyStates[scanCode] == 1) { return true; }
		}
		return false;
	}
};