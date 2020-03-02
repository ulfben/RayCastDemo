# RayCastDemo
A port of the ray casting chapter (chap 6) of Game Programming Gurus (1994), by André LaMothe. 

### Revision 0: 
- initalize SDL2 (window, renderer and inputs)
- copy-paste the original Ray.cpp
- write a facade to translate between the original dependencies and SDL: 
  - Eg: _moveto, _lineto, _setcolor, _drawrect, getch, clear
  - ignoring: _settextposition and drawing text on screen (let's keep it simple)
- delete dead code (Timer), replace all far*-types and _fmalloc-calls
- replace original level loading- and parsing with an inline constexpr array
- rename original "main" to "run"
- add SDL_RenderPresent() & SDL_Delay() to the main loop
- Fix OG bug: move map drawing into the main loop so it's not lost on updates.

Bugs: 
+ the movement code is buggy (can't move)
+ using _getch means you have to focus the console window to control the app, but otherwise we're in business. 

It compiles, runs and renders. Time to start refactoring!

### Revision 1: 
- Make Ray a class, use the constructor to accept the renderer and input manager. 
- comment out 143 lines of movement code (let's keep it simple)
- replace all #defines' with typed- and scoped constexpr
- (manually) translate original color palette indexes to RGB values
- name all literals
- simplify and cache repeated math expressions where appropriate. Eg:

```cpp
//original code
if (block==0)
    {

    _setcolor(15);
    _rectangle(_GBORDER,column*CELL_X_SIZE/4,row*CELL_Y_SIZE/4,
                column*CELL_X_SIZE/4+CELL_X_SIZE/4-1,row*CELL_Y_SIZE/4+CELL_Y_SIZE/4-1);

    }
else
    {

    _setcolor(2);
    _rectangle(_GFILLINTERIOR,column*CELL_X_SIZE/4,row*CELL_Y_SIZE/4,
                column*CELL_X_SIZE/4+CELL_X_SIZE/4-1,row*CELL_Y_SIZE/4+CELL_Y_SIZE/4-1);

    }
```

```cpp
//refactored code
if (block == 0){
    _setcolor(White);
    _rectangle(RectStyle::OUTLINE, left, top, right, bottom);
}else{
    _setcolor(DarkGreen);
    _rectangle(RectStyle::FILL, left, top, right, bottom);
}
```

### Revision 2: 
- create a translation table for palette index -> rgba value. (allows me to use the original code without manually memorizing which number is what color)
- set up game loop for continual rendering instead of only drawing when there's keyboard input
- clean up the movement code:
  - move it to a function to give it a name: "updateViewPoint"
  - replace magical literals, simplify expressions, etc. as per above.
  - create a private ViewPoint type, make it a member and replace the local x/z/angle with this member. (enables further refactorings, as we can now break the 143 line function into constituent parts)

Bugs:
+ There's a crashing bug with the y-intersection going out of range, causing us to read out-of-bounds on the map and crashing. Temporary clamp the y-intersection value and come back later to debug the math.

### Revision 3: 
- clean the RayCaster
  - replace magical literals, simplify expressions, etc. as per above. 
  - unhoist 31 stack variables - place them near first use, and update their type appropriately. (64bit OS, don't need to use long...)
- separate movement code and collision code in "updateViewPoint" by moving the collision stuff to "checkCollisions"
- separate movement code and view port-clearing code in "updateViewPoint", by moving the render calls to "clearViewPort". 
  - also: give names to all coordinates used for drawing the viewport.
- simplify checkCollision with a "bool isWall(x, y)"-helper function
- add utily "clamp" to get rid of repeated if/else statements

### Revision 4: 
- fix all compiler warnings
- fix all static analysis warnings
- remove my ColorRegister-enum. If I'm changing the original function signatures to accept an enum value, I might as well have them take the SDL_Color directly.
- fix some spelling misstakes
- added support to conveniently check multiple keys for a single action "InputManager::isAnyKeyDown(Keys)"
- added mouse interaction (teleport player by clicking on the map). Mainly done to help me understand the coordinate system.
- begin digging out currently implicit values and give them names: resolution, FOV, ray count, map scale etc

### Revision 5: 
- activated hardware rendering and vsync with SDL2
- made 2D map rendering a compile-time setting
- simplified the drawing API (removed _moveto), which enabled me to const all methods
- reduced the size of the lookup tables by ~23% (13,447kB -> 10,405kB @ 320x240px 60° FoV) :
    - inverse cos and sin now share a table (realizing that sin(x) == cos(x+90))
    - the cos lookup table now only cover FoV angles instead of the full 360 degrees 
- removed multiple redundant constants and a bunch of calculations by commiting to some limitations
    - a cell is always square, and a power-of-2
    - the world is always square, and a power-of-2
- replaced a lot of division with shifts
- split ray_caster into separate functions for horizontal and vertical walls, which massively clarifies their logic
- fixed the off-by-1 error with the raycaster drawing on the (bottom and right) viewport boundaries.
- split the code into separate files as a first step in creating a cross-platform facade
    - Config (settings and constants)
    - Graphics (drawing routines), 
    - LevelData (map layout and isWall(x,y))
    - ViewPoint (player position, view angle, movement code, collision checking)
    - MiniMap
   
### Revision 6:
- The coordinate system is no longer reversed in-world vs. on screen. This let me stop inverting axis in Ray_Caster, MiniMap::render() etc. 
- cleaned up the mixed code styles (esp. function and method names)
- re-organized the file hierarchy (visual studio added all new files to top-level directory)

### TODO:
- convert globals to arguments where possible
- prepare for port to Arduboy 
  - provide a proper facade for the RayCaster to use for rendering - based on SDL2 or Arduboy2 or whatever else one might want to run on.
  - provide another facade for the input management 
  
### Bugs: 
- KeyMap can no longer be constexpr constructed.
- Some combinations of viewport width & FOV will result in 1 pixel gaps being rendered when facing up (90), down (270) or right (360).
  - need to figure out what the relationship between these values are.