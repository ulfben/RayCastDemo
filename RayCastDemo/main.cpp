// RayCastDemo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define SDL_MAIN_HANDLED
#include <memory>
#include <iostream>
#include <string_view>
#include "src/SDLex.h"
#include "src/Ray.h"


int main([[maybe_unused]]int argc, [[maybe_unused]] char* argv[]){
	try {				
		run();
		return EXIT_SUCCESS;
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
