#include <SDL2/SDL.h>

#include <string>
#include <vector>
#include <deque>

#include "types.h"
#define DEFAULT_SIZE 500
class Renderer {
 public:
  Renderer(int width = DEFAULT_SIZE, int height = DEFAULT_SIZE);
  ~Renderer();

  void start();

 private:
  SDL_Event event;
  SDL_Surface *screen;
  SDL_Window *window;
  int w;
  int h;
  int t = 0;

  mat4 worldToCamera = {{1.0, 0.0, 0.0, 0.0},
                        {0.0, 1.0, 0.0, 0.0},
                        {0.0, 0.0, 1.0, 0.0},
                        {0.0, 0.0, -10, 1.0}};

  vec3 projRaster(vec3 point);
  vec3 projCamera(vec3 point);
  vec2 projectPoint(vec3 point);


  void draw();

  void plot(vec2 pos, color c);
  void plotLine(vec2 start, vec2 end, color c);
  void plotLine3(vec3 start, vec3 end, color c);
  void plotTriLines(tri t, color c);
  void plotMesh(std::vector<tri> tris, color c);
  void plotWireframe(std::vector<tri> tris, color c);
  void plotTriRaster(tri t, color c);

  bool checkPos(vec2 p);
  std::vector<tri> loadOBJ(std::string file);
  std::vector<tri> testObject;
  std::vector<tri> cowObject;
  std::deque<unsigned int> lastFrameTimes;

  std::vector<float> depthBuffer;

  float nearClipPlane = 0.01;
  float hFov = 80 * 3.1415 / 180.0;
};