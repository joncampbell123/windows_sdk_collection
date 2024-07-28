/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include "puzzle.h"

// C7 _fmemset does not work, it wants near pointer?
char achHack[1024] = {0};

//
// Initialize the puzzle, and optionally simulate 1000 clicks on the puzzle
//
void InitPuzzle(LPPUZZLE p, BOOL fScramble)
{
    int i,j;

    // Set the puzzle to a "solved" state
    for (i = 0; i < PSIZE; i++)
	for (j = 0; j < PSIZE; j++)
	    p->a[i][j] = i + j * PSIZE;

    // Put the "hole" in the lower right corner.
    p->a[PSIZE-1][PSIZE-1] = -1;
    p->hx = PSIZE - 1;
    p->hy = PSIZE - 1;

    if (fScramble) {
	// Make things really be random
	srand((unsigned int) timeGetTime());
    
	for (i = 0; i < 1000; i++) {
	    int r, s;

	    // Click on a random square
	    r = rand() % PSIZE;
	    s = rand() % PSIZE;

	    ClickPuzzle(p, r, s);
	}
    }
}

//
// Given a puzzle, and x & y in puzzle coordinates, move squares around
// or not as appropriate, given how such puzzles work.
//
void ClickPuzzle(LPPUZZLE p, int x, int y)
{
    int i;

    if (x < 0 || x >= PSIZE)
	return;
    
    if (y < 0 || y >= PSIZE)
	return;
    
    if (x == p->hx) {
	if (y < p->hy) {
	    for (i = p->hy; i > y; i--) {
		p->a[x][i] = p->a[x][i-1];
	    }
	} else if (y > p->hy) {
	    for (i = p->hy; i < y; i++) {
		p->a[x][i] = p->a[x][i+1];
	    }
	}
	p->hy = y;
	p->a[x][y] = -1;
    } else if (y == p->hy) {
	if (x < p->hx) {
	    for (i = p->hx; i > x; i--) {
		p->a[i][y] = p->a[i-1][y];
	    }
	} else if (x > p->hx) {
	    for (i = p->hx; i < x; i++) {
		p->a[i][y] = p->a[i+1][y];
	    }
	}
	p->hx = x;
	p->a[x][y] = -1;
    }

    // We could potentially see if the puzzle was solved here.
    // If we do that, the prototype should change to
    // BOOL ClickPuzzle(LPPUZZLE p, int x, int y, BOOL fCheckSolved)
    // where we would pass TRUE for fCheckSolved if the call was
    // a result of the user really clicking, and not a call from
    // InitPuzzle() or something....
}

#define WIDTHBYTES(i)     ((unsigned)((i+31)&(~31))/8)  /* ULONG aligned ! */
#define DIBWIDTHBYTES(bi) (DWORD)WIDTHBYTES((int)(bi).biWidth * (int)(bi).biBitCount)

//
// Given a puzzle, map the input picture to the output picture with squares
// rearranged.
//
// Works on any RGB DIB.  Doesn't work on bitmaps, probably, so could be a
// problem with Todd's new DrawDib.
//
void MixPicture(LPPUZZLE p, LPBITMAPINFOHEADER lpbi,
		LPBYTE lpIn, LPBYTE lpOut)
{
    int     i,j;
    LONG    lRowBytes;
    int     y;
    int     dx = ((int) lpbi->biWidth / PSIZE) * ((int) lpbi->biBitCount / 8);
    int     dy = (int) lpbi->biHeight / PSIZE;
    BYTE _huge *lpI;
    BYTE _huge *lpO;
    
    lRowBytes = DIBWIDTHBYTES(*lpbi);

    for (i = 0; i < PSIZE; i++) {
	for (j = 0; j < PSIZE; j++) {
	    // Get pointer to square we're copying into 
	    lpO = (BYTE _huge *) lpOut +
		  (PSIZE - 1 - j) * dy * lRowBytes +
		  dx * i;
	    
	    if (p->a[i][j] >= 0) {
		// Get pointer to square we're copying from
		lpI = (BYTE _huge *) lpIn +
		      (PSIZE - 1 - (p->a[i][j] / PSIZE)) * dy * lRowBytes +
		      dx * (p->a[i][j] % PSIZE);

		// do the copy
		for (y = 0; y < dy; y++) {
		    hmemcpy(lpO, lpI, dx);
		    lpO += lRowBytes;
		    lpI += lRowBytes;
		}
	    } else {
		// clear the square to zeroes
		for (y = 0; y < dy; y++) {
		    /////_fmemset(lpO, 0, dx);
		    hmemcpy(lpO, achHack, dx);
		    lpO += lRowBytes;
		}
	    }
	}
    }
}
