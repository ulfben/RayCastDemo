#define SDL_MAIN_HANDLED
#include <iostream>
#include <string_view>
#include "src/Config.h"
#include "src/ViewPoint.h"
#include "src/MiniMap.h"
#include "src/SDLSystem.h"
#include "src/Renderer.h"
#include "src/Window.h"
#include "src/InputManager.h"
#include "src/RayCaster.h"

int main([[maybe_unused]]int argc, [[maybe_unused]] char* argv[]){
	assert(testIsWallLookup());
	try {		
		SDLSystem _sdl;
		Window _window{ Cfg::TITLE, Cfg::WIN_WIDTH, Cfg::WIN_HEIGHT };		
		Renderer _r{ _window };
		Graphics _g(_r);
		InputManager _input{};				
		RayCaster ray{};		
		ViewPoint _viewPoint{ Cfg::START_POS_X, Cfg::START_POS_Y, ANGLE_0 };
		//ray.prettyPrintLUTs();
		while (!_input.quitRequested()) {
			_input.update();						
			_viewPoint.update(_input);
			_viewPoint.checkCollisions();
			_g.clearScreen();			
			if constexpr (Cfg::hasMinimap()) { 
				MiniMap::renderMap(_g);
			}
			ray.renderView(_g, _viewPoint.x, _viewPoint.y, _viewPoint.angle);
			_g.present();
		}
		return 0;		
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
