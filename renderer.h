#include <SDL2/SDL.h>

#include "types.h"
#define DEFAULT_SIZE 600
class Renderer {
 public:
  Renderer(int width = DEFAULT_SIZE, int height = DEFAULT_SIZE);
  ~Renderer();

  void start();

 private:
  SDL_Event event;
  SDL_Renderer *renderer;
  SDL_Window *window;
  int w;
  int h;

  void draw();

  void plot(vec2 pos, color c);
  void plotLine(vec2 start, vec2 end, color c);
};