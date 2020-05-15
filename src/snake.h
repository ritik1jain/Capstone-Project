#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include "SDL.h"

class Snake {
 public:
  // Define Direction type
  enum class Direction { kUp, kDown, kLeft, kRight };

  // Constructor
  Snake(int gridWidth, int gridHeight)
      : _gridWidth(gridWidth),
        _gridHeight(gridHeight),
        headX(gridWidth / 2),
        headY(gridHeight / 2) {}

  // Public Methods
  void update();
  void growBody();
  bool snakeCell(int x, int y);

  // Public Data
  Direction direction = Direction::kUp;
  float speed{0.1f};
  int   size{1};
  bool  alive{true};
  float headX;
  float headY;
  std::vector<SDL_Point> body{};

 private:
  // Private methods
  void updateHead_();
  void updateBody_(SDL_Point &&currentHeadCell, SDL_Point &&previousHeadCell);

  // Private Data
  bool _growing{false};
  int  _gridWidth;
  int  _gridHeight;
};

#endif