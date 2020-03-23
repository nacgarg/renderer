#include <SDL2/SDL.h>

#include <string>
#include <vector>

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
  int t = 0;

  mat4 worldToCamera = {{1.0, 0.0, 0.0, 0.0},
                        {0.0, 1.0, 0.0, 0.0},
                        {0.0, 0.0, 1.0, 0.0},
                        {0.0, 0.0, 4, 1.0}};

  vec2 projectPoint(vec3 point);

  void draw();

  void plot(vec2 pos, color c);
  void plotLine(vec2 start, vec2 end, color c);
  void plotLine3(vec3 start, vec3 end, color c);
  void plotTriLines(tri t, color c);
  void plotMesh(std::vector<tri> tris, color c);

  bool checkPos(vec2 p);
  std::vector<tri> loadOBJ(std::string file);
  std::vector<tri> test_object;
};