#include "InputManager.h"
#include <stdexcept>
#include "SDLSystem.h"
#include "Utils.h"
InputManager::InputManager():
		_keyStates(SDL_GetKeyboardState(nullptr)),
		_wantExit(false), 
		_wantPause(false),
		_mousePosition{ 0,0 }
{
	if (!_keyStates) {
		throw std::runtime_error(SDL_GetError());
	}
}
InputManager::~InputManager() {
	_keyStates = nullptr;
}
void InputManager::update() {	
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {		
		case SDL_MOUSEMOTION:
			onMouseMove(e.motion);
			break;
		case SDL_MOUSEWHEEL:
			onMouseWheel(e.wheel);
			break;
		case SDL_MOUSEBUTTONUP:
			onMouseButtonUp(e.button);
			break;
		case SDL_MOUSEBUTTONDOWN:
			onMouseButtonDown(e.button);
			break;
		case SDL_KEYUP:
			//onKeyUp(e.key);
			break;
		case SDL_KEYDOWN:
			//onKeyDown(e.key);
			break;
		case SDL_QUIT:
			_wantExit = true;
			break;
		case SDL_WINDOWEVENT:		
			onWindowEvent(e.window);
			break;
		default:
			break;
		}	
	}
}
void InputManager::setRelativeMouseMode(bool on) const {
	int res = SDL_SetRelativeMouseMode(SDLex::fromBool(on));
	SDL_assert(res == 0);
}

bool InputManager::isKeyDown(const SDL_Scancode& key) const {
	return (_keyStates[key] == 1);
}
bool InputManager::isButtonDown(Button b) const {
	const auto index = Utils::to_underlying(b);
	return _buttonStates[index] == true;
}
int InputManager::mouseX() const {
	return _mousePosition.x;
}
int InputManager::mouseY() const {
	return _mousePosition.y;
}
bool InputManager::quitRequested() const {
	return _wantExit || isKeyDown(SDL_SCANCODE_ESCAPE);
}
bool InputManager::pauseRequested() const {
	return _wantPause;
}

void InputManager::onWindowEvent(const SDL_WindowEvent& e) {
	if (e.event == SDL_WINDOWEVENT_CLOSE) {
		_wantExit = true;
		return; 
	}
	if (e.event == SDL_WINDOWEVENT_FOCUS_LOST) {
		_wantPause = true;				
	}
	else if (e.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
		_wantPause = false;				
	}		
	//broadCastEvent<WindowEvent>(e, this);
}
void InputManager::onMouseWheel([[maybe_unused]]const SDL_MouseWheelEvent& e) {		
}
void InputManager::onMouseMove(const SDL_MouseMotionEvent& e) {	
	_mousePosition.x = e.x;
	_mousePosition.y = e.y;
}
void InputManager::onMouseButtonUp(const SDL_MouseButtonEvent& e) {	
	_buttonStates[e.button] = false;
}
void InputManager::onMouseButtonDown(const SDL_MouseButtonEvent& e) {	
	_buttonStates[e.button] = true;
}