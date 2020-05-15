#ifndef GAME_H
#define GAME_H

#include <random>
#include <string>
#include <unordered_map>
#include <thread>
#include <future>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"

class Game {
 public:
  // Constructor
  Game(std::size_t gridWidth, std::size_t gridHeight,
       Controller &&controller, Renderer &&renderer);

  // Public Methods
  void displayScoreBoard();
  void run();
  
  // Getters
  int getScore() const;
  int getHighScore() const;
  std::string getPlayerName() const;

  // Public Data
  const std::string kScoreBoardPath{"../assets/scoreboard.txt"};
  const std::size_t kFramesPerSecond{60};
  const std::size_t kTargetFrameDuration{1000 / kFramesPerSecond};

 private:

  // Private methods
  void placeFood_();
  void update_(bool &running);
  bool newPlayer_(std::string name);
  void updateScoreBoard_();
  void showGameBanner_();
  void getPlayerDetails_();
  void readScoreBoard_();
  void run_();
  void displayResult_();
  bool isValidScore_(std::string const &score);

  // Private data
  Snake        _snake;
  Controller   _gController;
  Renderer     _gRenderer;
  SDL_Point    _food;
  int          _score{0};
  int          _highScore{0};
  std::string  _playerName{};
  std::string  _topScorer{};
  bool         _disableLeaderBoardFeature{false};

  // For randomly placing food 
  std::random_device _dev;
  std::mt19937    _engine;
  std::uniform_int_distribution<int> _randomW;
  std::uniform_int_distribution<int> _randomH;

  // To store players and their scores
  std::unordered_map <std::string, std::string> _scoreboard{};
};

#endif