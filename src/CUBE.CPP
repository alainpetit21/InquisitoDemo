#include <stdio.h>
#include <dos.h>
#include <math.h>
#include "conio.h"
#include "vram.h"

CVRAM* fb;
CVRAM back;

signed long sinTable[360];
signed long cosTable[360];

typedef struct vertex Vertex;
typedef struct face Face;
typedef struct object Object;

struct vertex {
  signed short int x, y, z;
  signed short int xr, yr, zr;
  signed short int xt, yt;
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

int m_zbuffer[80*25];
int m_left[80];
int m_right[80];

void GenereTables()
{
 int i;
	for(i=0;i<360;++i){
	 sinTable[i]= sin(i*6.28318/360)*256;
	 cosTable[i]= cos(i*6.28318/360)*256;
    }
}

int char_lookup[6][4] = {
	{'L','L','L','L'},
	{'E','P','P','E'},
	{'I','I','I','I'},
	{'P','E','E','P'},
	{'K','C','C','K'},
	{'K','C','C','K'}
};

int attr_lookup[6][4] = {
	{0x0E,0x0E,0x0E,0x0E},
	{0x0A,0x0A,0x0A,0x0A},
	{0x0E,0x0E,0x0E,0x0E},
	{0x0A,0x0A,0x0A,0x0A},
	{0x0F,0x0F,0x0F,0x0F},
	{0x0F,0x0F,0x0F,0x0F}
};

void psp(Face *temp, int face) {
  int i;
  int a, b, c, d;
  for(i = 0; i < 4; i ++) {
    temp->v[i].xt = (temp->v[i].xr * (temp->v[i].zr + 256)) >> 8;
    temp->v[i].yt = (temp->v[i].yr * (temp->v[i].zr + 256)) >> 8;
	a = temp->v[i].xt+XOFF,
	b = temp->v[i].yt+YOFF,
	c = 1,
	d = 1;
	if(back.ClipFX(a,b,c,d))
	 back.BltFX(a,b,c,d, MK_ATTR(attr_lookup[face][i])|MK_CHAR(char_lookup[face][i]));
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
    nz = (signed short)(cos_val * temp->v[i].zr - sin_val * temp->v[i].yr) >> 8;
    ny = (signed short)(cos_val * temp->v[i].yr + sin_val * temp->v[i].zr) >> 8;
    temp->v[i].zr = nz;
    temp->v[i].yr = ny;
    temp->v[i].xr = temp->v[i].xr;
  }
}

void rotY(Face *temp, signed long sin_val, signed long cos_val) {
  int i;
  signed short nx, nz;
  for(i = 0; i < 4; i ++) {
    nx = (signed short)(cos_val * temp->v[i].xr - sin_val * temp->v[i].zr) >> 8;
    nz = (signed short)(cos_val * temp->v[i].zr + sin_val * temp->v[i].xr) >> 8;
    temp->v[i].xr = nx;
    temp->v[i].zr = nz;
    temp->v[i].yr = temp->v[i].yr;
  }
}

void rotZ(Face *temp, signed long sin_val, signed long cos_val) {
  int i;
  signed short nx, ny;
  for(i = 0; i < 4; i ++) {
    nx = (signed short)(cos_val * temp->v[i].x - sin_val * temp->v[i].y) >> 8;
    ny = (signed short)(cos_val * temp->v[i].y + sin_val * temp->v[i].x) >> 8;
    temp->v[i].xr = nx;
    temp->v[i].yr = ny;
    temp->v[i].zr = temp->v[i].z;
  }
}
void rotate(Object *obj, int x, int y, int z)
{
 Face* toto;
 for(toto= obj->face; toto; toto= toto->n){
  rotZ(toto, sinTable[z], cosTable[z]);
  rotX(toto, sinTable[x], cosTable[x]);
  rotY(toto, sinTable[y], cosTable[y]);
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
  f[0].v[0].x = -10; f[0].v[0].y = -10; f[0].v[0].z = -10;
  f[0].v[1].x = -10; f[0].v[1].y = -10; f[0].v[1].z =  10;
  f[0].v[2].x = -10; f[0].v[2].y =  10; f[0].v[2].z =  10;
  f[0].v[3].x = -10; f[0].v[3].y =  10; f[0].v[3].z = -10;
  f[0].col = COLOR_BLUE;

  // Front face.
  f[1].v[0].x = -15; f[1].v[0].y = -15; f[1].v[0].z =  15;
  f[1].v[1].x =  15; f[1].v[1].y = -15; f[1].v[1].z =  15;
  f[1].v[2].x =  15; f[1].v[2].y =  15; f[1].v[2].z =  15;
  f[1].v[3].x = -15; f[1].v[3].y =  15; f[1].v[3].z =  15;
  f[1].col = COLOR_RED;

  // Right face.
  f[2].v[0].x =  10; f[2].v[0].y = -10; f[2].v[0].z =  10;
  f[2].v[1].x =  10; f[2].v[1].y = -10; f[2].v[1].z = -10;
  f[2].v[2].x =  10; f[2].v[2].y =  10; f[2].v[2].z = -10;
  f[2].v[3].x =  10; f[2].v[3].y =  10; f[2].v[3].z =  10;
  f[2].col = COLOR_BROWN;

  // Back face.
  f[3].v[0].x =  15; f[3].v[0].y = -15; f[3].v[0].z = -15;
  f[3].v[1].x = -15; f[3].v[1].y = -15; f[3].v[1].z = -15;
  f[3].v[2].x = -15; f[3].v[2].y =  15; f[3].v[2].z = -15;
  f[3].v[3].x =  15; f[3].v[3].y =  15; f[3].v[3].z = -15;
  f[3].col = COLOR_BLUE|COLOR_RED;

  // Top face.
  f[4].v[0].x = -5; f[4].v[0].y =  5; f[4].v[0].z =  5;
  f[4].v[1].x =  5; f[4].v[1].y =  5; f[4].v[1].z =  5;
  f[4].v[2].x =  5; f[4].v[2].y =  5; f[4].v[2].z = -5;
  f[4].v[3].x = -5; f[4].v[3].y =  5; f[4].v[3].z = -5;
  f[4].col = COLOR_RED|COLOR_BROWN;

  // Bottom face.
  f[5].v[0].x = -5; f[5].v[0].y = -5; f[5].v[0].z = -5;
  f[5].v[1].x =  5; f[5].v[1].y = -5; f[5].v[1].z = -5;
  f[5].v[2].x =  5; f[5].v[2].y = -5; f[5].v[2].z =  5;
  f[5].v[3].x = -5; f[5].v[3].y = -5; f[5].v[3].z =  5;
  f[5].col = COLOR_BROWN|COLOR_BLUE;

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

void bltRotated(CVRAM* dst, CVRAM* src, signed long sin_val, signed long cos_val)
{
 int i, j;
 signed short nx, ny;
 for(i = 0; i < src->m_cx; i++) {
  for(j = 0; j < src->m_cy; j++) {
   nx = 18+((signed short)(cos_val * (i-(src->m_cx/2)) - sin_val * (j-(src->m_cy/2))) >> 8);
   ny = 18+((signed short)(cos_val * (j-(src->m_cy/2)) + sin_val * (i-(src->m_cx/2))) >> 8);
   dst->m_data[ny*dst->m_cx+nx]= src->m_data[j*src->m_cx+i];
  }
 }
}

char* szGreetz =
	"                                                                                "
	"hey, mode text cucumber by inquisito and neuroS! greetings go to (we don't know all these groups, but we always dreamed of greetings a lot of people) : HeCk8, xmoby, suboptical, absurdus, inca master, dc5, RV, depth, the hornet, humanoid, madbrain, the finn, barney, decax, vexxor, krishol, j33p33, unkelben, apsod, madASScows, darkium, zelkor, rasoir 1911, the lady in red (is dancing with me) and everyone in the world (except Plada and all the other cucumbers' killers).";

#define GREETZ_SKIP	7

void main()
{
int greetz_skip = GREETZ_SKIP;
	char* greetz_ptr = szGreetz;
 float incX= 0.1, incY= 0.1;
 float posX= 10, posY= 10;
 int i;
 int a, b, c, d,e,f;
 FILE* fbk;
 unsigned char* bkBinaire;
 int n,angle= 0;
 Object cube;
 CVRAM bkgnd;
 CVRAM filter;
 CVRAM filter2;
 Face *toto;
 Face temp, cube_faces[6], *curr;
 init_cube(&cube, cube_faces);

 textmode(C4350);
 fb= CVRAM::GetVRAM();
 back.Create(fb->m_cx, fb->m_cy);

 GenereTables();

 bkgnd.Create(fb->m_cx, 1);
 filter.Create(27, 25);
 filter2.Create(37, 37);

 /*Loader Back*/
 fbk= fopen("BKGND.BIN", "rb");
 fread(bkgnd.m_data, 80, 1*sizeof(short), fbk);
 fclose(fbk);

 /*Loader Filter*/
 fbk= fopen("FILTER.BIN", "rb");
 fread(filter.m_data, 27, 25*sizeof(short), fbk);
 fclose(fbk);


 while(!kbhit()){
  back.BltFX(0, 0,fb->m_cx, fb->m_cy, MK_ATTR(0x00));
  /*Wrapper le texte*/
  for(i= 0; i < fb->m_cy; ++i){
   int toto2= ((i*17)%80)-80;
   a=toto2,b=i,c=0,d=0,e=80,f=1;
   if (back.Clip(a,b,&bkgnd,c,d,e,f))
	   back.Blt(a, b, &bkgnd, c, d, e, f, MK_CHAR(OP_SET));
   a=toto2+80,b=i,c=0,d=0,e=80,f=1;
   if (back.Clip(a,b,&bkgnd,c,d,e,f))
	   back.Blt(a, b, &bkgnd, c, d, e, f, MK_CHAR(OP_SET));
  }

  back.Blt(posX,posY,&filter2,0,0,filter2.m_cx,filter2.m_cy, MK_ATTR(OP_SET));

  memset(filter2.m_data, 0, sizeof(short)*filter2.m_cx*filter2.m_cy);
  bltRotated(&filter2, &filter, sinTable[angle], cosTable[angle]);
  rotate(&cube, angle, angle/2, angle/4);

  for(n=0,toto= cube.face; toto; toto= toto->n,n++){
   psp(toto, n);
  }
  if((++angle)>=360)
   angle=0;
  if((posX+= incX)>=80-filter2.m_cx)    incX*= -1;
  if((posY+= incY)>=50-filter2.m_cy)    incY*= -1;
  if((posX+= incX)<=0)                 incX*= -1;
  if((posY+= incY)<=0)                 incY*= -1;

  //greetz
  back.BltFX(0, back.m_cy -5,back.m_cx,1,0x0F00);
  back.BltFX(0, back.m_cy -5,13,1,0x0400,MK_ATTR(OP_SET));
  back.BltFX(13, back.m_cy -5,13,1,0x0C00,MK_ATTR(OP_SET));
  back.BltFX(54, back.m_cy -5,13,1,0x0C00,MK_ATTR(OP_SET));
  back.BltFX(67, back.m_cy -5,13,1,0x0400,MK_ATTR(OP_SET));


   fb->Blt(0, 0, &back, 0, 0, fb->m_cx, fb->m_cy);

  gotoxy(1,back.m_cy-4);
printf("%-79.79s",greetz_ptr);
  --greetz_skip;
  if (greetz_skip == 0)
  {
  	greetz_skip = GREETZ_SKIP;
  ++greetz_ptr;
	if(!(*greetz_ptr))
	  	greetz_ptr=szGreetz;
  }
  delay(10);
 }
 back.Destroy();
 bkgnd.Destroy();
}
