#pragma once
#include <array>

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

    static constexpr auto ANGLE_0 = 0; 
    static constexpr auto ANGLE_5 = 25;    
    static constexpr auto ANGLE_15 = 80;
    static constexpr auto ANGLE_30 = 160;
    static constexpr auto ANGLE_45 = 240;    
    static constexpr auto ANGLE_90 = 480;    
    static constexpr auto ANGLE_180 = 960;    
    static constexpr auto ANGLE_270 = 1440;    
    static constexpr auto ANGLE_360 = 1920;

    static constexpr auto RAY_COUNT = 320;
    static constexpr auto WORLD_ROWS = 16; // number of rows in the game world
    static constexpr auto WORLD_COLUMNS = 16; // number of columns in the game world
    static constexpr auto CELL_WIDTH = 64; // size of a cell in the gamw world
    static constexpr auto CELL_HEIGHT = 64;
    static constexpr auto WORLD_WIDTH = (WORLD_COLUMNS * CELL_WIDTH);
    static constexpr auto WORLD_HEIGHT = (WORLD_ROWS * CELL_HEIGHT);
    static constexpr auto OVERBOARD = 32; // the absolute closest a player can get to a wall
    static constexpr auto START_POS_X = 8;
    static constexpr auto START_POS_Y = 3;
    static constexpr auto TWO_PI = 2.0f * 3.141592654f;
    static constexpr auto MAX_X = 638; //how far to the right we can draw.
    static constexpr auto MIN_X = 2; //how far to the left we can draw.
    static auto constexpr WALK_SPEED = 10;
    static auto constexpr VIEWPORT_LEFT = 319;
    static auto constexpr VIEWPORT_TOP = 1;
    static auto constexpr VIEWPORT_RIGHT = 638;
    static auto constexpr VIEWPORT_BOTTOM = 200;
    static auto constexpr VIEWPORT_HORIZON = 100;
    static auto constexpr ROTATION_SPEED = ANGLE_5;
    static constexpr auto MAP_SCALE_FACTOR = 4;
    static constexpr auto ORIG_Y = (WORLD_ROWS * CELL_HEIGHT) / MAP_SCALE_FACTOR;

    static constexpr auto DUNNO = 3.272e-4f;
    static constexpr auto EPSILON_MAYBE = 1e-10f;
    static auto constexpr A_SIZE_MAYBE = 15000.0f;
    static constexpr auto ASYMTOTIC_DISTANCE_VALUE = 1e+8f;

    // world map, each cell is 64x64 pixels
    static constexpr char world[WORLD_ROWS][WORLD_COLUMNS] = {
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

    // tangent tables equivalent to slopes
    std::array<float, ANGLE_360 + 1> tan_table; // tangent tables used to compute initial
    std::array<float, ANGLE_360 + 1> inv_tan_table; // intersections with ray

    // step tables used to find next intersections, equivalent to slopes
    // times width and height of cell
    std::array<float, ANGLE_360 + 1> y_step; // x and y steps, used to find intersections
    std::array<float, ANGLE_360 + 1> x_step; // after initial one is found

    // cos table used to fix view distortion caused by caused by radial projection
    std::array<float, ANGLE_360 + 1> cos_table; // used to cacell out fishbowl effect

    // 1/cos and 1/sin tables used to compute distance of intersection very quickly
    std::array<float, ANGLE_360 + 1> inv_cos_table; // used to compute distances by calculating
    std::array<float, ANGLE_360 + 1> inv_sin_table; // the hypontenuse       

    void Build_Tables() noexcept {
        for (int ang = ANGLE_0; ang <= ANGLE_360; ang++) {
            const float rad_angle = DUNNO + ang * TWO_PI / ANGLE_360;
            tan_table[ang] = std::tanf(rad_angle);
            inv_tan_table[ang] = 1.0f / tan_table[ang];
            // tangent has the incorrect signs in all quadrants except 1, so manually fix the signs of each quadrant since the tangent is
            // equivalent to the slope of a line and if the tangent is wrong then the ray that is case will be wrong
            if (ang >= ANGLE_0 && ang < ANGLE_180) {
                y_step[ang] = std::fabs(tan_table[ang] * CELL_HEIGHT);
            }
            else {
                y_step[ang] = -std::fabs(tan_table[ang] * CELL_HEIGHT);
            }
            if (ang >= ANGLE_90 && ang < ANGLE_270) {
                x_step[ang] = -std::fabs(inv_tan_table[ang] * CELL_WIDTH);
            }
            else {
                x_step[ang] = std::fabs(inv_tan_table[ang] * CELL_WIDTH);
            }
            // create the sin and cosine tables to copute distances
            inv_cos_table[ang] = 1.0f / std::cos(rad_angle);
            inv_sin_table[ang] = 1.0f / std::sin(rad_angle);
        }
        // create view filter table.  There is a cosine wave modulated on top of the view distance as a side effect of casting from a fixed point.
        // to cancel this effect out, we multiple by the inverse of the cosine and the result is the proper scale.  Without this we would see a fishbowl effect
        for (int ang = -ANGLE_30; ang <= ANGLE_30; ang++) {
            const float rad_angle = DUNNO + ang * TWO_PI / ANGLE_360;
            const int index = ang + ANGLE_30;
            cos_table[index] = 1.0f / std::cos(rad_angle);
        }
    }

    void sline(int x1, int y1, int x2, int y2, const SDL_Color& color) noexcept {
        // used a a diagnostic function to draw a scaled line
        x1 = x1 / MAP_SCALE_FACTOR;
        y1 = ORIG_Y - (y1 / MAP_SCALE_FACTOR);
        x2 = x2 / MAP_SCALE_FACTOR;
        y2 = ORIG_Y - (y2 / MAP_SCALE_FACTOR);
        _setcolor(color);
        _moveto(x1, y1);
        _lineto(x2, y2);
    }

    void splot(int x, int y, const SDL_Color& color) const noexcept{
        // used as a diagnostic function to draw a scaled point
        x = x / MAP_SCALE_FACTOR;
        y = ORIG_Y - (y / MAP_SCALE_FACTOR);
        _setcolor(color);
        _setpixel(x, y);
        _setpixel(x + 1, y);
        _setpixel(x, y + 1);
        _setpixel(x + 1, y + 1);
    }

    void Draw_2D_Map() const noexcept {
        static constexpr auto SCALED_CELL_WIDTH = CELL_WIDTH / MAP_SCALE_FACTOR;
        static constexpr auto SCALED_CELL_HEIGHT = CELL_HEIGHT / MAP_SCALE_FACTOR;
        for (int row = 0; row < WORLD_ROWS; row++) {
            for (int column = 0; column < WORLD_COLUMNS; column++) {
                const int left = column * SCALED_CELL_WIDTH;
                const int top = row * SCALED_CELL_HEIGHT;
                const int right = left + SCALED_CELL_WIDTH - 1;
                const int bottom = top + SCALED_CELL_HEIGHT - 1;
                const int block = world[row][column];
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
        
    void Ray_Caster(int x, int y, int view_angle) {
        // This function casts out 320 rays from the viewer and builds up the video
        // display based on the intersections with the walls. The 320 rays are
        // cast in such a way that they all fit into a 60 degree field of view
        // a ray is cast and then the distance to the first horizontal and vertical
        // edge that has a cell in it is recorded.  The intersection that has the
        // closer distance to the user is the one that is used to draw the bitmap.
        // the distance is used to compute the height of the "sliver" of texture
        // or line that will be drawn on the screen        

         // SECTION 1 /////////////////////////////////////////////////////////
        // compute starting angle from player.  Field of view is 60 degrees, so subtract half of that from the current view angle
        if ((view_angle -= ANGLE_30) < 0){
            view_angle = ANGLE_360 + view_angle; // wrap angle around
        }
        for (int ray = 0; ray < RAY_COUNT; ray++) {
            // SECTION 2 /////////////////////////////////////////////////////////
            // compute first x intersection
            // need to know which half plane we are casting from relative to Y axis
            float xi;  // used to track the x and y intersections
            int y_bound;  // the next vertical and horizontal intersection point    
            int y_delta; // the amount needed to move to get to the next cell position
            int next_y_cell; // used to figure out the quadrant of the ray
            if (view_angle >= ANGLE_0 && view_angle < ANGLE_180) {
                // compute first horizontal line that could be intersected with ray
                // note: it will be above player
                y_bound = CELL_HEIGHT + CELL_HEIGHT * (y / CELL_HEIGHT);// compute delta to get to next horizontal line                
                y_delta = CELL_HEIGHT;
                xi = inv_tan_table[view_angle] * (y_bound - y) + x; // based on first possible horizontal intersection line, compute X intercept, so that casting can begin
                next_y_cell = 0; // set cell delta
            }
            else {
                // compute first horizontal line that could be intersected with ray
                // note: it will be below player
                y_bound = CELL_HEIGHT * (y / CELL_HEIGHT); // compute delta to get to next horizontal line                
                y_delta = -CELL_HEIGHT;
                xi = inv_tan_table[view_angle] * (y_bound - y) + x; // based on first possible horizontal intersection line, compute X intercept, so that casting can begin              
                next_y_cell = -1; // set cell delta
            }

            int x_bound;       // the next vertical and horizontal intersection point    
            float yi;  // used to track the x and y intersections  
            int x_delta; // the amount needed to move to get to the next cell position
            int next_x_cell; // used to figure out the quadrant of the ray            
            // SECTION 3 /////////////////////////////////////////////////////////
            // compute first y intersection
            // need to know which half plane we are casting from relative to X axis
            if (view_angle < ANGLE_90 || view_angle >= ANGLE_270) {
                // compute first vertical line that could be intersected with ray
                // note: it will be to the right of player
                x_bound = CELL_WIDTH + CELL_WIDTH * (x / CELL_WIDTH);
                x_delta = CELL_WIDTH; // compute delta to get to next vertical line                
                yi = tan_table[view_angle] * (x_bound - x) + y; // based on first possible vertical intersection line, compute Y intercept, so that casting can begin                
                next_x_cell = 0; // set cell delta
            }
            else {
                // compute first vertical line that could be intersected with ray
                // note: it will be to the left of player
                x_bound = CELL_WIDTH * (x / CELL_WIDTH);
                x_delta = -CELL_WIDTH; // compute delta to get to next vertical line                
                yi = tan_table[view_angle] * (x_bound - x) + y; // based on first possible vertical intersection line, compute Y intercept, so that casting can begin                                
                next_x_cell = -1; // set cell delta
            }

            // SECTION 4 /////////////////////////////////////////////////////////
            // begin cast
            int casting = 2; // two rays to cast simultaneously
            bool xray_intersection_found = false;
            bool yray_intersection_found = false;
            int xb_save = 0; // storage to record intersections cell boundaries
            int yb_save = 0;
            int xi_save = 0; // used to save exact x and y intersection points
            int yi_save = 0;
            float dist_x = 0.0f; // the distance of the x and y ray intersections from
            float dist_y = 0.0f; // the viewpoint
            while (casting) {
                // continue casting each ray in parallel
                if (!xray_intersection_found) {
                    if (std::fabs(y_step[view_angle]) == 0) { // test for asymtotic ray      
                        xray_intersection_found = true;
                        casting--;
                        dist_x = ASYMTOTIC_DISTANCE_VALUE;
                    }
                    // compute current map position to inspect
                    int cell_x = ((x_bound + next_x_cell) / CELL_WIDTH);   // the current cell that the ray is in
                    int cell_y = Utils::clamp(static_cast<int>(yi / CELL_HEIGHT), 0, WORLD_ROWS); //TODO: this is a hack. YI goes out of bounds when rotating! 

                    // test if there is a block where the current x ray is intersecting
                    const int x_hit_type = world[(WORLD_ROWS - 1) - cell_y][cell_x];  // records the block that was intersected, used to figure  out which texture to use
                    if (x_hit_type != 0) {
                        dist_x = (yi - y) * inv_sin_table[view_angle]; // compute distance to hit
                        yi_save = static_cast<int>(yi);
                        xb_save = x_bound;
                        xray_intersection_found = true;
                        casting--;
                    } else {
                        yi += y_step[view_angle]; // compute next Y intercept
                    }
                }

                // SECTION 5 /////////////////////////////////////////////////////////
                if (!yray_intersection_found) {
                    if (std::fabs(x_step[view_angle]) == 0) { // test for asymtotic ray
                        yray_intersection_found = true;
                        casting--;
                        dist_y = ASYMTOTIC_DISTANCE_VALUE;
                    }
                    // compute current map position to inspect
                    int cell_x = static_cast<int>(xi / CELL_WIDTH);   // the current cell that the ray is in
                    int cell_y = static_cast<int>((y_bound + next_y_cell) / CELL_HEIGHT);
                    // test if there is a block where the current y ray is intersecting
                    const int y_hit_type = world[(WORLD_ROWS - 1) - cell_y][cell_x];  // records the block that was intersected, used to figure  out which texture to use
                    if (y_hit_type != 0) {
                        dist_y = (xi - x) * inv_cos_table[view_angle]; // compute distance
                        xi_save = static_cast<int>(xi);
                        yb_save = y_bound;
                        yray_intersection_found = true;
                        casting--;
                    } else {
                        xi += x_step[view_angle]; // compute next X intercept
                    }
                }
                x_bound += x_delta; // move to next possible intersection points
                y_bound += y_delta;
            }

            // SECTION 6 /////////////////////////////////////////////////////////
            // at this point, we know that the ray has succesfully hit both a vertical wall and a horizontal wall, so we need to see which one
            // was closer and then render it
            // note: later we will replace the crude monochrome line with a sliver of texture, but this is good enough for now            
            float scale; // the final scale to draw the "sliver" in            
            SDL_Color color = LightGreen;
            if (dist_x < dist_y) { // there was a vertical wall closer than the horizontal
                sline(x, y, xb_save, yi_save, LightGreen);
                scale = cos_table[ray] * A_SIZE_MAYBE / (EPSILON_MAYBE + dist_x); // compute actual scale and multiply by view filter so that spherical distortion is cancelled                 
                if (yi_save % CELL_HEIGHT > 1) {
                    color = LightGreen;
                } else {
                    color = White; //draw divider between wall sections                
                }
            }
            else { // must have hit a horizontal wall first
                sline(x, y, xi_save, yb_save, DarkGreen);
                scale = cos_table[ray] * A_SIZE_MAYBE / (EPSILON_MAYBE + dist_y); // compute actual scale and multiply by view filter so that spherical distortion is cancelled        
                if (xi_save % CELL_WIDTH > 1) {
                    color = DarkGreen;
                }
                else {
                    color = White; //draw divider between wall sections                
                }                
            }
            // compute the top and bottom of the sliver (with crude clipping), which is drawn symmetrically around the viewport horizon. 
            int top = Utils::clamp(VIEWPORT_HORIZON - static_cast<int>(scale / 2.0f), VIEWPORT_TOP, VIEWPORT_BOTTOM);
            int bottom = Utils::clamp(static_cast<int>(top + scale), VIEWPORT_TOP, VIEWPORT_BOTTOM);            
            _setcolor(color);
            _moveto((MAX_X - ray), top);
            _lineto((MAX_X - ray), bottom);

            //move on to next ray
            if (++view_angle >= ANGLE_360){
                view_angle = 0;// reset angle back to zero
            }
        }
    } // end Ray_Caster
      
    void clearWindow() {
        _setcolor(Black);
        _r.clear();
        _setcolor(Gray);
        _rectangle(RectStyle::FILL, VIEWPORT_LEFT, VIEWPORT_TOP, VIEWPORT_RIGHT, VIEWPORT_HORIZON);
        _setcolor(Brown);
        _rectangle(RectStyle::FILL, VIEWPORT_LEFT, VIEWPORT_HORIZON, VIEWPORT_RIGHT, VIEWPORT_BOTTOM);
        _setcolor(White);
        _rectangle(RectStyle::OUTLINE, VIEWPORT_LEFT - 1, VIEWPORT_TOP - 1, VIEWPORT_RIGHT + 1, VIEWPORT_BOTTOM + 1); //draw line around map
    }

    void updateViewPoint() {
        _viewPoint.dx = 0.0f;
        _viewPoint.dy = 0.0f;
        if (_input.isKeyDown(SDL_SCANCODE_KP_6) || _input.isKeyDown(SDL_SCANCODE_RIGHT)) {
            if ((_viewPoint.angle -= ROTATION_SPEED) < ANGLE_0) {
                _viewPoint.angle = ANGLE_360;
            }
        }
        else if (_input.isKeyDown(SDL_SCANCODE_KP_4) || _input.isKeyDown(SDL_SCANCODE_LEFT)) {
            if ((_viewPoint.angle += ROTATION_SPEED) >= ANGLE_360) {
                _viewPoint.angle = ANGLE_0;
            }
        }
        if (_input.isKeyDown(SDL_SCANCODE_KP_8) || _input.isKeyDown(SDL_SCANCODE_UP)) {
            _viewPoint.dx = cos(TWO_PI * _viewPoint.angle / ANGLE_360) * WALK_SPEED;     // move player along view vector foward
            _viewPoint.dy = sin(TWO_PI * _viewPoint.angle / ANGLE_360) * WALK_SPEED;
        } 
        else if (_input.isKeyDown(SDL_SCANCODE_KP_2) || _input.isKeyDown(SDL_SCANCODE_DOWN)) {
            _viewPoint.dx = -cos(TWO_PI * _viewPoint.angle / ANGLE_360) * WALK_SPEED; // move player along view vector backward
            _viewPoint.dy = -sin(TWO_PI * _viewPoint.angle / ANGLE_360) * WALK_SPEED;
        }       
        if (_input.isKeyDown(SDL_SCANCODE_A)) { //strafe left
          //TODO
        }
        else if (_input.isKeyDown(SDL_SCANCODE_A)) { //strafe right
          //TODO
        }
        _viewPoint.x += static_cast<int>(_viewPoint.dx);
        _viewPoint.y += static_cast<int>(_viewPoint.dy);
    }

    void checkCollisions() {
        // test if user has bumped into a wall i.e. test if there is a cell within the direction of motion, if so back up!                               
        int x_cell = _viewPoint.x / CELL_WIDTH;
        int y_cell = (WORLD_ROWS - 1) - (_viewPoint.y / CELL_HEIGHT);
        int x_sub_cell = _viewPoint.x % CELL_WIDTH; // compute position within the cell
        int y_sub_cell = _viewPoint.y % CELL_HEIGHT;

        if (_viewPoint.dx > 0 && isWall(x_cell + 1, y_cell)) {// moving right, towards a wall
            if (x_sub_cell > (CELL_WIDTH - OVERBOARD)) {
                _viewPoint.x -= (x_sub_cell - (CELL_WIDTH - OVERBOARD)); // back player up amount they stepped over the line
            }
        } else if (_viewPoint.dx < 0 && isWall(x_cell - 1, y_cell)) {// moving left, towards a wall
            if (x_sub_cell < (OVERBOARD)) {
                _viewPoint.x += (OVERBOARD - x_sub_cell);
            }
        }
        if (_viewPoint.dy > 0 && isWall(x_cell, y_cell - 1)) { // moving up, towards a wall
            if (y_sub_cell > (CELL_HEIGHT - OVERBOARD)) {
                _viewPoint.y -= (y_sub_cell - (CELL_HEIGHT - OVERBOARD));
            }
        } else if (_viewPoint.dy < 0 && isWall(x_cell, y_cell + 1)) {// moving down            
            if (y_sub_cell < (OVERBOARD)) {
                _viewPoint.y += (OVERBOARD - y_sub_cell);
            }
        }
    }

    inline const bool isWall(int x, int y) noexcept {
        return (world[y][x] != 0);
    }

public:
    Ray(const Renderer& r, InputManager& input) : _r(r), _input(input) {
        _viewPoint.x = START_POS_X * CELL_WIDTH + CELL_WIDTH / 2;
        _viewPoint.y = START_POS_Y * CELL_HEIGHT + CELL_HEIGHT / 2;
        _viewPoint.angle = ANGLE_45;
        Build_Tables();
    }
       
    int run() {   
        while (!_input.quitRequested()) {
            _input.update();
            clearWindow();
            updateViewPoint();
            checkCollisions();            
            Draw_2D_Map();            
            Ray_Caster(_viewPoint.x, _viewPoint.y, _viewPoint.angle);
            _r.present();
            SDL_Delay(16);
        }
        return 0;
    }
};