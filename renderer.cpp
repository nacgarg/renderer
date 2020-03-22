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
  while (1) {
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) break;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);  // black background

    draw();
    SDL_RenderPresent(renderer);
  }
}

void Renderer::plot(vec2 pos, color c) {
  SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
  SDL_RenderDrawPoint(renderer, pos.x, pos.y);
}

void Renderer::draw() {
  // here is where we actually plot stuff
  plotLine({0, 0}, {500, 100}, {255, 255, 255, 255});
  plotLine({500, 100}, {300, 550}, {255, 255, 255, 255});
  plotLine({300, 550}, {0, 0}, {123, 123, 255, 255});
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