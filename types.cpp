#include "types.h"

#include "math.h"

vec4 mul(mat4 m, vec4 v) {
  vec4 out;
  out.x = v.x * m.i.x + v.y * m.j.x + v.z * m.k.x + v.w * m.l.x;
  out.y = v.x * m.i.y + v.y * m.j.y + v.z * m.k.y + v.w * m.l.y;
  out.z = v.x * m.i.z + v.y * m.j.z + v.z * m.k.z + v.w * m.l.z;
  out.w = v.x * m.i.w + v.y * m.j.w + v.z * m.k.w + v.w * m.l.w;
  return out;
}

vec3 mulP(mat4 m, vec3 v) {
  // multiply the matrix m by the position vector v (allow translation)
  vec4 v4 = {v.x, v.y, v.z, 1.0};
  vec4 r = mul(m, v4);
  return {r.x, r.y, r.z};
}

vec3 mulV(mat4 m, vec3 v) {
  // multiply the matrix m by the direction vector v (no translation)
  vec4 v4 = {v.x, v.y, v.z, 0.0};
  vec4 r = mul(m, v4);
  return {r.x, r.y, r.z};
}

mat4 zero() {
  mat4 m;
  m.i = {0, 0, 0, 0};
  m.j = {0, 0, 0, 0};
  m.k = {0, 0, 0, 0};
  m.l = {0, 0, 0, 0};
  return m;
}

mat4 rotY(float theta) {
  mat4 r = zero();
  float c = cos(theta);
  float s = sin(theta);
  r.i.x = c;
  r.i.z = -s;
  r.j.y = 1.0;
  r.k.x = s;
  r.k.z = c;
  r.l.w = 1.0;
  return r;
}

mat4 rotX(float theta) {
  mat4 r = zero();
  float c = cos(theta);
  float s = sin(theta);
  r.i.x = 1.0;
  r.j.y = c;
  r.j.z = s;
  r.k.y = -s;
  r.k.z = c;
  r.l.w = 1.0;
  return r;
}

mat4 trans(vec3 pos) {
  // translation matrix at the position
  mat4 t = zero();
  t.l.x = pos.x;
  t.l.y = pos.y;
  t.l.z = pos.z;
  t.i.x = 1.0;
  t.j.y = 1.0;
  t.k.z = 1.0;
  t.l.w = 1.0;
  return t;
}

mat4 mul(mat4 a, mat4 b) {
  mat4 r = zero();
  r.i = mul(a, b.i);
  r.j = mul(a, b.j);
  r.k = mul(a, b.k);
  r.l = mul(a, b.l);
  return r;
}

bool operator==(color a, color b) {
  return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}
bool operator!=(color a, color b) { return !(a == b); }
color operator*(color a, float b) { return {a.r * b, a.g * b, a.b * b, a.a * b}; }
float triEdge(vec3 a, vec3 b, vec3 c) {
  return ((c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x));
}

float length(vec3 a) { return sqrt(a.x * a.x + a.y * a.y + a.z * a.z); }
vec3 unit(vec3 a) {
  float l = length(a);
  return {a.x / l, a.y / l, a.z / l};
}

vec3 cross(vec3 u, vec3 v) {
  return {(u.y * v.z) - (v.y * u.z), (v.x * u.z) - (u.x * v.z),
          (u.x * v.y) - (v.x * u.y)};
}

vec3 operator-(vec3 a, vec3 b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
