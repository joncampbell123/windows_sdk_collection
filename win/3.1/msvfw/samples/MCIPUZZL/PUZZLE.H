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

#define PSIZE	4	// Puzzle is 4x4

typedef struct {
    int a[PSIZE][PSIZE];	// square # at position [x][y]
    int hx;			// coordinates of "hole"
    int	hy;
} PUZZLE, FAR *LPPUZZLE;

void InitPuzzle(LPPUZZLE p, BOOL fScramble);
void ClickPuzzle(LPPUZZLE p, int x, int y);
void MixPicture(LPPUZZLE p, LPBITMAPINFOHEADER lpbi,
		LPBYTE lpIn, LPBYTE lpOut);

