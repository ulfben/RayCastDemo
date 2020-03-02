#pragma once
static constexpr auto WORLD_ROWS = 16;
static constexpr auto WORLD_COLUMNS = WORLD_ROWS;
static constexpr auto WORLD_SIZE = (WORLD_ROWS * Cfg::CELL_SIZE); //width and height of the gameworld    

static constexpr auto FIRST_VALID_CELL = 1; //to shortcut collision testing we can check against the map boundary walls. Helps with mouse interaction and for Q&D respawn when stuck in a wall.
static constexpr auto LAST_VALID_CELL = WORLD_ROWS-2; //0 == wall, 15 == wall. so if our position is below or above the valid spaces, we can bail without performing lookup.
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

inline constexpr bool isWall(int x, int y) noexcept {
    const auto LAST_ROW = (WORLD_ROWS - 1);
   // y = LAST_ROW - y;
    if (x < FIRST_VALID_CELL || y < FIRST_VALID_CELL
        || x > LAST_VALID_CELL || y > LAST_VALID_CELL) {
        return true;
    }
    return (WORLD[y][x] != 0);
}

static_assert(WORLD_ROWS == WORLD_COLUMNS && "The map must be square - WORLD_ROWS == WORLD_COLUMNS");
static_assert(Utils::isPowerOfTwo(WORLD_SIZE) && "World width and height must be a power-of-2");