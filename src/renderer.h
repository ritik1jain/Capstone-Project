#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <string>
#include "SDL.h"
#include "SDL_mixer.h"
#include "snake.h"

class Renderer {
 public:
  // Define SoundEffect Type
  enum class SoundEffect { kbiteSound, kdeadSnakeSound, kNoSound };

  // Constructor
  Renderer(const std::size_t screenWidth, const std::size_t screenHeight,
           const std::size_t gridWidth, const std::size_t gridHeight);

  // Destructor
  ~Renderer();

  /*
   * Rule of 5 implementation
   * Adopting a "No Copy, Only Move" memory management policy
   */
  Renderer(const Renderer &) = delete;             // Delete copy constructor
  Renderer &operator=(const Renderer &) = delete;  // Delete copy assignment operator

  // Move Constructor
  Renderer(Renderer &&source);

  // Move Assignment Operator
  Renderer &operator=(Renderer &&source);

  // Public methods
  void render(Snake const &snake, SDL_Point const &food);
  void updateWindowTitle(std::string name, int score, bool withHighScore, int highScore = 0);
  void play(SoundEffect sound);

  // Public data
  const std::string kBiteSoundPath{"../assets/biting-sound.wav"};
  const std::string kDeadSoundPath{"../assets/dead.wav"};
  SoundEffect soundEffect{SoundEffect::kNoSound};

 private:
  SDL_Window   *_sdlWindowPtr;
  SDL_Renderer *_sdlRendererPtr;
  Mix_Chunk    *_biteSoundPtr;   // To store biting sound effect
  Mix_Chunk    *_deadSoundPtr;   // To store dead snake sound effect

  std::size_t _screenWidth;
  std::size_t _screenHeight;
  std::size_t _gridWidth;
  std::size_t _gridHeight;
};

#endif