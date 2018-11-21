// cube demo
// Pete (dooby@bits.bris.ac.uk)

#include <stdio.h>
#include "vram.h"

typedef struct vertex Vertex;
typedef struct face Face;
typedef struct object Object;

struct vertex {
  signed short int x, y, z;
};

struct face {
  Vertex v[4];
  unsigned short col;		// We may want 15bpp colours one day...
  struct face *n;		// Pointer to next face in object face list.
};

struct object {
  struct face *face;
  unsigned short angX;
  unsigned short angY;
  unsigned short angZ;
  signed short offX;
  signed short offY;
  signed short offZ;
};

#define XOFF 40
#define YOFF 25

void psp(Face *temp) {
  int i;
  for(i = 0; i < 4; i ++) {
    temp->v[i].x = XOFF+(temp->v[i].x * (temp->v[i].z + 256)) >> 8;
    temp->v[i].y = YOFF+(temp->v[i].y * (temp->v[i].z + 256)) >> 8;
    fb->BltFX(temp->v[i].x, temp->v[j].y, 1, 1, MK_ATTR(MK_BKGND(COLOR_BLUE)))
  }
}

void trs(Face *temp, signed short offX, signed short offY, signed short offZ) {
  int i;
  for(i = 0; i < 4; i ++) {
    temp->v[i].x += offX;
    temp->v[i].y += offY;
    temp->v[i].z += offZ;
  }
}

void rotX(Face *temp, signed long sin_val, signed long cos_val) {
  int i;
  signed short nz, ny;
  for(i = 0; i < 4; i ++) {
    nz = (signed short)(cos_val * temp->v[i].z - sin_val * temp->v[i].y) >> 7;
    ny = (signed short)(cos_val * temp->v[i].y + sin_val * temp->v[i].z) >> 7;
    temp->v[i].z = nz;
    temp->v[i].y = ny;
    temp->v[i].x = temp->v[i].x;
  }
}

void rotY(Face *temp, signed long sin_val, signed long cos_val) {
  int i;
  signed short nx, nz;
  for(i = 0; i < 4; i ++) {
    nx = (signed short)(cos_val * temp->v[i].x - sin_val * temp->v[i].z) >> 7;
    nz = (signed short)(cos_val * temp->v[i].z + sin_val * temp->v[i].x) >> 7;
    temp->v[i].x = nx;
    temp->v[i].z = nz;
    temp->v[i].y = temp->v[i].y;
  }
}

void rotZ(Face *temp, signed long sin_val, signed long cos_val) {
  int i;
  signed short nx, ny;
  for(i = 0; i < 4; i ++) {
    nx = (signed short)(cos_val * temp->v[i].x - sin_val * temp->v[i].y) >> 7;
    ny = (signed short)(cos_val * temp->v[i].y + sin_val * temp->v[i].x) >> 7;
    temp->v[i].x = nx;
    temp->v[i].y = ny;
    temp->v[i].z = temp->v[i].z;
  }
}

void copy(Face *dest, Face *src) {
  int i;

  // This can almost certainly be done quicker using some sort of memcpy.
  for(i = 0; i < 4; i ++) {
    dest->v[i].x = src->v[i].x;
    dest->v[i].y = src->v[i].y;
    dest->v[i].z = src->v[i].z;
  }
  dest->col = src->col;
}

void init_cube(Object *o, Face f[]) {
  // Left face.
  f[0].v[0].x = -5; f[0].v[0].y = -5; f[0].v[0].z = -5;
  f[0].v[1].x = -5; f[0].v[1].y = -5; f[0].v[1].z =  5;
  f[0].v[2].x = -5; f[0].v[2].y =  5; f[0].v[2].z =  5;
  f[0].v[3].x = -5; f[0].v[3].y =  5; f[0].v[3].z = -5;
  f[0].col = 1;

  // Front face.
  f[1].v[0].x = -5; f[1].v[0].y = -5; f[1].v[0].z =  5;
  f[1].v[1].x =  5; f[1].v[1].y = -5; f[1].v[1].z =  5;
  f[1].v[2].x =  5; f[1].v[2].y =  5; f[1].v[2].z =  5;
  f[1].v[3].x = -5; f[1].v[3].y =  5; f[1].v[3].z =  5;
  f[1].col = 2;

  // Right face.
  f[2].v[0].x =  5; f[2].v[0].y = -5; f[2].v[0].z =  5;
  f[2].v[1].x =  5; f[2].v[1].y = -5; f[2].v[1].z = -5;
  f[2].v[2].x =  5; f[2].v[2].y =  5; f[2].v[2].z = -5;
  f[2].v[3].x =  5; f[2].v[3].y =  5; f[2].v[3].z =  5;
  f[2].col = 3;

  // Back face.
  f[3].v[0].x =  5; f[3].v[0].y = -5; f[3].v[0].z = -5;
  f[3].v[1].x = -5; f[3].v[1].y = -5; f[3].v[1].z = -5;
  f[3].v[2].x = -5; f[3].v[2].y =  5; f[3].v[2].z = -5;
  f[3].v[3].x =  5; f[3].v[3].y =  5; f[3].v[3].z = -5;
  f[3].col = 4;

  // Top face.
  f[4].v[0].x = -5; f[4].v[0].y =  5; f[4].v[0].z =  5;
  f[4].v[1].x =  5; f[4].v[1].y =  5; f[4].v[1].z =  5;
  f[4].v[2].x =  5; f[4].v[2].y =  5; f[4].v[2].z = -5;
  f[4].v[3].x = -5; f[4].v[3].y =  5; f[4].v[3].z = -5;
  f[4].col = 5;

  // Bottom face.
  f[5].v[0].x = -5; f[5].v[0].y = -5; f[5].v[0].z = -5;
  f[5].v[1].x =  5; f[5].v[1].y = -5; f[5].v[1].z = -5;
  f[5].v[2].x =  5; f[5].v[2].y = -5; f[5].v[2].z =  5;
  f[5].v[3].x = -5; f[5].v[3].y = -5; f[5].v[3].z =  5;
  f[5].col = 6;

  // Initialise object face list.
  o->face = &f[0];
  f[0].n = &f[1];
  f[1].n = &f[2];
  f[2].n = &f[3];
  f[3].n = &f[4];
  f[4].n = &f[5];
  f[5].n = 0;

  // Initialise object offset & rotation attributes.
  o->angX =    0;
  o->angY =    0;
  o->angZ =    0;
  o->offX =    0;
  o->offY =    0;
  o->offZ =    0;
}

void main()
{
  CVRAM* fb;
  Object cube;
  Face *toto;
  Face temp, cube_faces[6], *curr;
  init_cube(&cube, cube_faces);
  fb= CVRAM::GetVRAM();


  for(toto= cube.face; toto; toto= toto->n){
   psp(toto);

  }


}