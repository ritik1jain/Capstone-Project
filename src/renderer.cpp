#include "renderer.h"
#include <iostream>
#include <string>

Renderer::Renderer(const std::size_t screenWidth,
                   const std::size_t screenHeight,
                   const std::size_t gridWidth, 
                   const std::size_t gridHeight)
    : _screenWidth(screenWidth),
      _screenHeight(screenHeight),
      _gridWidth(gridWidth),
      _gridHeight(gridHeight)  {

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Initialize SDL Mixer
  if (Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0) {
    std::cerr << "SDL_mixer could not initialize.\n";
    std::cerr << "SDL_mixer Error: " << Mix_GetError() << "\n";
  }

  // Create Window
  _sdlWindowPtr = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED, _screenWidth,
                                   _screenHeight, SDL_WINDOW_SHOWN);

  if (nullptr == _sdlWindowPtr) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create Renderer
  _sdlRendererPtr = SDL_CreateRenderer(_sdlWindowPtr, -1, SDL_RENDERER_ACCELERATED);
  if (nullptr == _sdlRendererPtr) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Load bite sound effect
  _biteSoundPtr = Mix_LoadWAV(kBiteSoundPath.c_str());
  if (nullptr == _biteSoundPtr) {
    std::cerr << "Failed to load biting sound effect.\n";
    std::cerr << "SDL_mixer Error: " << Mix_GetError() << "\n";
  }

  // Load dead snake sound effect
  _deadSoundPtr = Mix_LoadWAV(kDeadSoundPath.c_str());
  if (nullptr == _deadSoundPtr) {
    std::cerr << "Failed to load dead snake sound effect.\n";
    std::cerr << "SDL_mixer Error: " << Mix_GetError() << "\n";
  }
}

Renderer::~Renderer() {
  Mix_FreeChunk(_deadSoundPtr);
  Mix_FreeChunk(_biteSoundPtr);
  SDL_DestroyRenderer(_sdlRendererPtr);
  SDL_DestroyWindow(_sdlWindowPtr);
  Mix_Quit();
  SDL_Quit();
}

// Move Constructor
Renderer::Renderer(Renderer &&source) {
  _sdlWindowPtr   = source._sdlWindowPtr;
  _sdlRendererPtr = source._sdlRendererPtr;
  _biteSoundPtr   = source._biteSoundPtr;
  _deadSoundPtr   = source._deadSoundPtr;
  _screenWidth    = source._screenWidth;
  _screenHeight   = source._screenHeight;
  _gridWidth      = source._gridWidth;
  _gridHeight     = source._gridHeight;
  soundEffect     = source.soundEffect;

  // Invalidating source after move operation
  source._sdlWindowPtr   = nullptr;
  source._sdlRendererPtr = nullptr;
  source._biteSoundPtr   = nullptr;
  source._deadSoundPtr   = nullptr;
  source._screenWidth    = 0;
  source._screenHeight   = 0;
  source._gridWidth      = 0;
  source._gridHeight     = 0;
  source.soundEffect     = SoundEffect::kNoSound;
}

// Move Assignment Operator
Renderer &Renderer::operator=(Renderer &&source) {
  if (this == &source) { return *this; }  // To handle self assignment scenario

  _sdlWindowPtr   = source._sdlWindowPtr;
  _sdlRendererPtr = source._sdlRendererPtr;
  _biteSoundPtr   = source._biteSoundPtr;
  _deadSoundPtr   = source._deadSoundPtr;
  _screenWidth    = source._screenWidth;
  _screenHeight   = source._screenHeight;
  _gridWidth      = source._gridWidth;
  _gridHeight     = source._gridHeight;
  soundEffect     = source.soundEffect;

  // Invalidating source after move operation
  source._sdlWindowPtr   = nullptr;
  source._sdlRendererPtr = nullptr;
  source._biteSoundPtr   = nullptr;
  source._deadSoundPtr   = nullptr;
  source._screenWidth    = 0;
  source._screenHeight   = 0;
  source._gridWidth      = 0;
  source._gridHeight     = 0;
  source.soundEffect     = SoundEffect::kNoSound;

  return *this;
}

void Renderer::render(Snake const &snake, SDL_Point const &food) {
  SDL_Rect block;
  block.w = _screenWidth / _gridWidth;
  block.h = _screenHeight / _gridHeight;

  // Clear screen
  SDL_SetRenderDrawColor(_sdlRendererPtr, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(_sdlRendererPtr);

  // Render food
  SDL_SetRenderDrawColor(_sdlRendererPtr, 0xFF, 0xCC, 0x00, 0xFF);  // yellow
  block.x = food.x * block.w;
  block.y = food.y * block.h;
  SDL_RenderFillRect(_sdlRendererPtr, &block);

  // Render snake's body
  SDL_SetRenderDrawColor(_sdlRendererPtr, 0xFF, 0xFF, 0xFF, 0xFF);  // white
  for (SDL_Point const &point : snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(_sdlRendererPtr, &block);
  }

  // Render snake's head
  block.x = static_cast<int>(snake.headX) * block.w;
  block.y = static_cast<int>(snake.headY) * block.h;
  if (snake.alive) {
    SDL_SetRenderDrawColor(_sdlRendererPtr, 0x00, 0x7A, 0xCC, 0xFF);  // blue
  } else {
    SDL_SetRenderDrawColor(_sdlRendererPtr, 0xFF, 0x00, 0x00, 0xFF);  // red
  }
  SDL_RenderFillRect(_sdlRendererPtr, &block);

  // Update Screen
  SDL_RenderPresent(_sdlRendererPtr);
}

void Renderer::updateWindowTitle(std::string name, int score, bool withHighScore, int highScore) {
  std::string title{};
  if (withHighScore) {
    title = std::string{"Player: " + name + "   " + " Score: " + std::to_string(score) + "   " + "Highest Score: " + std::to_string(highScore)};
  } else {
    title = std::string{"Player: " + name + "       " + " Score: " + std::to_string(score)};
  }
  SDL_SetWindowTitle(_sdlWindowPtr, title.c_str());
}

void Renderer::play(SoundEffect sound) {
  switch (sound) {
    case SoundEffect::kbiteSound:
       Mix_PlayChannel(-1, _biteSoundPtr, 0);
       break;
    case SoundEffect::kdeadSnakeSound:
       Mix_PlayChannel(-1, _deadSoundPtr, 0);
       break;
    default:
       // Play no sound
       break;
  }
}
