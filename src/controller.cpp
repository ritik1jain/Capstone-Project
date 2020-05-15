#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"

void Controller::changeDirection_(Snake &snake, Snake::Direction input,
                                 Snake::Direction opposite) const {
  /*
   * Here the opposite direction is used to prevent the snake
   * to move into itself if it's size is more than 1
   */
  if (snake.direction != opposite || snake.size == 1) {
    snake.direction = input;
  }
  return;
}

/*
 * Define game controls
 * If user closes the game window, set running as false to exit the game loop
 * If user presses left arrow key or 'a' change the snake direction to left
 * If user presses right arrow key or 'd' change the snake direction to right
 * If user presses up arrow key or 'w' change the snake direction to up
 * If user presses down arrow key or 's' change the snake direction to down
 * If user presses q, set running as false to exit the game loop
 */
void Controller::handleInput(bool &running, Snake &snake) const {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      running = false;
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_UP:
        case SDLK_w:
          changeDirection_(snake, Snake::Direction::kUp,
                           Snake::Direction::kDown);
          break;

        case SDLK_DOWN:
        case SDLK_s:
          changeDirection_(snake, Snake::Direction::kDown,
                           Snake::Direction::kUp);
          break;

        case SDLK_LEFT:
        case SDLK_a:
          changeDirection_(snake, Snake::Direction::kLeft,
                           Snake::Direction::kRight);
          break;

        case SDLK_RIGHT:
        case SDLK_d:
          changeDirection_(snake, Snake::Direction::kRight,
                           Snake::Direction::kLeft);
          break;

        case SDLK_q:
          running = false;
          break;
      }
    }
  }
}