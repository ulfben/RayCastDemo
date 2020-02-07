#define SDL_MAIN_HANDLED
#include <iostream>
#include <string_view>
#include "Config.h"
#include "src/SDLSystem.h"
#include "src/Renderer.h"
#include "src/Window.h"
#include "src/InputManager.h"
#include "src/Ray.h"


int main([[maybe_unused]]int argc, [[maybe_unused]] char* argv[]){
	try {
		SDLSystem _sdl;
		Window _window{ Config::TITLE, Config::WIN_WIDTH, Config::WIN_HEIGHT };
		Renderer _r{ _window };	
		InputManager _input{};
		Ray ray{ _r, _input };
		return ray.run();			
	}
	catch (const SDLInitError & e) {
		std::cerr << "SDL initialization error: " << e.what() << std::endl;
	}
	catch (const SDLError & e) {
		std::cerr << "SDL error: " << e.what() << std::endl;
	}
	catch (const std::runtime_error & e) {
		std::cerr << "Runtime error: " << e.what() << std::endl;
	}
	catch (const std::exception & e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Unknown failure occurred. Possible memory corruption" << std::endl;
	}
	return EXIT_FAILURE;
}
