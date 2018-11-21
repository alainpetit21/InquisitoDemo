typedef struct vertex Vertex;
typedef struct face Face;
typedef struct object Object;
struct vertex{
 signed short int x, y, z;
};
struct face{
 Vertex v4;
 unsigned short col;
 struct face *n;
};

struct object{
 struct face *face;
 unsigned short angX;
 unsigned short angY;
 unsigned short angZ;
 signed short offX;
 signed short offY;
 signed short offZ;
};


#define XOFF 40
#define YOFF 12











void main(void)
{

}
