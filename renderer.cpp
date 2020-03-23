#include "renderer.h"

#include <fstream>
#include <iostream>

#define DEFAULT_SIZE 600

Renderer::Renderer(int width, int height) : w(width), h(height) {
  SDL_Init(SDL_INIT_VIDEO);
  window = SDL_CreateWindow("Renderer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            w, h, 0);
  screen = SDL_GetWindowSurface(window);
}

Renderer::~Renderer() {
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Renderer::start() {
  testObject = loadOBJ("test.obj");
  while (1) {
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) break;
    switch (event.type) {
      case SDL_KEYDOWN:
        // printf("Key press detected: %s\n", SDL_GetKeyName(event.key.keysym.sym));
        if (event.key.keysym.sym == SDLK_w) {
          worldToCamera.l.z -= 0.05;
        }
        if (event.key.keysym.sym == SDLK_a) {
          worldToCamera.l.x -= 0.05;
        }
        if (event.key.keysym.sym == SDLK_s) {
          worldToCamera.l.z += 0.05;
        }
        if (event.key.keysym.sym == SDLK_d) {
          worldToCamera.l.x += 0.05;
        }
        break;

      default:
        break;
    }

    unsigned int start = SDL_GetTicks();

    SDL_memset(screen->pixels, 0, screen->h * screen->pitch);  // black background

    draw();

    SDL_UpdateWindowSurface(window);
    unsigned int msPerFrame = SDL_GetTicks() - start;
    lastFrameTimes.push_front(msPerFrame);
    if (lastFrameTimes.size() > 100) {
      lastFrameTimes.pop_back();
    }
    if (t % 100 == 0) {
      float sum = 0;
      for (auto a : lastFrameTimes) {
        sum += a;
      }
      sum /= lastFrameTimes.size();
      std::cout << "FPS: " << 1000.0 / sum << "\n";
    }

    ++t;
  }
}

void Renderer::plot(vec2 pos, color c) {
  if (!checkPos(pos)) return;
  auto ptr = (unsigned char*)screen->pixels;
  ptr += ((int)pos.y * screen->pitch + ((int)pos.x * sizeof(unsigned int)));
  *((unsigned int*)ptr) = SDL_MapRGBA(screen->format, c.r, c.g, c.b, c.a);
}

void Renderer::draw() {
  // here is where we actually plot stuff
  //   plotLine({0, 0}, {500, 100}, {255, 255, 255, 255});
  //   plotLine({500, 100}, {300, 550}, {255, 255, 255, 255});
  // plotLine({300, 550}, {0, 0}, {123, 123, 255, 255});
  // plotLine3({5, 5, -1}, {-5, -5, -2}, {50, 50, 255, 255});
  //   plotLine3({t / 100 - 100, -10, -0.1}, {t / 100 - 100, -10, -10}, {123, 255, 123,
  //   255});

  float theta = (float)t / 1000;
  mat4 rx = rotX(theta);  // rotation matrix around X axis
  mat4 ry = rotY(theta);  // rotation around Y axis
  mat4 r = mul(rx, ry);

  vec3 a, b, c, d;
  a = mulP(r, {0.0, 0.0, 0.0});
  b = mulP(r, {1., 0.0, 0.0});
  c = mulP(r, {0.5, 0.0, -1.});
  d = mulP(r, {0.5, 1.0, -0.5});
  std::vector<tri> tetrahedron = {{a, b, c}, {a, b, d}, {a, c, d}, {b, c, d}};
  std::vector<tri> rotated(testObject.size());
  int i = 0;
  for (tri t : testObject) {
    rotated[i++] = {mulP(r, t.a), mulP(r, t.b), mulP(r, t.c)};
  }
  plotMesh(tetrahedron, {123, 123, 255, 255});
  plotMesh(rotated, {255, 50, 255, 255});
}

void Renderer::plotLine(vec2 start, vec2 end, color c) {
  // We have a couple different cases here
  float x0 = start.x;
  float x1 = end.x;
  float y0 = start.y;
  float y1 = end.y;

  float dx = x1 - x0;
  float dy = y1 - y0;

  float dErr;
  float err = 0;
  int y;
  int x;

  std::vector<SDL_Point> pts;

  // Case 1: not steep (dx > dy)
  if (abs(dx) > abs(dy)) {
    if (x0 > x1) {
      std::swap(x0, x1);
      std::swap(y0, y1);
    }
    dx = x1 - x0;
    dy = y1 - y0;
    dErr = abs(dy / dx);

    y = y0;
    for (x = x0; x < x1; ++x) {
      plot({x, y}, c);
      err += dErr;
      if (err > 0.5) {
        if (dy < 0) {
          y--;
        } else {
          y++;
        }
        err -= 1;
      }
    }
  }

  // Case 2: steep (dy > dx)
  else {
    if (y0 > y1) {
      std::swap(x0, x1);
      std::swap(y0, y1);
    }
    dx = x1 - x0;
    dy = y1 - y0;
    dErr = abs(dx / dy);

    x = x0;
    for (y = y0; y < y1; ++y) {
      plot({x, y}, c);
      err += dErr;
      if (err > 0.5) {
        if (dx < 0) {
          x--;
        } else {
          x++;
        }
        err -= 1;
      }
    }
  }
}

vec2 Renderer::projectPoint(vec3 p) {
  // compute position in camera space
  vec3 pCamera = mulP(worldToCamera, p);
  vec2 pScreen = {pCamera.x / -pCamera.z, pCamera.y / -pCamera.z};
  float ww = 1;
  float hh = 1;
  vec2 pNorm = {(pScreen.x + ww / 2.0) / ww, (pScreen.y + hh / 2.0) / hh};
  float pixelWidth = w;
  float pixelHeight = h;
  vec2 pRaster = {pNorm.x * pixelWidth, (1.0 - pNorm.y) * pixelHeight};
  return pRaster;
}

void Renderer::plotLine3(vec3 start, vec3 end, color c) {
  vec2 startProj = projectPoint(start);
  vec2 endProj = projectPoint(end);
  //   std::cout << startProj.x << " " << startProj.y << ", " << endProj.x << " " <<
  //   endProj.y
  //             << "\n";
  if (checkPos(startProj) && checkPos(endProj)) {
    plotLine(startProj, endProj, c);
  }
}

void Renderer::plotTriLines(tri t, color c) {
  plotLine3(t.a, t.b, c);
  plotLine3(t.b, t.c, c);
  plotLine3(t.c, t.a, c);
}

void Renderer::plotMesh(std::vector<tri> tris, color c) {
  for (tri t : tris) {
    plotTriLines(t, c);
  }
}

bool Renderer::checkPos(vec2 p) { return p.x >= 0 && p.x < w && p.y >= 0 && p.y < h; }

std::vector<tri> Renderer::loadOBJ(std::string file) {
  std::cout << "Opening " << file << std::endl;
  std::ifstream f(file);
  if (!f.is_open()) {
    std::cerr << "Unable to open " << file << "\n";
  }
  std::vector<vec3> verts;
  std::vector<tri> tris;

  float x, y, z;
  int a, b, c;
  std::string line;

  while (f) {
    char ch = f.peek();
    if (ch == 'v') {
      f >> ch >> x >> y >> z;
      verts.push_back({x, y, z});
    } else if (ch == 'f') {
      f >> ch >> a >> b >> c;
      tris.push_back({verts[a], verts[b], verts[c]});
    } else {
      getline(f, line);
    }
  }

  f.close();
  return tris;
}