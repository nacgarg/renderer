#ifndef _INCL_TYPES
#define _INCL_TYPES

struct vec3 {
  float x;
  float y;
  float z;
};

struct color {
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char a;
};

struct vec2 {
  float x;
  float y;
};

struct vec4 {
  float x;
  float y;
  float z;
  float w;
};

struct mat4 {
  // column vectors
  vec4 i;
  vec4 j;
  vec4 k;
  vec4 l;
  /*
  i  j  k  l
  ----------
  x  x  x  x
  y  y  y  y
  z  z  z  z
  w  w  w  w
  */
};

struct tri {
  vec3 a;
  vec3 b;
  vec3 c;
};

vec4 mul(mat4 m, vec4 v);

vec3 mulP(mat4 m, vec3 v);

vec3 mulV(mat4 m, vec3 v);

mat4 mul(mat4 a, mat4 b);

mat4 rotY(float theta);
mat4 rotX(float theta);
mat4 trans(vec3 pos);


#endif