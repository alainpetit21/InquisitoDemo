#define CHAR(x)			((x) & 0xFF)
#define ATTR(x)			CHAR((x) >> 8)

#define MK_CHAR(x)		CHAR(x)
#define MK_ATTR(x)		(CHAR(x) << 8)

#define OP_NULL			0
#define OP_SET				1
#define OP_AND				2
#define OP_OR				3
#define OP_XOR				4

#define COLOR_BLACK		0x00
#define COLOR_BLUE		0x01
#define COLOR_GREEN		0x02
#define COLOR_CYAN		0x03
#define COLOR_RED			0x04
#define COLOR_MAGENTA	0x05
#define COLOR_BROWN		0x06
#define COLOR_WHITE		0x07

#define COLOR_LIGHT		0x08
#define COLOR_BLINK		0x80

#define MK_BKGND(x)		(((x) & 0xF) << 4)

class CVRAM
{
public:
	short* m_data;
	int m_cx,m_cy;
	int m_del;

	static CVRAM* GetVRAM();

	CVRAM();

	void Init();

	int Create(int cx, int cy);
	void Destroy();

	int Attach(short* data, int cx, int cy);
 	void Detach();

    int Clip(int& dx, int& dy, CVRAM* src, int& sx, int& sy, int& cx, int& cy);
	void Blt(int dx, int dy, CVRAM* src, int sx, int sy, int cx, int cy);
	void Blt(int dx, int dy, CVRAM* src, int sx, int sy, int cx, int cy, int op);

    int ClipFX(int& dx, int& dy, int& cx, int& cy);
	void BltFX(int dx, int dy, int cx, int cy, int src);
	void BltFX(int dx, int dy, int cx, int cy, int src, int op);
};
