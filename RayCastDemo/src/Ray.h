#pragma once
#include <array>
#include <cmath>
#include "Config.h"
#include "Renderer.h"
#include "InputManager.h"
#include "SDLSystem.h"
#include "Utils.h"

class Ray {
    enum class RectStyle {
        OUTLINE,
        FILL
    };
    struct ViewPoint {
        int x = 0, y = 0, angle = 0;
        float dx = 0.0f, dy = 0.0f;        
    };
    struct RayEnd {
        int boundary = 0; // record intersections with cell boundaries        
        int intersection = 0; // used to save exact intersection point 
        float distance = 0.0f; // the distance of the x and y ray intersections from the player
        bool operator <(const RayEnd& that) const noexcept { return distance < that.distance; };
    };
    struct RayBegin {
        float intersection = 0; //the first possible intersection point
        int bound = 0; // the next intersection point   
        int delta = 0; // the amount needed to move to get to the next cell position
        int next_cell = 0; //used to figure out the quadrant of the ray            
    };
    
    //declare SDL_Color values for each color in the original LUT
    static constexpr SDL_Color Black = { 0, 0, 0 };
    static constexpr SDL_Color DarkBlue = { 0, 0, 139 };
    static constexpr SDL_Color DarkGreen = { 0, 139, 0 };
    static constexpr SDL_Color DarkCyan = { 0, 139, 139 };
    static constexpr SDL_Color DarkRed = { 139, 0, 0 };
    static constexpr SDL_Color DarkMagenta = { 139, 0, 139 };
    static constexpr SDL_Color Brown = { 139, 69, 19 };
    static constexpr SDL_Color Gray = { 128, 128, 128 };
    static constexpr SDL_Color DarkGray = { 105, 105, 105 };
    static constexpr SDL_Color LightBlue = { 65, 105, 225 };
    static constexpr SDL_Color LightGreen = { 144, 238, 144 };
    static constexpr SDL_Color LightCyan = { 63, 255, 255 };
    static constexpr SDL_Color LightRed = { 255, 128, 128 };
    static constexpr SDL_Color LightMagenta = { 255, 128, 255 };
    static constexpr SDL_Color Yellow = { 255, 255, 0 };
    static constexpr SDL_Color White = { 255, 255, 255 };
    //indexed palette to translate between original LUT value and SDL_Color
    static constexpr std::array<SDL_Color, 16> Palette{
        Black, DarkBlue, DarkGreen, DarkCyan,       //0, 1, 2, 3
        DarkRed, DarkMagenta, Brown, Gray,          //4, 5, 6, 7
        DarkGray, LightBlue, LightGreen, LightCyan, //8, 9, 10, 11
        LightRed, LightMagenta, Yellow, White       //12, 13, 14, 15 
    };

    const Renderer& _r;
    InputManager& _input;    
    ViewPoint _viewPoint;
    using KeyMap = Keys<3>;
    const KeyMap rotateRight{ SDL_SCANCODE_KP_6, SDL_SCANCODE_RIGHT, SDL_SCANCODE_D };
    const KeyMap rotateLeft{ SDL_SCANCODE_KP_4, SDL_SCANCODE_LEFT, SDL_SCANCODE_A };
    const KeyMap moveForward{ SDL_SCANCODE_KP_8, SDL_SCANCODE_UP, SDL_SCANCODE_W };
    const KeyMap moveBackward{ SDL_SCANCODE_KP_2, SDL_SCANCODE_DOWN, SDL_SCANCODE_S };
    int _x1 = 0;
    int _y1 = 0;
    void _setcolor(int lutIndex) const noexcept {
        SDL_assert(lutIndex > -1 && lutIndex < Palette.size() && "_setColor (int): invalid LUT color index specified, must be 0-15");
        const auto color = Palette[lutIndex];
        _r.setColor(color);
    }
    void _setcolor(const SDL_Color& color) const noexcept {
        _r.setColor(color);
    }
    void _moveto(int x1, int y1) noexcept { //TODO: const correctness
        _x1 = x1;
        _y1 = y1;
    }
    void _lineto(int x2, int y2) const noexcept {
        _r.drawLine(_x1, _y1, x2, y2);
    }
    void _setpixel(int x, int y) const noexcept {
        _r.drawPoint(x, y);
    }
    void _rectangle(RectStyle style, int left, int top, int right, int bottom) const noexcept {
        SDL_Rect rect{ left, top, right - left, bottom - top };
        if (style == RectStyle::FILL) {
            _r.drawFilledRect(rect);
        }
        else {
            _r.drawRect(rect);
        }
    }
     
    static constexpr auto WALL_BOUNDARY_COLOR = White;
    static constexpr auto VERTICAL_WALL_COLOR = LightGreen;
    static constexpr auto HORIZONTAL_WALL_COLOR = DarkGreen;
    static constexpr auto MAP_WIDTH = 256; //target width of the minimap, in pixels. 
    static constexpr auto VIEWPORT_WIDTH = 320; //TODO: Some combinations of viewport width & FOV will result in 1 pixel gaps being rendered when facing up (90), down (270) or right (360).
    static constexpr auto VIEWPORT_HEIGHT = 240;
    static constexpr auto RAY_COUNT = VIEWPORT_WIDTH; //one ray per column of screen space (horizontal resolution)    
    static constexpr auto FOV_DEGREES = 60; //Field of View, in degrees. We'll need to break these into RAY_COUNT sub-angles and cast a ray for each angle. We'll be using a lookup table for that        
    static constexpr auto TABLE_SIZE = static_cast<int>(VIEWPORT_WIDTH * (360.0f / FOV_DEGREES)); //how many elements we need to store the slope of every possible ray that can be projected.
    static constexpr auto ANGLE_360 = TABLE_SIZE; //number of possible angles in a full rotation (right)
    static constexpr auto HALF_FOV_ANGLE = static_cast<int>(ANGLE_360 / (360.0f / FOV_DEGREES)) / 2; //in angles, not degrees. So FOV 60 (degrees) = HALF_FOV 30 (degree) = 160 angles (for lookup tables)
    static constexpr auto START_POS_X = 8;
    static constexpr auto START_POS_Y = 3;
    static constexpr auto WALK_SPEED = 10;
    static constexpr auto ROTATION_SPEED = ANGLE_360 / 100; //arbitrary. 
    static constexpr auto CELL_WIDTH = 64; //size of a cell in the game world
    static constexpr auto CELL_HEIGHT = 64; //must be a power of two
    static constexpr auto CELL_WIDTH_FP = Utils::log2(CELL_WIDTH); // log base 2 of 64 (used for quick division)
    static constexpr auto CELL_HEIGHT_FP = Utils::log2(CELL_HEIGHT);
    //320x240@60fov = K15000, 128x64@60fov = K7000
    static constexpr auto K = 15000.0f;// think of K as a combination of view distance and aspect ratio. Pick a value that looks good. In my case: that makes the block on screen look square. (p.213)
    static constexpr auto ANGLE_270 = static_cast<int>(ANGLE_360 * 0.75f); //down
    static constexpr auto ANGLE_180 = ANGLE_360 / 2; //left
    static constexpr auto ANGLE_90 = ANGLE_360 / 4; //up
    static constexpr auto ANGLE_45 = ANGLE_360 / 8; //up and right
    static constexpr auto ANGLE_0 = 0;  //also right (same as 360)
    static constexpr auto VIEWPORT_LEFT = MAP_WIDTH + (((Config::WIN_WIDTH-MAP_WIDTH)/2)- (VIEWPORT_WIDTH/2)); //center between map and screen edge
    static constexpr auto VIEWPORT_TOP = Config::WIN_HEIGHT / 2 - VIEWPORT_HEIGHT / 2;
    static constexpr auto VIEWPORT_RIGHT = VIEWPORT_LEFT + VIEWPORT_WIDTH;
    static constexpr auto VIEWPORT_BOTTOM = VIEWPORT_TOP + VIEWPORT_HEIGHT;
    static constexpr auto VIEWPORT_HORIZON = VIEWPORT_TOP + (VIEWPORT_HEIGHT / 2);    
    static constexpr auto OVERBOARD = (CELL_WIDTH+CELL_HEIGHT)/4; // the absolute closest a player can get to a wall  
    static constexpr auto TWO_PI = 2.0f * 3.141592654f;
    static constexpr auto WORLD_ROWS = 16;
    static constexpr auto WORLD_COLUMNS = 16;
    static constexpr auto FIRST_VALID_CELL = 1; //to shortcut collision testing we can check against the map boundary walls. Helps with mouse interaction and for Q&D respawn when stuck in a wall.
    static constexpr auto LAST_VALID_CELL = WORLD_ROWS-2; //0 == wall, 15 == wall. so if our position is below or above the valid spaces, we can bail without performing lookup. TODO: this should be part of the map data. 
    static constexpr auto WORLD_WIDTH = (WORLD_COLUMNS * CELL_WIDTH);
    static constexpr auto WORLD_HEIGHT = (WORLD_ROWS * CELL_HEIGHT); 
    //Originally: 0xFFC0 (65472), which is 0xFFFF-CELL_WIDTH. The constant must be based on an even power-of-two >= WORLD_WIDTH. Used to quickly round our position to nearest cell wall using bitwise AND.
    static constexpr auto MAGIC_CONSTANT = (Utils::isPowerOfTwo(WORLD_WIDTH) ? WORLD_WIDTH : Utils::nextPowerOfTwo(WORLD_WIDTH))-CELL_WIDTH;                
    static constexpr auto MAP_SCALE_FACTOR = static_cast<int>(1.0f / (static_cast<float>(MAP_WIDTH) / WORLD_WIDTH)); //could invert this (eg: *0.25 instead of /4), but I'll take this ugly casting business once to get integer math throughout the runtime.
    static constexpr auto MAP_HEIGHT = (WORLD_ROWS * CELL_HEIGHT) / MAP_SCALE_FACTOR;
    static constexpr auto SCALED_CELL_WIDTH = CELL_WIDTH / MAP_SCALE_FACTOR;
    static constexpr auto SCALED_CELL_HEIGHT = CELL_HEIGHT / MAP_SCALE_FACTOR;
    static constexpr auto ANGLE_TO_RADIANS = (TWO_PI / ANGLE_360);    
    static constexpr char WORLD[WORLD_ROWS][WORLD_COLUMNS] = { // world map
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,1,1,1,1,1,0,0,0,0,0,0,0,1},
        {1,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1},
        {1,0,0,1,0,0,0,1,0,0,0,0,0,0,0,1},
        {1,0,0,1,0,0,1,1,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,0,0,1,1,0,0,1,1,1,1,1,1,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1},
        {1,0,0,1,1,1,0,0,0,0,0,0,1,0,0,1},
        {1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1},
        {1,0,0,1,1,1,1,1,1,1,1,0,1,0,0,1},
        {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    // tangent tables equivalent to slopes, used to compute initial intersections with ray
    std::array<float, ANGLE_360 + 1> tan_table; 
    std::array<float, ANGLE_360 + 1> inv_tan_table;

    // step tables used to find next intersections, equivalent to slopes times width and height of cell    
    std::array<float, ANGLE_360 + 1> y_step; //x and y steps, used to find intersections after initial one is found
    std::array<float, ANGLE_360 + 1> x_step;
    
    // 1/cos and 1/sin tables used to compute distance of intersection very quickly  
    // Optimization: cos(X) == sin(X+90), so for cos lookups we can simply re-use the sin-table with an offset of ANGLE_90.    
    std::array<float, ANGLE_360 + 1 + ANGLE_90> inv_sin_table;   //+90 degrees to make room for the tail-end of the offset cos values.    
    float* inv_cos_table = &inv_sin_table[ANGLE_90]; //cos(X) == sin(X+90).
    
    // cos table used to fix view distortion caused by radial projection (eg: cancel out fishbowl effect)
    std::array<float, HALF_FOV_ANGLE * 2 + 1> cos_table;

    void Build_Tables() noexcept {
        constexpr auto TENTH_OF_A_RADIAN = ANGLE_TO_RADIANS * 0.1f; //original hardcoded value: 3.272e-4f, or 0.0003272f, matching TWO_PI / MAX_NUMBER_OF_ANGLES.     
        for (int ang = ANGLE_0; ang <= ANGLE_360; ang++) {
            const auto rad_angle = TENTH_OF_A_RADIAN + (ang * ANGLE_TO_RADIANS); //adding a small offset to avoid edge cases with 0. 
            tan_table[ang] = std::tan(rad_angle);
            inv_tan_table[ang] = 1.0f / tan_table[ang];
            // tangent has the incorrect signs in all quadrants except 1, so manually fix the signs of each quadrant. Since the tangent is
            // equivalent to the slope of a line, if the tangent is wrong then the ray that is cast will be wrong
            if (ang >= ANGLE_0 && ang < ANGLE_180) { //upper half plane (eg. upper right & left quadrants)
                y_step[ang] = std::abs(tan_table[ang] * CELL_HEIGHT);
            }
            else {
                y_step[ang] = -std::abs(tan_table[ang] * CELL_HEIGHT); 
            }
            if (ang >= ANGLE_90 && ang < ANGLE_270) { //left half plane (left up and down quads)
                x_step[ang] = -std::abs(inv_tan_table[ang] * CELL_WIDTH);
            }
            else {
                x_step[ang] = std::abs(inv_tan_table[ang] * CELL_WIDTH);
            }     

            //asymtotic rays goes to infinity. this test was originally handled in the ray caster inner loop, but never triggered during development. Moved to build, as sanity check.            
            SDL_assert(std::fabs(y_step[ang]) != 0 && "Potential asymtotic ray on the y-axis produced while building lookup tables.");
            SDL_assert(std::fabs(x_step[ang]) != 0 && "Potential asymtotic ray on the x-axis produced while building lookup tables.");           
          
            inv_sin_table[ang] = 1.0f / std::sin(rad_angle);
        }
        auto end = std::end(inv_sin_table) - ANGLE_90;
        std::copy_n(std::begin(inv_sin_table), ANGLE_90, end); //duplicate the first 90 sin values at the end of the array, to complete the joint sin & cos lookup table.
                
        // create view filter table. There is a cosine wave modulated on top of the view distance as a side effect of casting from a fixed point.
        // to cancel this effect out, we multiple by the inverse of the cosine and the result is the proper scale.  Without this we would see a fishbowl effect
        for (int ang = -HALF_FOV_ANGLE; ang <= HALF_FOV_ANGLE; ang++) {
            const auto rad_angle = TENTH_OF_A_RADIAN + (ang * ANGLE_TO_RADIANS);
            const auto index = ang + HALF_FOV_ANGLE;            
            cos_table[index] = (K / std::cos(rad_angle));
        }
    }
    
    void sline(int x1, int y1, int x2, int y2, const SDL_Color& color) noexcept {
        // used a a diagnostic function to draw a scaled line
        x1 = x1 / MAP_SCALE_FACTOR;
        y1 = MAP_HEIGHT - (y1 / MAP_SCALE_FACTOR);
        x2 = x2 / MAP_SCALE_FACTOR;
        y2 = MAP_HEIGHT - (y2 / MAP_SCALE_FACTOR);
        _setcolor(color);
        _moveto(x1, y1);
        _lineto(x2, y2);
    }

    void splot(int x, int y, const SDL_Color& color) const noexcept{
        // used as a diagnostic function to draw a scaled point
        x = x / MAP_SCALE_FACTOR;
        y = MAP_HEIGHT - (y / MAP_SCALE_FACTOR);
        _setcolor(color);
        _setpixel(x, y);
        _setpixel(x + 1, y);
        _setpixel(x, y + 1);
        _setpixel(x + 1, y + 1);
    }

    void Draw_2D_Map() const noexcept {  
        if constexpr (false == Config::hasMinimap()) { return;  }
        for (int row = 0; row < WORLD_ROWS; row++) {
            const auto top = row * SCALED_CELL_HEIGHT;
            const auto bottom = top + SCALED_CELL_HEIGHT - 1;
            for (int column = 0; column < WORLD_COLUMNS; column++) {
                const auto left = column * SCALED_CELL_WIDTH;                
                const auto right = left + SCALED_CELL_WIDTH - 1;                
                const auto block = WORLD[row][column];
                if (block == 0) {
                    _setcolor(White);
                    _rectangle(RectStyle::OUTLINE, left, top, right, bottom);
                }
                else {
                    _setcolor(DarkGreen);
                    _rectangle(RectStyle::FILL, left, top, right, bottom);
                }
            }
        }
    }
        
    void Ray_Caster(int x, int y, int view_angle) noexcept {
        // This function casts out RAY_COUNT rays from the viewer and builds up the display based on the intersections with the walls.
        // The distance to the first horizontal and vertical edge is recorded. The closest intersection is the one used to draw the display.
        // The inverse of that distance is used to compute the height of the "sliver" of texture that will be drawn on the screen
        if ((view_angle -= HALF_FOV_ANGLE) < 0) { // compute starting angle from player. Field of view is FOV angles, subtract half of that from the current view angle
            view_angle = ANGLE_360 + view_angle;
        }
        for (int ray = 0; ray < RAY_COUNT; ray++) {
            RayEnd xray = cast_horizontal(x, y, view_angle); //find first collision with a horizontal wall
            RayEnd yray = cast_vertical(x, y, view_angle); // and first collision with a vertical wall            
            SDL_Color color = WALL_BOUNDARY_COLOR;
            const float min_dist = (xray < yray) ? xray.distance : yray.distance;
            if (xray < yray) { // there was a vertical wall closer than a horizontal wall                
                if (xray.intersection % CELL_HEIGHT > 1) {
                    color = VERTICAL_WALL_COLOR;
                }
                if constexpr (Config::hasMinimap()) {
                    sline(x, y, xray.boundary, xray.intersection, color);
                }
            }
            else { // must have hit a horizontal wall first                            
                if (yray.intersection % CELL_WIDTH > 1) {
                    color = HORIZONTAL_WALL_COLOR;
                }
                if constexpr (Config::hasMinimap()) {
                    sline(x, y, yray.intersection, yray.boundary, color);
                }
            }
            // height of the sliver is based on the inverse distance to the intersection. Closer is bigger, so: height = 1/dist. However, 1 is too low a factor to look good. Thus the constant K which has been pre-multiplied into the view-filter lookup-table.
            const int height = static_cast<int>(cos_table[ray] / min_dist);                          
            const int clipped_height = (height > VIEWPORT_HEIGHT) ? VIEWPORT_HEIGHT : height;
            const int top = VIEWPORT_HORIZON - (clipped_height >> 1); //Optimization: height >> 1 == height / 2. slivers are drawn symmetrically around the viewport horizon.             
            const int bottom = top + clipped_height;
            const int sliver_x = (VIEWPORT_RIGHT - ray);
            _setcolor(color);
            _moveto(sliver_x, top);
            _lineto(sliver_x, bottom);

            if (++view_angle >= ANGLE_360) {
                view_angle = 0; //reset angle back to zero
            }
        }
    } 

    RayBegin init_horizontal_ray(int x, int y, int view_angle) const noexcept {
        float yi;    // used to track the y intersections  
        int x_bound; // the next horizontal intersection point   
        int x_delta; // the amount needed to move to get to the next cell position
        int next_x_cell; // used to figure out the quadrant of the ray     
        if (view_angle < ANGLE_90 || view_angle >= ANGLE_270) { // compute first vertical line that could be intersected with ray. note: it will be to the right of player
            //x_bound = CELL_WIDTH + CELL_WIDTH * (x / CELL_WIDTH);  //round x to nearest CELL_WIDTH (power-of-2)
            x_bound = (CELL_WIDTH + (x & MAGIC_CONSTANT)); //Optmization of the above.
            x_delta = CELL_WIDTH; // compute delta to get to next vertical line                
            yi = tan_table[view_angle] * (x_bound - x) + y; // based on first possible vertical intersection line, compute Y intercept, so that casting can begin                
            next_x_cell = 0; // set cell delta
        }
        else { // compute first vertical line that could be intersected with ray. note: it will be to the left of player
            x_bound = (x & MAGIC_CONSTANT); //Optmization of the above.
            x_delta = -CELL_WIDTH; // compute delta to get to next vertical line                
            yi = tan_table[view_angle] * (x_bound - x) + y; // based on first possible vertical intersection line, compute Y intercept, so that casting can begin                                
            next_x_cell = -1; // set cell delta
        }
        return RayBegin{ yi, x_bound, x_delta, next_x_cell };
    }

    RayBegin init_vertical_ray(int x, int y, int view_angle) const noexcept {
        float xi;  // used to track the x intersections
        int y_bound;  // the next vertical intersection point    
        int y_delta; // the amount needed to move to get to the next cell position
        int next_y_cell; // used to figure out the quadrant of the ray
        if (view_angle >= ANGLE_0 && view_angle < ANGLE_180) { // compute first horizontal line that could be intersected with ray. note: it will be above player
            //y_bound = CELL_HEIGHT + CELL_HEIGHT * (y / CELL_HEIGHT); //round y to nearest CELL_HEIGHT (power-of-2) 
            y_bound = (CELL_HEIGHT + (y & MAGIC_CONSTANT)); //Optimization. Achieves same as above. 
            y_delta = CELL_HEIGHT; // compute delta to get to next horizontal line                
            xi = inv_tan_table[view_angle] * (y_bound - y) + x; // based on first possible horizontal intersection line, compute X intercept, so that casting can begin
            next_y_cell = 0; // set cell delta
        }
        else { // compute first horizontal line that could be intersected with ray. note: it will be below player
            y_bound = (y & MAGIC_CONSTANT); //Optimization, same as above.
            y_delta = -CELL_HEIGHT; // compute delta to get to next horizontal line                
            xi = inv_tan_table[view_angle] * (y_bound - y) + x; // based on first possible horizontal intersection line, compute X intercept, so that casting can begin              
            next_y_cell = -1; // set cell delta
        }
        return RayBegin{ xi, y_bound, y_delta, next_y_cell };
    }

    RayEnd cast_horizontal(int x, int y, int view_angle) const noexcept  {       
        auto [yi,  x_bound, x_delta, next_x_cell] = init_horizontal_ray(x, y, view_angle);         
        RayEnd result;
        while (x_bound < WORLD_WIDTH) {
            const int cell_x = ((x_bound + next_x_cell) >> CELL_WIDTH_FP); //Optimization: shift instead of divide, might help the Arduboy
            const int cell_y = static_cast<int>(yi) >> CELL_HEIGHT_FP;                   
            if (!isWall(cell_x, cell_y)) {
                yi += y_step[view_angle]; // compute next Y intercept
                x_bound += x_delta; // move to next possible intersection points
                continue;
            }
            result.distance = (yi - y) * inv_sin_table[view_angle]; // compute distance to hit
            result.boundary = x_bound; // record intersections with cell boundaries
            result.intersection = static_cast<int>(yi);
            return result;                        
        }        
        return result;          
    }  
  
    RayEnd cast_vertical(int x, int y, int view_angle) const noexcept {        
        auto [xi, y_bound, y_delta, next_y_cell] = init_vertical_ray(x, y, view_angle);
        RayEnd result;
        while(y_bound < WORLD_HEIGHT){
            const int cell_x = static_cast<int>(xi) >> CELL_WIDTH_FP;   // the current cell that the ray is in             
            const int cell_y = ((y_bound + next_y_cell) >> CELL_HEIGHT_FP);
            if (!isWall(cell_x, cell_y)) {
                xi += x_step[view_angle]; //compute next X intercept
                y_bound += y_delta;
                continue;
            }
            result.distance = (xi - x) * inv_cos_table[view_angle];
            result.boundary = y_bound;
            result.intersection = static_cast<int>(xi);                                        
            return result;            
        }
        return result;
    } 

    void clearWindow() {
        _setcolor(Black);
        _r.clear();
        _setcolor(Gray);
        _rectangle(RectStyle::FILL, VIEWPORT_LEFT, VIEWPORT_TOP, VIEWPORT_RIGHT, VIEWPORT_HORIZON);
        _setcolor(Brown);
        _rectangle(RectStyle::FILL, VIEWPORT_LEFT, VIEWPORT_HORIZON, VIEWPORT_RIGHT, VIEWPORT_BOTTOM);
        _setcolor(DarkRed);
        _rectangle(RectStyle::OUTLINE, VIEWPORT_LEFT - 1, VIEWPORT_TOP - 1, VIEWPORT_RIGHT + 1, VIEWPORT_BOTTOM + 1); //draw line around viewport
    }

    void updateViewPoint() {       
        _viewPoint.dx = 0.0f;
        _viewPoint.dy = 0.0f;                  
        if (_input.isButtonDown(MouseButton::LEFT)) {
            const int mouseX = _input.mouseX();
            const int mouseY = _input.mouseY();            
            const int mouseCellX = mouseX / SCALED_CELL_WIDTH;
            const int mouseCellY = mouseY / SCALED_CELL_HEIGHT;
            std::cout << "x:"<< mouseX << " y:" << mouseY << " / cellx:" << mouseCellX << " celly:" << mouseCellY << "\n";
            if (!isWall(mouseCellX, mouseCellY)) {
                centerPlayerInCell(mouseCellX, mouseCellY);
                return;
            }
        }

        if (_input.isAnyKeyDown(rotateRight)) {
            if ((_viewPoint.angle -= ROTATION_SPEED) < ANGLE_0) {
                _viewPoint.angle = ANGLE_360;
            }
        }
        else if (_input.isAnyKeyDown(rotateLeft)) {
            if ((_viewPoint.angle += ROTATION_SPEED) >= ANGLE_360) {
                _viewPoint.angle = ANGLE_0;
            }
        }
        if (_input.isAnyKeyDown(moveForward)) {            
            _viewPoint.dx = cos(_viewPoint.angle*ANGLE_TO_RADIANS) * WALK_SPEED;
            _viewPoint.dy = sin(_viewPoint.angle*ANGLE_TO_RADIANS) * WALK_SPEED;
            std::cout << "player x:" << _viewPoint.x << " player y:" << _viewPoint.y << " angle: " << _viewPoint.angle << "\n";
            
        } 
        else if (_input.isAnyKeyDown(moveBackward)) {            
            _viewPoint.dx = -cos(_viewPoint.angle*ANGLE_TO_RADIANS) * WALK_SPEED;
            _viewPoint.dy = -sin(_viewPoint.angle*ANGLE_TO_RADIANS) * WALK_SPEED;
            std::cout << "player x:" << _viewPoint.x << " player y:" << _viewPoint.y << " angle: " << _viewPoint.angle << "\n";
        }       
        _viewPoint.x += static_cast<int>(_viewPoint.dx);
        _viewPoint.y += static_cast<int>(_viewPoint.dy);
    }


    void checkCollisions() {
        // test if user has bumped into a wall i.e. test if there is a cell within the direction of motion, if so back up!                               
        const int x_cell = _viewPoint.x / CELL_WIDTH;
        const int y_cell = _viewPoint.y / CELL_HEIGHT;                
        const int x_sub_cell = _viewPoint.x % CELL_WIDTH; // compute position within the cell
        const int y_sub_cell = _viewPoint.y % CELL_HEIGHT;          
       
        if (isWall(x_cell, y_cell)) { //standing inside a wall, somehow. 
            return centerPlayerInCell(FIRST_VALID_CELL, FIRST_VALID_CELL);
        }
        if (_viewPoint.dx > 0 && isWall(x_cell + 1, y_cell)) {// moving right, towards a wall
            if (x_sub_cell > (CELL_WIDTH - OVERBOARD)) {                
                _viewPoint.x -= (x_sub_cell - (CELL_WIDTH - OVERBOARD)); // back player up amount they stepped over the line
            }
        }
        else if (_viewPoint.dx < 0 && isWall(x_cell - 1, y_cell)) {// moving left, towards a wall
            if (x_sub_cell < (OVERBOARD)) {                
                _viewPoint.x += (OVERBOARD - x_sub_cell);
            }
        }

        if (_viewPoint.dy > 0 && isWall(x_cell, y_cell + 1)) { // moving up
            if (y_sub_cell > (CELL_HEIGHT - OVERBOARD)) {                
                _viewPoint.y -= (y_sub_cell - (CELL_HEIGHT - OVERBOARD));
            }
        }
        else if (_viewPoint.dy < 0 && isWall(x_cell, y_cell - 1)) {// moving down            
            if (y_sub_cell < (OVERBOARD)) {             
                _viewPoint.y += (OVERBOARD - y_sub_cell);
            }
        }
    }
      
    inline constexpr bool isWall(int x, int y) const noexcept {
        const auto LAST_ROW = (WORLD_ROWS - 1);
        y = LAST_ROW - y;
        if (x < FIRST_VALID_CELL || y < FIRST_VALID_CELL 
            || x > LAST_VALID_CELL || y > LAST_VALID_CELL) {
            return true;
        }        
        return (WORLD[y][x] != 0);
    }

    inline constexpr void centerPlayerInCell(int cellx, int celly) noexcept {
        _viewPoint.x = cellx * CELL_WIDTH  + (CELL_WIDTH >> 1);
        _viewPoint.y = celly * CELL_HEIGHT + (CELL_HEIGHT >> 1);
    }

public:
    Ray(const Renderer& r, InputManager& input) : _r(r), _input(input) {
        centerPlayerInCell(START_POS_X, START_POS_Y);        
        _viewPoint.angle = ANGLE_45;
        Build_Tables();
    }
       
    int run() {   
        while (!_input.quitRequested()) {
            _input.update();
            clearWindow();
            updateViewPoint();
            checkCollisions();
            if constexpr (Config::hasMinimap()) { Draw_2D_Map(); }
            
            Ray_Caster(_viewPoint.x, _viewPoint.y, _viewPoint.angle);
            _r.present();
            SDL_Delay(16);
        }
        return 0;
    }
};