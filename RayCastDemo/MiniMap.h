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
        g._setcolor(color);
        g.drawLine(x1, y1, x2, y2);
    }
    void render(const Graphics& g)  noexcept {
        if constexpr (false == Cfg::hasMinimap()) { return; }        
        for (int row = 0; row < WORLD_ROWS; row++) {
            const auto top = (row * SCALED_CELL_SIZE);
            const auto bottom = top + SCALED_CELL_SIZE - 1;
            for (int column = 0; column < WORLD_COLUMNS; column++) {
                const auto left = MAP_LEFT + (column * SCALED_CELL_SIZE);
                const auto right = left + SCALED_CELL_SIZE - 1;
                const auto block = WORLD[row][column];
                if (block == 0) {
                    g._setcolor(White);
                    g._rectangle(RectStyle::OUTLINE, left, top, right, bottom);
                }
                else {
                    g._setcolor(DarkGreen);
                    g._rectangle(RectStyle::FILL, left, top, right, bottom);
                }
            }
        }
    }
}