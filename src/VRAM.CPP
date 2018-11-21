#include <string.h>
#include <dos.h>
#include "conio.h"
#include "vram.h"

static CVRAM s_vram;

short* m_data;
int m_cx,m_cy;
int m_del;

CVRAM* CVRAM::GetVRAM()
{/*
	text_info ti;
    gettextinfo(&ti);
	s_vram.m_data = (short*)MK_FP(0xb800,0x0000);
	s_vram.m_cx = ti.screenwidth,
	s_vram.m_cy = ti.screenheight;
*/
	s_vram.m_data = (short*)(0xb8000000);
	s_vram.m_cx = 80;
	s_vram.m_cy = 300;
	return &s_vram;
}

void CVRAM::Init()
{
	m_data= 0;
	m_cx=m_cy=0;
	m_del = 0;
}

CVRAM::CVRAM()
{
	Init();
}

int CVRAM::Create(int cx, int cy)
{
	if (cx <= 0 || cy <= 0)
		return 0;
	m_data = new short[(m_cx = cx) * (m_cy = cy)];
	m_del = 1;
	return 1;
}

void CVRAM::Destroy()
{
	if (!m_del)
		return;
	delete [] m_data;
	Init();
}

int CVRAM::Attach(short* data, int cx, int cy)
{
	if (!data || cx <= 0 || cy <= 0)
		return 0;
	m_data = data;
	m_cx = cx, m_cy = cy;
	return 1;
}

void CVRAM::Detach()
{
	if (m_del)
		return;
	Init();
}

int CVRAM::Clip(int& dx, int& dy, CVRAM* src, int& sx, int& sy, int& cx, int& cy)
{
	if (dx < 0)
    {
    	sx -= dx;
    	cx += dx;
        dx = 0;
    }
	if (dy < 0)
    {
    	sy -= dy;
    	cy += dy;
        dy = 0;
    }

	if (sx < 0)
    {
    	dx -= sx;
    	cx += sx;
        sx = 0;
    }
	if (sy < 0)
    {
    	dy -= sy;
    	cy += sy;
        sy = 0;
    }

    if (dx + cx > m_cx)
		cx = m_cx - dx;
    if (dy + cy > m_cy)
		cy = m_cy - dy;

    if (sx + cx > src->m_cx)
		cx = src->m_cx - sx;
    if (sy + cy > src->m_cy)
		cy = src->m_cy - sy;

    if (cx <= 0 || cy <= 0)
    	return 0;
    return 1;
}

void CVRAM::Blt(int dx, int dy, CVRAM* src, int sx, int sy, int cx, int cy)
{
	short
		*dest_ptr = m_data + (dy * m_cx) + dx,
		*src_ptr = src->m_data + (sy * src->m_cx) + sx;
	while (cy--)
	{
		memmove(dest_ptr, src_ptr, cx * sizeof(short));
		dest_ptr += m_cx, src_ptr += src->m_cx;
	}
}

void CVRAM::Blt(int dx, int dy, CVRAM* src, int sx, int sy, int cx, int cy, int op)
{
	int
		i,
		temp[2];
	short
		*dest_ptr = m_data + (dy * m_cx) + dx,
		*src_ptr = src->m_data + (sy * src->m_cx) + sx;
	while (cy--)
	{
		for (i = cx; i--; )
		{
			temp[0] = CHAR(dest_ptr[i]),
				temp[1] = ATTR(dest_ptr[i]);

			switch (CHAR(op))
			{
			case OP_SET:
				temp[0] = CHAR(src_ptr[i]);
				break;
			case OP_AND:
				temp[0] &= CHAR(src_ptr[i]);
				break;
			case OP_OR:
				temp[0] |= CHAR(src_ptr[i]);
				break;
			case OP_XOR:
				temp[0] ^= CHAR(src_ptr[i]);
				break;
			case OP_NULL:
			default:
				/*do nothing*/;
			}

			switch (ATTR(op))
			{
			case OP_SET:
				temp[1] = ATTR(src_ptr[i]);
				break;
			case OP_AND:
				temp[1] &= ATTR(src_ptr[i]);
				break;
			case OP_OR:
				temp[1] |= ATTR(src_ptr[i]);
				break;
			case OP_XOR:
				temp[1] ^= ATTR(src_ptr[i]);
				break;
			case OP_NULL:
			default:
				/*do nothing*/;
			}

			dest_ptr[i] = MK_CHAR(temp[0]) | MK_ATTR(temp[1]);
		}
		dest_ptr += m_cx, src_ptr += src->m_cx;
	}
}

int CVRAM::ClipFX(int& dx, int& dy, int& cx, int& cy)
{
	if (dx < 0)
    {
    	cx += dx;
        dx = 0;
    }
	if (dy < 0)
    {
    	cy += dy;
        dy = 0;
    }

    if (dx + cx > m_cx)
		cx = m_cx - dx;
    if (dy + cy > m_cy)
		cy = m_cy - dy;

    if (cx <= 0 || cy <= 0)
    	return 0;
    return 1;
}

void CVRAM::BltFX(int dx, int dy, int cx, int cy, int src)
{
	int i;
	short
		*dest_ptr = m_data + (dy * m_cx) + dx;
	while (cy--)
    {
		for (i = cx; i--;)
			dest_ptr[i] = src;
		dest_ptr += m_cx;
	}
}

void CVRAM::BltFX(int dx, int dy, int cx, int cy, int src, int op)
{
	int
		i,
		temp[2];
	short
		*dest_ptr = m_data + (dy * m_cx) + dx;
	while (cy--)
	{
		for (i = cx; i--; )
		{
			temp[0] = CHAR(dest_ptr[i]),
				temp[1] = ATTR(dest_ptr[i]);

			switch (CHAR(op))
			{
			case OP_SET:
				temp[0] = CHAR(src);
				break;
			case OP_AND:
				temp[0] &= CHAR(src);
				break;
			case OP_OR:
				temp[0] |= CHAR(src);
				break;
			case OP_XOR:
				temp[0] ^= CHAR(src);
				break;
			case OP_NULL:
			default:
				/*do nothing*/;
			}

			switch (ATTR(op))
			{
			case OP_SET:
				temp[1] = ATTR(src);
				break;
			case OP_AND:
				temp[1] &= ATTR(src);
				break;
			case OP_OR:
				temp[1] |= ATTR(src);
				break;
			case OP_XOR:
				temp[1] ^= ATTR(src);
				break;
			case OP_NULL:
			default:
				/*do nothing*/;
			}

			dest_ptr[i] = MK_CHAR(temp[0]) | MK_ATTR(temp[1]);
		}
		dest_ptr += m_cx;
	}
}
