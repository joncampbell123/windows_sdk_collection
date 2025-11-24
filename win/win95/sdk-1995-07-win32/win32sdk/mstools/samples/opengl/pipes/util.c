/******************************Module*Header*******************************\
* Module Name: util.c
*
* Misc. utility functions
*
* Copyright (c) 1994-1995 Microsoft Corporation
*
\**************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <GL/gl.h>

extern HWND PipesGetHWND();

/*-----------------------------------------------------------------------
|									|
|    mfRand(numVal): 							|
|        Generates random number 0..(numVal-1) 				|
|									|
-----------------------------------------------------------------------*/

int mfRand( int numVal )
{
    int num;

    num = (int) ( numVal * ( ((float)rand()) / ((float)(RAND_MAX+1)) ) );
    return num;
}


/*-----------------------------------------------------------------------
|									|
|    RectWipeClear(width, height):                                      |
|       - Does a rectangular wipe (or clear) by drawing in a sequence   |
|         of rectangles using Gdi                                       |
|	MOD: add calibrator capability to adjust speed for different	|
|	     architectures						|
|									|
-----------------------------------------------------------------------*/
void RectWipeClear( int width, int height )
{
    HWND hwnd;
    HDC hdc;
    HBRUSH hbr;
    RECT rect;
    int i, j, xinc, yinc, numDivs = 500;
    int xmin, xmax, ymin, ymax;
    int repCount = 3;

    xinc = 1;
    yinc = 1;
    numDivs = height;
    xmin = ymin = 0;
    xmax = width;
    ymax = height;

    hwnd = PipesGetHWND();
    hdc = GetDC( hwnd );

    hbr = CreateSolidBrush( RGB( 0, 0, 0 ) );

    for( i = 0; i < (numDivs/2 - 1); i ++ ) {
      for( j = 0; j < repCount; j ++ ) {
	rect.left = xmin; rect.top = ymin;
	rect.right = xmax; rect.bottom = ymin + yinc;
    	FillRect( hdc, &rect, hbr );
	rect.top = ymax - yinc;
	rect.bottom = ymax;
    	FillRect( hdc, &rect, hbr );
	rect.top = ymin + yinc;
	rect.right = xmin + xinc; rect.bottom = ymax - yinc;
    	FillRect( hdc, &rect, hbr );
	rect.left = xmax - xinc; rect.top = ymin + yinc;
	rect.right = xmax; rect.bottom = ymax - yinc;
    	FillRect( hdc, &rect, hbr );
      }

	xmin += xinc;
	xmax -= xinc;
	ymin += yinc;
	ymax -= yinc;
    }

    // clear last square in middle

    rect.left = xmin; rect.top = ymin;
    rect.right = xmax; rect.bottom = ymax;
    FillRect( hdc, &rect, hbr );

    ReleaseDC( hwnd, hdc );

    GdiFlush();
}
