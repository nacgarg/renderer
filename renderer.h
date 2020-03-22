#include <SDL2/SDL.h>

#include "types.h"
#define DEFAULT_SIZE 600
class Renderer {
 public:
  Renderer(int width = DEFAULT_SIZE, int height = DEFAULT_SIZE) : w(width), h(height) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(w, h, 0, &window, &renderer);
  }

  ~Renderer() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
  }

  void start() {
    while (1) {
      if (SDL_PollEvent(&event) && event.type == SDL_QUIT) break;
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      SDL_RenderClear(renderer);  // black background

      draw();
      SDL_RenderPresent(renderer);
    }
  }

 private:
  SDL_Event event;
  SDL_Renderer *renderer;
  SDL_Window *window;
  int w;
  int h;

  void plot(vec2 pos, color c) {
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderDrawPoint(renderer, pos.x, pos.y);
  }

  void draw() {
    // here is where we actually plot stuff
    for (int i = 0; i < w; ++i) {
      plot({i, i}, {255, 255, 255, 255});
    }
  }
};