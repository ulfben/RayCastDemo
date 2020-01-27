#pragma once
#include "SDLSystem.h"
#include <array>
enum class Button : size_t {
	LEFT = SDL_BUTTON_LEFT,	   //1
	MIDDLE = SDL_BUTTON_MIDDLE,//2
	RIGHT = SDL_BUTTON_RIGHT,  //3
	X1 =  SDL_BUTTON_X1,       //4
	X2 = SDL_BUTTON_X2         //5
};
class InputManager {
	InputManager(const InputManager&) = delete;	
	InputManager& operator=(InputManager&) = delete;

	void onWindowEvent(const SDL_WindowEvent& e);	
	void onMouseMove(const SDL_MouseMotionEvent& e);
	void onMouseButtonUp(const SDL_MouseButtonEvent& e);
	void onMouseButtonDown(const SDL_MouseButtonEvent& e);
	void onMouseWheel(const SDL_MouseWheelEvent& e);
	const Uint8* _keyStates;	
	std::array<bool, 6> _buttonStates{};
	bool _wantExit;
	bool _wantPause;
	SDL_Point _mousePosition;
public:
	InputManager();
	~InputManager();
	void update();
	bool isKeyDown(const SDL_Scancode& key) const;
	bool isButtonDown(Button b) const;
	bool quitRequested() const;
	bool pauseRequested() const;
	void setRelativeMouseMode(bool on) const;
	int mouseX() const;
	int mouseY() const;	
};