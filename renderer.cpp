#include "renderer.h"

#include <iostream>

#define DEFAULT_SIZE 600

Renderer::Renderer(int width, int height) : w(width), h(height) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(w, h, 0, &window, &renderer);
}

Renderer::~Renderer() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

void Renderer::start() {
  //   worldToCamera = rotX(3.1415 * 4);

  while (1) {
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) break;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);  // black background

    draw();
    ++t;
    SDL_RenderPresent(renderer);
  }
}

void Renderer::plot(vec2 pos, color c) {
  SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
  SDL_RenderDrawPoint(renderer, pos.x, pos.y);
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
  mat4 ry = rotY(theta); // rotation around Y axis
  mat4 r = mul(rx, ry);

  vec3 a, b, c, d;
  a = mulP(r, {0.0, 0.0, 0.0});
  b = mulP(r, {1., 0.0, 0.0});
  c = mulP(r, {0.5, 0.0, -1.});
  d = mulP(r, {0.5, 1.0, -0.5});
  tri tetrahedron[4] = {{a, b, c}, {a, b, d}, {a, c, d}, {b, c, d}};

  for (tri t : tetrahedron) {
    plotTriLines(t, {123, 123, 255, 255});
  }
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
//   std::cout << startProj.x << " " << startProj.y << ", " << endProj.x << " " << endProj.y
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

bool Renderer::checkPos(vec2 p) { return p.x >= 0 && p.x < w && p.y >= 0 && p.y < h; }