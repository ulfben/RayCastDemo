#include "InputManager.h"
#include <stdexcept>
#include "SDLSystem.h"
#include "Utils.h"

InputManager::InputManager():
		_keyStates(SDL_GetKeyboardState(nullptr)){
	if (!_keyStates) {
		throw std::runtime_error(SDL_GetError());
	}
}
InputManager::~InputManager() {
	_keyStates = nullptr;
}
void InputManager::update() noexcept {
	_mouse.reset();
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
void InputManager::setRelativeMouseMode(bool on) const noexcept {
	int res = SDL_SetRelativeMouseMode(SDLex::fromBool(on));
	SDL_assert(res == 0);
}

bool InputManager::isKeyDown(const SDL_Scancode& scanCode) const noexcept {
	return (_keyStates[scanCode] == 1);
}
bool InputManager::isButtonDown(MouseButton b) const noexcept {
	const auto index = Utils::to_underlying(b);
	return _buttonStates[index] == true;
}
int InputManager::mouseX() const noexcept {
	return _mouse.x;
}
int InputManager::mouseY() const noexcept {
	return _mouse.y;
}
int InputManager::scrollX() const noexcept{
	return _mouse.scrollX;
}
int InputManager::scrollY() const noexcept{
	return _mouse.scrollX;
}
bool InputManager::quitRequested() const noexcept {
	return _wantExit || isKeyDown(SDL_SCANCODE_ESCAPE);
}
bool InputManager::pauseRequested() const noexcept {
	return _wantPause;
}

void InputManager::onWindowEvent(const SDL_WindowEvent& e) noexcept {
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
void InputManager::onMouseWheel(const SDL_MouseWheelEvent& e) noexcept {
	_mouse.scrollX = e.x; 
	_mouse.scrollY = e.y;
}
void InputManager::onMouseMove(const SDL_MouseMotionEvent& e) noexcept {
	_mouse.x = e.x;
	_mouse.y = e.y;
	_mouse.xrel = e.xrel;
	_mouse.yrel = e.yrel;
}
void InputManager::onMouseButtonUp(const SDL_MouseButtonEvent& e) noexcept {
	_buttonStates[e.button] = false;
}
void InputManager::onMouseButtonDown(const SDL_MouseButtonEvent& e) noexcept {
	_buttonStates[e.button] = true;
}