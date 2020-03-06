#pragma once
#include "SDLex.h"
#include "Utils.h"
#include "Keys.h"
#include <string_view>
namespace Cfg {	
	using KeyMap = Keys<3>;
	using namespace std::literals::string_view_literals;	
	static constexpr std::string_view TITLE = "Ray Caster Demo (5th iteration)"sv;
	static constexpr bool RENDER_MINIMAP = true;
	static constexpr auto MAP_SCALE_FACTOR = 2; //how many left shifts to perform (eg. 4 times smaller than the actual world)
	static constexpr int WIN_WIDTH = 640;
	static constexpr int WIN_HEIGHT = 480;	
	static constexpr auto VIEWPORT_WIDTH = 128;
	static constexpr auto VIEWPORT_HEIGHT = 64;
	static constexpr auto VIEWPORT_LEFT = 0;
	static constexpr auto VIEWPORT_TOP = 0;
	static constexpr auto CELL_SIZE = 64; //width and height of a cell in the game world, must be a power of 2.   	
	static constexpr auto FOV_DEGREES = 60; //Field of View, in degrees. We'll need to break these into RAY_COUNT sub-angles and cast a ray for each angle. We'll be using a lookup table for that        	
	static const KeyMap rotateRight{ SDL_SCANCODE_KP_6, SDL_SCANCODE_RIGHT, SDL_SCANCODE_D };
	static const KeyMap rotateLeft{ SDL_SCANCODE_KP_4, SDL_SCANCODE_LEFT, SDL_SCANCODE_A };
	static const KeyMap moveForward{ SDL_SCANCODE_KP_8, SDL_SCANCODE_UP, SDL_SCANCODE_W };
	static const KeyMap moveBackward{ SDL_SCANCODE_KP_2, SDL_SCANCODE_DOWN, SDL_SCANCODE_S };
	static constexpr auto START_POS_X = 1;
	static constexpr auto START_POS_Y = 7;
	static constexpr auto WALK_SPEED = 8;
	static constexpr auto ROTATION_SPEED = 16;		
	static constexpr auto TABLE_SIZE = static_cast<int>(VIEWPORT_WIDTH* (360.0f / FOV_DEGREES)); //how many elements we need to store the slope of every possible ray that can be projected.
	//compile time feature-flags
	constexpr bool hasMinimap() noexcept { return RENDER_MINIMAP; }

	static_assert(Utils::isPowerOfTwo(CELL_SIZE) && "Cell width and height must be a power-of-2");
};

//global constants derived from the config above:
static constexpr auto RAY_COUNT = Cfg::VIEWPORT_WIDTH; //one ray per column of screen space (horizontal resolution)        
static constexpr auto TWO_PI = 2.0f * 3.141592654f;
static constexpr auto ANGLE_360 = Cfg::TABLE_SIZE; //East (and total number of possible angles in a full rotation)
static constexpr auto ANGLE_90 = ANGLE_360 / 4; //South
static constexpr auto ANGLE_180 = ANGLE_360 / 2; //West
static constexpr auto ANGLE_270 = ANGLE_360 - ANGLE_90; //North
static constexpr auto ANGLE_0 = 0;  //back to East
static constexpr auto ANGLE_TO_RADIANS = (TWO_PI / ANGLE_360);
static constexpr auto HALF_FOV_ANGLE = Cfg::VIEWPORT_WIDTH / 2; // FoV/2 in angles (for table lookup) instead of degrees.    

static constexpr auto CELL_SIZE = Cfg::CELL_SIZE; //width and height of a cell in the game world, must be a power of 2.   
static constexpr auto OVERBOARD = CELL_SIZE / 2; // the absolute closest a player can get to a wall  
static constexpr auto CELL_SIZE_FP = Utils::log2(CELL_SIZE); // log base 2 of 64 (used for quick division)    

static constexpr auto VIEWPORT_LEFT = Cfg::VIEWPORT_LEFT;
static constexpr auto VIEWPORT_TOP = Cfg::VIEWPORT_TOP;
static constexpr auto VIEWPORT_RIGHT = Cfg::VIEWPORT_LEFT + Cfg::VIEWPORT_WIDTH;
static constexpr auto VIEWPORT_BOTTOM = Cfg::VIEWPORT_TOP + Cfg::VIEWPORT_HEIGHT;
static constexpr auto VIEWPORT_HORIZON = Cfg::VIEWPORT_TOP + (Cfg::VIEWPORT_HEIGHT >> 1);
