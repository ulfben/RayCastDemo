#pragma once
#include <array>
#include <cmath>
#include "Config.h"
#include "LevelData.h"
#include "Graphics.h"
#include "SDLSystem.h"
#include "Utils.h"
#include "MiniMap.h"
class RayCaster {    
    struct RayStart {
        float intersection = 0.0f; //the first possible intersection point
        int boundary = 0; // the next intersection point   
        int delta = 0; // the amount needed to move to get to the next cell position
        int next_cell = 0; //cell delta, to move left / right or up / down
    };
    struct RayEnd {
        float distance = 0.0f; // the distance of intersection from the player
        int boundary = 0; // record intersections with cell boundaries        
        int intersection = 0; // used to save exact intersection point with a wall         
        bool operator <(const RayEnd& that) const noexcept { return distance < that.distance; };
    };    
    static constexpr auto WALL_BOUNDARY_COLOR = White;
    static constexpr auto VERTICAL_WALL_COLOR = LightGreen;
    static constexpr auto HORIZONTAL_WALL_COLOR = DarkGreen;  
    static constexpr auto CEILING_COLOR = Gray;
    static constexpr auto FLOOR_COLOR = Brown;
    //320x240@60fov = K15000, 128x64@60fov = K7000
    static constexpr auto K = 15000.0f;// think of K as a combination of view distance and aspect ratio. Pick a value that looks good. In my case: that makes the block on screen look square. (p.213)            
    //Originally: 0xFFC0 (65472), which is 0xFFFF-CELL_WIDTH. The constant must be an even power-of-two >= WORLD_SIZE. Used to quickly round our position to nearest cell wall using bitwise AND.
    static constexpr auto MAGIC_CONSTANT = (Utils::isPowerOfTwo(WORLD_SIZE) ? WORLD_SIZE : Utils::nextPowerOfTwo(WORLD_SIZE))- Cfg::CELL_SIZE;

    // tangent tables equivalent to slopes, used to compute initial intersections with ray
    std::array<float, ANGLE_360 + 1> tan_table; 
    std::array<float, ANGLE_360 + 1> inv_tan_table;

    // step tables used to find next intersections, equivalent to slopes times width and height of cell    
    std::array<float, ANGLE_360 + 1> y_step; //x and y steps, used to find intersections after initial one is found
    std::array<float, ANGLE_360 + 1> x_step;
    
    // 1/cos and 1/sin tables used to compute distance of intersection very quickly  
    // Optimization: cos(X) == sin(X+90), so for cos lookups we can simply re-use the sin-table with an offset of ANGLE_90.    
    std::array<float, ANGLE_360 + ANGLE_90> inv_sin_table; //+90 degrees to make room for the tail-end of the offset cos values.    
    float* inv_cos_table = &inv_sin_table[ANGLE_90]; //cos(X) == sin(X+90).    

    // cos table used to fix view distortion caused by radial projection (eg: cancel out fishbowl effect)
    std::array<float, HALF_FOV_ANGLE * 2 + 1> cos_table;

    void buildLookupTables() noexcept {
        constexpr auto TENTH_OF_A_RADIAN = ANGLE_TO_RADIANS * 0.1f; //original hardcoded value: 3.272e-4f, or 0.0003272f, matching TWO_PI / MAX_NUMBER_OF_ANGLES.     
        for (int ang = ANGLE_0; ang <= ANGLE_360; ang++) {            
            const auto rad_angle = TENTH_OF_A_RADIAN + (ang * ANGLE_TO_RADIANS); //adding a small offset to avoid edge cases with 0. 
            tan_table[ang] = std::tan(rad_angle);
            inv_tan_table[ang] = 1.0f / tan_table[ang];
            // tangent has the incorrect signs in all quadrants except 1, so manually fix the signs of each quadrant. Since the tangent is
            // equivalent to the slope of a line, if the tangent is wrong then the ray that is cast will be wrong
            if (ang >= ANGLE_0 && ang < ANGLE_180) { //upper half plane (eg. upper right & left quadrants)
                y_step[ang] = std::abs(tan_table[ang] * CELL_SIZE);
            } else {
                y_step[ang] = -std::abs(tan_table[ang] * CELL_SIZE);
            }
            if (ang >= ANGLE_90 && ang < ANGLE_270) { //left half plane (left up and down quads)
                x_step[ang] = -std::abs(inv_tan_table[ang] * CELL_SIZE);
            } else {
                x_step[ang] = std::abs(inv_tan_table[ang] * CELL_SIZE);
            }     
            //asymtotic rays goes to infinity. this test was originally handled in the ray caster inner loop, but never triggered during development. Moved to build, as sanity check.            
            SDL_assert(std::fabs(y_step[ang]) != 0.0f && "Potential asymtotic ray on the y-axis produced while building lookup tables.");
            SDL_assert(std::fabs(x_step[ang]) != 0.0f && "Potential asymtotic ray on the x-axis produced while building lookup tables.");                     
            inv_sin_table[ang] = 1.0f / std::sin(rad_angle);         
        }
        auto end = std::end(inv_sin_table) - ANGLE_90;
        std::copy_n(std::begin(inv_sin_table), ANGLE_90, end); //duplicate the first 90 sin values at the end of the array, to complete the joint sin & cos lookup table.
                
        // create view filter table. There is a cosine wave modulated on top of the view distance as a side effect of casting from a fixed point.
        // to cancel this effect out, we multiple by the inverse of the cosine and the result is the proper scale.  Without this we would see a fishbowl effect.
        // inverse cosine would be 1/cos(rad_angle), but 1 is too small to give us good sized slivers, hence the constant K which is arbitrarily chosen for what looks good. 
        for (int ang = -HALF_FOV_ANGLE; ang <= HALF_FOV_ANGLE; ang++) {
            const auto rad_angle = TENTH_OF_A_RADIAN + (ang * ANGLE_TO_RADIANS);
            const auto index = ang + HALF_FOV_ANGLE;
            cos_table[index] = (K / std::cos(rad_angle));
        }
    }     
  
    inline RayStart initHorizontalRay(const int x, const int y, const int view_angle) const noexcept {        
        const auto FACING_RIGHT = (view_angle < ANGLE_90 || view_angle >= ANGLE_270);        
        const int x_bound = FACING_RIGHT ? CELL_SIZE + (x & MAGIC_CONSTANT) : (x & MAGIC_CONSTANT); //round x to nearest CELL_WIDTH (power-of-2), this is the first possible intersection point. 
        const int x_delta = FACING_RIGHT ? CELL_SIZE : -CELL_SIZE; // the amount needed to move to get to the next vertical line (cell boundary)
        const int next_cell_direction = FACING_RIGHT ? 0 : -1;  //x coordinates increase to the left, and decrease to the right      
        const float yi = tan_table[view_angle] * (x_bound - x) + y; // based on first possible vertical intersection line, compute Y intercept, so that casting can begin                                
        return RayStart{ yi, x_bound, x_delta, next_cell_direction };
    }

    inline RayStart initVerticalRay(const int x, const int y, const int view_angle) const noexcept {
        const auto FACING_DOWN = (view_angle >= ANGLE_0 && view_angle < ANGLE_180);
        const int y_bound = FACING_DOWN ? CELL_SIZE  + (y & MAGIC_CONSTANT) : (y & MAGIC_CONSTANT); //Optimization: round y to nearest CELL_HEIGHT (power-of-2) 
        const int y_delta = FACING_DOWN ? CELL_SIZE : -CELL_SIZE; // the amount needed to move to get to the next horizontal line (cell boundary)
        const int next_cell_direction = FACING_DOWN ? 0 : -1; //remember: y coordinates increase as we move down (south) in the world, and decrease towards the top (north)               
        const float xi = inv_tan_table[view_angle] * (y_bound - y) + x; // based on first possible horizontal intersection line, compute X intercept, so that casting can begin              
        return RayStart{ xi, y_bound, y_delta, next_cell_direction };
    }

    RayEnd findVerticalWall(const int x, const int y, const int view_angle) const noexcept  {
        auto [yi,  x_bound, x_delta, next_x_cell] = initHorizontalRay(x, y, view_angle); // cast a ray horizontally, along the x-axis, to intersect with vertical walls
        RayEnd result;
        while (x_bound > -1 && x_bound < WORLD_SIZE) {
            const int cell_x = ((x_bound + next_x_cell) >> CELL_SIZE_FP); //Optimization: shift instead of divide, might help the Arduboy
            const int cell_y = static_cast<int>(yi) >> CELL_SIZE_FP;                   
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
  
    RayEnd findHorizontalWall(const int x, const int y, const int view_angle) const noexcept {
        auto [xi, y_bound, y_delta, next_y_cell] = initVerticalRay(x, y, view_angle); ///ast a ray vertically, along the y-axis, to intersect with horizontal walls
        RayEnd result;
        while (y_bound > -1 && y_bound < WORLD_SIZE) {
            const int cell_x = static_cast<int>(xi) >> CELL_SIZE_FP;   // the current cell that the ray is in             
            const int cell_y = ((y_bound + next_y_cell) >> CELL_SIZE_FP);
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
    void clearView(const Graphics& g) const noexcept {        
        g.setColor(CEILING_COLOR);
        g.drawRectangle(RectStyle::FILL, VIEWPORT_LEFT, VIEWPORT_TOP, VIEWPORT_RIGHT, VIEWPORT_HORIZON);
        g.setColor(FLOOR_COLOR);
        g.drawRectangle(RectStyle::FILL, VIEWPORT_LEFT, VIEWPORT_HORIZON, VIEWPORT_RIGHT, VIEWPORT_BOTTOM);
        g.setColor(DarkRed);
        g.drawRectangle(RectStyle::OUTLINE, VIEWPORT_LEFT - 1, VIEWPORT_TOP - 1, VIEWPORT_RIGHT + 1, VIEWPORT_BOTTOM + 1); //debugging: draw a rect around the viewport so we can see overdraw.
    }
public:
    RayCaster() {
        buildLookupTables();
    } 
    void renderView(const Graphics& g, const int x, const int y, int view_angle) const noexcept {
        // This function casts out RAY_COUNT rays from the viewer and builds up the display based on the intersections with the walls.
        // The distance to the first horizontal and vertical edge is recorded. The closest intersection is the one used to draw the display.
        // The inverse of that distance is used to compute the height of the "sliver" of texture that will be drawn on the screen                
        clearView(g); //draw ceciling and floor first.
        if ((view_angle -= HALF_FOV_ANGLE) < 0) { // compute starting angle from player. Field of view is FOV angles, subtract half of that from the current view angle
            view_angle = ANGLE_360 + view_angle;
        }
        for (int ray = 0; ray < RAY_COUNT; ray++) {
            RayEnd xray = findVerticalWall(x, y, view_angle);  //cast a ray along the x-axis to intersect with vertical walls
            RayEnd yray = findHorizontalWall(x, y, view_angle); //cast a ray along the y-axis to intersect with horizontal walls
            SDL_Color color = WALL_BOUNDARY_COLOR;
            const float min_dist = (xray < yray) ? xray.distance : yray.distance;           
            if (xray < yray) { // there was a vertical wall closer than a horizontal wall                
                if (xray.intersection % CELL_SIZE > 1) {
                    color = VERTICAL_WALL_COLOR;                    
                }
                if constexpr (Cfg::hasMinimap()) {
                    MiniMap::drawLine(g, x, y, xray.boundary, xray.intersection, color);
                }
            }
            else { // must have hit a horizontal wall first                            
                if (yray.intersection % CELL_SIZE > 1) {
                    color = HORIZONTAL_WALL_COLOR;
                }
                if constexpr (Cfg::hasMinimap()) {
                    MiniMap::drawLine(g, x, y, yray.intersection, yray.boundary, color);
                }
            }
            // height of the sliver is based on the inverse distance to the intersection. Closer is bigger, so: height = 1/dist. However, 1 is too low a factor to look good. Thus the constant K which has been pre-multiplied into the view-filter lookup-table.
            const int height = static_cast<int>(cos_table[ray] / min_dist);
            const int clipped_height = (height > Cfg::VIEWPORT_HEIGHT) ? Cfg::VIEWPORT_HEIGHT : height;
            const int top = VIEWPORT_HORIZON - (clipped_height >> 1); //Optimization: height >> 1 == height / 2. slivers are drawn symmetrically around the viewport horizon.                       
            const int sliver_x = ray;       
            g.setColor(color);           
            g.drawVerticalLine(sliver_x, top, clipped_height - 1);              
            if (++view_angle >= ANGLE_360) {
                view_angle = 0; //reset angle back to zero
            }
        }
    }
};