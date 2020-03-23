#include "renderer.h"

#include <fstream>
#include <iostream>
#include <limits>

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
  cowObject = loadOBJ("cow.obj");

  while (1) {
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) break;
    switch (event.type) {
      case SDL_KEYDOWN:
        // printf("Key press detected: %s\n", SDL_GetKeyName(event.key.keysym.sym));
        if (event.key.keysym.sym == SDLK_w) {
          worldToCamera.l.z += 0.05;
        }
        if (event.key.keysym.sym == SDLK_a) {
          worldToCamera.l.x += 0.05;
        }
        if (event.key.keysym.sym == SDLK_s) {
          worldToCamera.l.z -= 0.05;
        }
        if (event.key.keysym.sym == SDLK_d) {
          worldToCamera.l.x -= 0.05;
        }
        if (event.key.keysym.sym == SDLK_k) {
          hFov += 0.01;
        }
        if (event.key.keysym.sym == SDLK_l) {
          hFov -= 0.01;
        }
        break;

      case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
          screen = SDL_GetWindowSurface(window);
          h = screen->h;
          w = screen->w;
        }
      default:
        break;
    }

    unsigned int start = SDL_GetTicks();

    SDL_memset(screen->pixels, 0, screen->h * screen->pitch);  // black background
    depthBuffer.clear();
    depthBuffer.resize(w * h, std::numeric_limits<float>::infinity());
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

  float theta = (float)t / 500;
  mat4 rx = rotX(theta);                            // rotation matrix around X axis
  mat4 ry = rotY(theta);                            // rotation around Y axis
  mat4 tz = trans({0, 0, sin(theta * 5.f) * 5.f});  // translation in z
  mat4 r = mul(rx, ry);
  mat4 rt = mul(r, tz);

  vec3 a, b, c, d;
  a = mulP(r, {0.0, 0.0, 0.0});
  b = mulP(r, {1., 0.0, 0.0});
  c = mulP(r, {0.5, 0.0, -1.});
  d = mulP(r, {0.5, 1.0, -0.5});
  std::vector<tri> tetrahedron = {{a, b, c}, {a, b, d}, {a, c, d}, {b, c, d}};
  std::vector<tri> rotatedTest(testObject.size());
  int i = 0;
  for (tri t : testObject) {
    rotatedTest[i++] = {mulP(rt, t.a), mulP(rt, t.b), mulP(rt, t.c)};
  }
  std::vector<tri> rotatedCow(cowObject.size());
  i = 0;
  for (tri t : cowObject) {
    rotatedCow[i++] = {mulP(r, t.a), mulP(r, t.b), mulP(r, t.c)};
  }
  plotMesh(rotatedTest, {255, 50, 255, 255});
  plotMesh(rotatedCow, {255, 255, 50, 255});
  plotWireframe(tetrahedron, {50, 255, 255, 255});
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

vec3 Renderer::projRaster(vec3 pScreen) {
  float aspect = (float)w / (float)h;
  float canvasWidth = tan(hFov / 2) * nearClipPlane;
  float canvasHeight = canvasWidth / aspect;

  //   std::cout << canvasWidth << " " << canvasHeight << "\n";

  vec3 pNorm = {(pScreen.x + canvasWidth / 2.0) / canvasWidth,
                (pScreen.y + canvasHeight / 2.0) / canvasHeight, pScreen.z};

  float pixelWidth = w;
  float pixelHeight = h;
  vec3 pRaster = {pNorm.x * pixelWidth, (1.0 - pNorm.y) * pixelHeight, pScreen.z};
  return pRaster;
}

vec3 Renderer::projCamera(vec3 p) {
  // compute position in camera/screen space
  vec3 pCamera = mulP(worldToCamera, p);
  vec3 pScreen = {nearClipPlane * pCamera.x / -pCamera.z,
                  nearClipPlane * pCamera.y / -pCamera.z, -pCamera.z};
  return pScreen;
}

vec2 Renderer::projectPoint(vec3 p) {
  vec3 pScreen = projCamera(p);
  return projRaster(pScreen);
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
    plotTriRaster(t, c);
  }
}

void Renderer::plotWireframe(std::vector<tri> tris, color c) {
  for (tri t : tris) {
    plotTriLines(t, c);
  }
}

void Renderer::plotTriRaster(tri t, color c) {
  // First find screenspace coords of triangle verts
  vec3 c0 = projCamera(t.a);
  vec3 c1 = projCamera(t.b);
  vec3 c2 = projCamera(t.c);

  // compute normal vector of plane
  vec3 planeNormal = cross(unit(c1 - c0), unit(c2 - c0));
  if (planeNormal.z <= 0) {
    // backface culling
    return;
  }

  vec3 v0 = projRaster(c0);
  vec3 v1 = projRaster(c1);
  vec3 v2 = projRaster(c2);

  if (!checkPos(v0) || !checkPos(v1) || !checkPos(v2)) return;

  // Find bounding box of triangle
  int minX = std::min(v0.x, std::min(v1.x, v2.x));
  int minY = std::min(v0.y, std::min(v1.y, v2.y));
  int maxX = std::max(v0.x, std::max(v1.x, v2.x));
  int maxY = std::max(v0.y, std::max(v1.y, v2.y));

  vec3 center = {(c0.x + c1.x + c2.x) / 3.f, (c0.y + c1.y + c2.y) / 3.f,
                 (c0.z + c1.z + c2.z) / 3.f};
  // std::cout << "center: " << center.x << "," << center.y <<","<< center.z << "\n";
  vec3 centerUnit = unit(vec3({0, 0, 0}) - center);
  vec3 light = unit(vec3({3, -3, -1}));

  float dotProduct = (centerUnit.x * planeNormal.x) + (centerUnit.y * planeNormal.y) +
                     (centerUnit.z * planeNormal.z);
  float angle = acos(dotProduct);
  float shade = std::min(std::max(0.f, -dotProduct * 500), 0.7f);
  shade += 0.3f;
  // std::cout << dotProduct << " " << angle << " " << shade << "\n";

  for (int i = minX; i <= maxX; ++i) {
    for (int j = minY; j <= maxY; ++j) {
      if (!checkPos({i, j})) continue;
      vec3 p = {i, j, 0};
      float doubleArea = triEdge(v0, v1, v2);
      float w0 = triEdge(v1, v2, p);
      float w1 = triEdge(v2, v0, p);
      float w2 = triEdge(v0, v1, p);

      if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
        // inside triangle
        w0 /= doubleArea;
        w1 /= doubleArea;
        w2 /= doubleArea;

        float z = 1.0 / ((w0 / v0.z) + (w1 / v1.z) + (w2 / v2.z));
        if (z < depthBuffer[j * w + i]) {
          depthBuffer[j * w + i] = z;

          // std::cout << dotProduct << " " << shade << "\n";

          plot(p, c * shade);
          // plot(p, {(unsigned char)255 * w0, (unsigned char)255 * w1,
          //          (unsigned char)255 * w2, 255});
          // plot(p, c);
        }
      }
    }
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
      tris.push_back({verts[a - 1], verts[b - 1], verts[c - 1]});
    } else {
      getline(f, line);
    }
  }

  f.close();
  return tris;
}