#pragma once
#include <iostream>
#include "Config.h"
#include "LevelData.h"
#include "MiniMap.h"
#include "src/InputManager.h"
struct ViewPoint {
	int x = 0, y = 0, angle = 0;
	float dx = 0.0f, dy = 0.0f;
	ViewPoint(int cellx, int celly, int startAngle) : angle{ startAngle } {
		centerInCell(cellx, celly);
	};
	void update(const InputManager& input) {
		dx = 0.0f;
		dy = 0.0f;
		if (input.isButtonDown(MouseButton::LEFT)) {
			const int mouseX = input.mouseX();
			const int mouseY = input.mouseY();
			const int mouseCellX =(mouseX / MiniMap::SCALED_CELL_SIZE);
			const int mouseCellY = mouseY / MiniMap::SCALED_CELL_SIZE;
			std::cout << "x:" << mouseX << " y:" << mouseY << " / cellx:" << mouseCellX << " celly:" << mouseCellY << "\n";
			if (!isWall(mouseCellX, mouseCellY)) {
				centerInCell(mouseCellX, mouseCellY);
				return;
			}
		}

		if (input.isAnyKeyDown(Cfg::rotateRight)) {
			if ((angle -= Cfg::ROTATION_SPEED) < ANGLE_0) {
				angle = ANGLE_360;
			}
		}
		else if (input.isAnyKeyDown(Cfg::rotateLeft)) {
			if ((angle += Cfg::ROTATION_SPEED) >= ANGLE_360) {
				angle = ANGLE_0;
			}
		}
		if (input.isAnyKeyDown(Cfg::moveForward)) {
			dx = cos(angle * ANGLE_TO_RADIANS) * Cfg::WALK_SPEED;
			dy = sin(angle * ANGLE_TO_RADIANS) * Cfg::WALK_SPEED;		

		}
		else if (input.isAnyKeyDown(Cfg::moveBackward)) {
			dx = -cos(angle * ANGLE_TO_RADIANS) * Cfg::WALK_SPEED;
			dy = -sin(angle * ANGLE_TO_RADIANS) * Cfg::WALK_SPEED;			
		}
		x += static_cast<int>(dx);
		y += static_cast<int>(dy);
	}

	inline constexpr void centerInCell(int cellx, int celly) noexcept {
		x = cellx * CELL_SIZE + (CELL_SIZE >> 1);
		y = celly * CELL_SIZE + (CELL_SIZE >> 1);
	}

	void checkCollisions() noexcept {
		// test if user has bumped into a wall i.e. test if there is a cell within the direction of motion, if so back up!                               
		const int x_cell = x / CELL_SIZE;
		const int y_cell = y / CELL_SIZE;
		if (isWall(x_cell, y_cell)) { //standing inside a wall, somehow. 
			return centerInCell(FIRST_VALID_CELL, FIRST_VALID_CELL);
		}

		const int x_sub_cell = x % CELL_SIZE; // compute position within the cell
		const int y_sub_cell = y % CELL_SIZE;
		if (dx > 0 && isWall(x_cell + 1, y_cell)) {// moving right, towards a wall
			if (x_sub_cell > (CELL_SIZE - OVERBOARD)) {
				x -= (x_sub_cell - (CELL_SIZE - OVERBOARD)); // back player up amount they stepped over the line
			}
		}
		else if (dx < 0 && isWall(x_cell - 1, y_cell)) {// moving left, towards a wall
			if (x_sub_cell < (OVERBOARD)) {
				x += (OVERBOARD - x_sub_cell);
			}
		}

		if (dy > 0 && isWall(x_cell, y_cell + 1)) { // moving up
			if (y_sub_cell > (CELL_SIZE - OVERBOARD)) {
				y -= (y_sub_cell - (CELL_SIZE - OVERBOARD));
			}
		}
		else if (dy < 0 && isWall(x_cell, y_cell - 1)) {// moving down            
			if (y_sub_cell < (OVERBOARD)) {
				y += (OVERBOARD - y_sub_cell);
			}
		}
	}
};