// demo_txt.cpp : Defines the entry point for the console application.
//

#include "vram.h"
#include <conio.h>

int main(void)
{
	CVRAM surface1;
	surface1.Create(10,10);
	surface1.BltFX(0, 0, 10, 10,
		MK_ATTR(MK_BKGND(COLOR_BLUE)) | MK_CHAR('1'));

	CVRAM surface2;
	surface2.Create(10,10);
	surface2.BltFX(0, 0, 10, 10,
		MK_ATTR(MK_BKGND(COLOR_RED)) | MK_CHAR('2'));

	int
    	dx = -2,
        dy = -2,
        sx = -3,
        sy = -3,
        cx = 10,
        cy = 10;
	CVRAM* pVRAM = CVRAM::GetVRAM();
/*	if (pVRAM->Clip(dx, dy, &surface1, sx, sy, cx, cy))
		pVRAM->Blt(dx, dy, &surface1, sx, sy, cx, cy);
*/		pVRAM->BltFX(10, 10, 1, 1, MK_ATTR(COLOR_WHITE) | MK_CHAR('A'));

    surface1.Destroy();
    surface2.Destroy();


	getch();
	return 0;
}
