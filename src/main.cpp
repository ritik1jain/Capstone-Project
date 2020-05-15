#include <iostream>
#include <thread>
#include <memory>
#include "controller.h"
#include "game.h"
#include "renderer.h"

int main() {
  // Define Game constants
  constexpr std::size_t kScreenWidth{640};
  constexpr std::size_t kScreenHeight{640};
  constexpr std::size_t kGridWidth{32};
  constexpr std::size_t kGridHeight{32};

  // Create Renderer instance
  Renderer renderer(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight);

  // Create Controller instance
  Controller controller;

  // Create Game instance
  Game game(kGridWidth, kGridHeight, std::move(controller), std::move(renderer));

  // Run the Game
  game.run();

  return 0;
}