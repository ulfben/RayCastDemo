#pragma once
#include "Config.h"
#include "Graphics.h"

namespace MiniMap {    
    static constexpr auto MAP_HEIGHT = WORLD_SIZE >> Cfg::MAP_SCALE_FACTOR; //target width and height of the minimap, in pixels. 
    static constexpr auto SCALED_CELL_SIZE = Cfg::CELL_SIZE >> Cfg::MAP_SCALE_FACTOR;
    static constexpr auto MAP_LEFT = VIEWPORT_RIGHT;
    
    void drawLine(const Graphics& g, int x1, int y1, int x2, int y2, const SDL_Color& color) noexcept {  
        x1 = MAP_LEFT + (x1 >> Cfg::MAP_SCALE_FACTOR);
        y1 = (y1 >> Cfg::MAP_SCALE_FACTOR);
        x2 = MAP_LEFT + (x2 >> Cfg::MAP_SCALE_FACTOR);
        y2 = (y2 >> Cfg::MAP_SCALE_FACTOR);
        g.setColor(color);
        g.drawLine(x1, y1, x2, y2);
    }
    void renderMap(const Graphics& g)  noexcept {
        if constexpr (false == Cfg::hasMinimap()) { return; }        
        for (int row = 0; row < WORLD_ROWS; row++) {
            const auto top = (row * SCALED_CELL_SIZE);
            const auto bottom = top + SCALED_CELL_SIZE - 1;
            for (int column = 0; column < WORLD_COLUMNS; column++) {
                const auto left = MAP_LEFT + (column * SCALED_CELL_SIZE);
                const auto right = left + SCALED_CELL_SIZE - 1;
                const auto block = isWall(column, row);
                if (!block) {
                    g.setColor(White);
                    g.drawRectangle(RectStyle::OUTLINE, left, top, right, bottom);
                }
                else {
                    g.setColor(DarkGreen);
                    g.drawRectangle(RectStyle::FILL, left, top, right, bottom);
                }
            }
        }
    }
}