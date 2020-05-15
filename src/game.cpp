#include <iostream>
#include <fstream>
#include <sstream>
#include "game.h"
#include "SDL.h"

Game::Game(std::size_t gridWidth, std::size_t gridHeight,
           Controller &&controller, Renderer &&renderer)
    : _snake(gridWidth, gridHeight),
      _gController(std::move(controller)),
      _gRenderer(std::move(renderer)),
      _engine(_dev()),
      /*
       * Setting the range from 1 to grid dimension - 1
       * so that the food does not get generated outside the grid
       */
      _randomW(1, static_cast<int>(gridWidth)-1),  
      _randomH(1, static_cast<int>(gridHeight)-1) {
  placeFood_();
}

// Implements Main Game Loop
void Game::run_() {
  Uint32 titleTimestamp = SDL_GetTicks();
  Uint32 frameStart;
  Uint32 frameEnd;
  Uint32 frameDuration;
  bool running = true;

  while (running) {
    frameStart = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    _gController.handleInput(running, _snake);
    update_(running);
    _gRenderer.render(_snake, _food);

    frameEnd = SDL_GetTicks();

    /*
     * Keep track of how long each frame loop through 
     * the input/update/render cycle
     */
    frameDuration = frameEnd - frameStart;

    // After every second, update the window title.
    if (frameEnd - titleTimestamp >= 1000) {
      if (_disableLeaderBoardFeature) {
        _gRenderer.updateWindowTitle(_playerName, _score, false);
      } else {
        _gRenderer.updateWindowTitle(_playerName, _score, true, _highScore);
      }
      titleTimestamp = frameEnd;
    }

    /*
     * If the time for this frame is too small (i.e. frameDuration is
     * smaller than the target ms_per_frame), delay the loop to
     * achieve the correct frame rate.
     */
    if (frameDuration < kTargetFrameDuration) {
      SDL_Delay(kTargetFrameDuration - frameDuration);
    }
  }
}

void Game::placeFood_() {
  int x, y;
  while (true) {
    x = _randomW(_engine);
    y = _randomH(_engine);
    /*
     * Check that the location is not occupied by a snake item 
     * before placing food.
     */
    if (!_snake.snakeCell(x, y)) {
      _food.x = x;
      _food.y = y;
      return;
    }
  }
}

void Game::update_(bool &running) {
  if (!_snake.alive) {
    _gRenderer.play(Renderer::SoundEffect::kdeadSnakeSound);
    running = false;
    SDL_Delay(1000); // Adding 1 sec delay to prevent a quick exit
                     // so that dead snake sound can finish playing
    return;
  }

  _snake.update();

  int newX = static_cast<int>(_snake.headX);
  int newY = static_cast<int>(_snake.headY);

  // Check if there's food over here
  if (_food.x == newX && _food.y == newY) {
    _gRenderer.play(Renderer::SoundEffect::kbiteSound);
    _score += 10;
    placeFood_();
    // Grow snake and increase speed.
    _snake.growBody();
    _snake.speed += 0.02;
  }
}

// Getters definition
int Game::getScore() const              { return _score;      }
int Game::getHighScore() const          { return _highScore;  }
std::string Game::getPlayerName() const { return _playerName; }

// Display an ASCII Snake Game Banner Art
void Game::showGameBanner_() {
  std::cout << "  _________              __           " << "\n";
  std::cout << " /   _____/ ____ _____  |  | __ ____  " << "\n";
  std::cout << " \\_____  \\ /    \\\\__  \\ |  |/ // __ \\ " << "\n";
  std::cout << " /        \\   |  \\/ __ \\|    <\\  ___/ " << "\n";
  std::cout << "/_______  /___|  (____  /__|_ \\\\___  >" << "\n";
  std::cout << std::endl;
}

// Check whether the score in an entry in leaderboard file is valid
bool Game::isValidScore_(std::string const &score) {
  std::string::const_iterator it = score.begin();

  // A valid score string should only be composed of digits
  while (it != score.end() && std::isdigit(*it)) {
     ++it;
  }
  return !score.empty() && it == score.end();
}

// Read the scoreboard file and store the data to game memory
void Game::readScoreBoard_() {
  std::ifstream scoreBoardFile(kScoreBoardPath);
  if (scoreBoardFile.is_open()) {
    std::string entry{};
    while (std::getline(scoreBoardFile, entry)) {
      std::string player{};
      std::string score{};
      int pScore = 0;
      std::istringstream stream(entry);
      stream >> player >> score;
      if (player.empty() || score.empty()) { 
        // One of the entries in file is corrupted
        _disableLeaderBoardFeature = true; 
      }
      if (isValidScore_(score)) {
        _scoreboard[player] = score;
        pScore = std::stoi(score);
        if (pScore > _highScore) { 
          _highScore = pScore; 
          _topScorer = player;
        }
      } else { 
        // Score value in this entry is corrupted
        _disableLeaderBoardFeature = true; 
      }
    }
    scoreBoardFile.close();
  } else {
    // File is empty or missing
    _disableLeaderBoardFeature = true;
  }
}

// Determine whether the player is new to the game
bool Game::newPlayer_(std::string name) {
  return (_scoreboard.find(name) == _scoreboard.end()); 
}

// Get the user inputs needed to personalize the game
void Game::getPlayerDetails_() {
  char pResponse;  // To get the player pressed key

  // Adding a 10 millisecond delay to ensure the other thread finish reading the scoreboard file
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  /*
   * Get player name
   * If new player, introduce him/her to the game controls
   * If existing player, let him/her know the old score
   * Showcase the current scoreboard leader and the score to beat
   * Give the player chance to enter a different name if chosen name
   * is already in the system
   */
  while(true) {
    std::cout << "Enter player name: ";
    std::cin >> _playerName;
    if (!_disableLeaderBoardFeature) {  // Check if leaderboard file reading was successful
      if (newPlayer_(_playerName)) {
        std::cout << "\nNamaste " << _playerName << "\U0001F64F  Welcome to the Classic Snake Game!!" << "\n";
        std::cout << "Since you are a new player, allow me to introduce you to the game controls!" << "\n";
        std::cout << "* To control the snake, you can either use the arrow keys or the 'w','a','s','d' keys." << "\n";
        std::cout << "* To quit the game, you can either close the game window or press 'q'" << "\n\n";
        std::cout << "Current Scoreboard Leader is " << _topScorer << " with a score of " << _highScore << "\n"; 
        std::cout << "When you are ready to play, press 's' and enter to start the game!!!" << std::endl;
        std::cin >> pResponse;
        if (pResponse == 's') { break; } else { std::cerr << "Invalid entry!\n"; }
      } else {
        std::cout << "Welcome back, " << _playerName << "!! Came back to improve your score?" << "\n";
        std::cout << "Your earlier score was: " << _scoreboard[_playerName] << "\n"; 
        std::cout << "Current Scoreboard Leader is " << _topScorer << " with a score of " << _highScore << "\n\n"; 
        std::cout << "If you are a new player and your chosen player name seems to be already taken,\n";
        std::cout << "then press 'c' and enter to change your player name. Otherwise, press 's' and enter to start the game!!!\n";
        std::cin >> pResponse;
        switch (pResponse) {
          case 's': return;
          case 'c': break;
          default: std::cerr << "Invalid entry!\n";
        }
      }
    } else {
      /* 
       * Either the scoreboard file is empty or absent or corrupted. 
       * Let the game proceed by disabling the leaderboard feature.
       */
      std::cout << "\nNamaste " << _playerName << "\U0001F64F  Welcome to the Classic Snake Game!!" << "\n";
      std::cout << "* To control the snake, you can either use the arrow keys or the 'w','a','s','d' keys." << "\n";
      std::cout << "* To quit the game, you can either close the game window or press 'q'" << "\n\n";
      std::cout << "When you are ready to play, press 's' to start the game!!!" << std::endl;
      std::cin >> pResponse;
      if (pResponse == 's') { break; } else { std::cerr << "Invalid entry!\n"; }
    }
  }
}

void Game::displayScoreBoard() {
  /*
   * Fix the length of the fields in the scoreboard table
   * so that scoreboard printout looks neat and aligned
   */
  int maxNameLen  = 8;
  int maxScoreLen = 3;
  int reqSpacesForName  = 0;  // Required number of spaces to be appended, to adjust name length
  int reqSpacesForScore = 0;  // Required number of spaces to be appended, to adjust score length

  std::string name, score;

  std::cout << std::endl;
  std::cout << "    SCOREBOARD    " << "\n";
  std::cout << "------------------" << "\n";

  for ( auto it = _scoreboard.begin(); it != _scoreboard.end(); ++it ) {
     bool bTrimName = false;  // To determine whether or not to trim the player name
                              // to fit the name nicely inside scoreboard table

     reqSpacesForName = maxNameLen - (it->first).length();
     if (reqSpacesForName < 0) { bTrimName = true; } 

     reqSpacesForScore = maxScoreLen - (it->second).length();

     // Trim the player name or add required number of spaces to align the name in the field
     if (bTrimName) { name = (it->first).substr(0,8); }
     else { 
       name = it->first;
       while(reqSpacesForName != 0) { 
         name += " "; 
         reqSpacesForName--; 
       }
     }

     // Add the required number of spaces to align the score neatly inside score field
     score = it->second;
     while (reqSpacesForScore != 0) {
       score += " ";
       reqSpacesForScore--;
     }

     // Display the scoreboard table row
     std::cout << "| " << name << " | " << score << " |" << "\n";
  }
  std::cout << "------------------" << std::endl;
}

// Add the current player's entry in the scoreboard.txt file
void Game::updateScoreBoard_() {
  _scoreboard[_playerName] = std::to_string(_score);  // Update scoreboard in memory so that
                                                      // displayScoreBoard() will include the latest entry

  // Update scoreboard.txt file
  std::ofstream scoreBoardFile;
  scoreBoardFile.open(kScoreBoardPath, std::ios_base::out | std::ios_base::app);

  if (scoreBoardFile.is_open()) {
    std::string entry{_playerName + " " + std::to_string(_score) + "\n"};
    scoreBoardFile << entry;
    scoreBoardFile.close();
  }
}

// Display the result of the game
void Game::displayResult_() {
  std::cout << "GAME OVER!" << "\n";
  int score = getScore();
  int highScore = getHighScore();
  std::cout << "Your score: " << score << "\n";
  if (!_disableLeaderBoardFeature) {  // Check if scoreboard file reading was successful
                                      // If not disable the leaderboard feature
    if (score > highScore) {
      std::cout << "Congratulations! You are now the Scoreboard leader! \U0001F4AA" << "\n";
    }
  }
}

void Game::run() {
  showGameBanner_();
  // Spawn threads to read the scoreboard file and to get the player details concurrently 
  std::thread t1 = std::thread(&Game::readScoreBoard_, this);
  std::thread t2 = std::thread(&Game::getPlayerDetails_, this); 
  t1.join();
  t2.join();
  run_();
  displayResult_();
  if (!_disableLeaderBoardFeature) {  // Display scoreboard only if the scoreboard.txt file could be
                                      // properly read. Otherwise disable the leaderboard feature
    updateScoreBoard_();
    displayScoreBoard();
  }
}