/* 
 * board.c		
 */

#include <windows.h>
#include <wincom.h>
#include <mediaman.h>
#include "medgo.h"
#include "go.h"


/* constants */
#define rgbGOBOARD	RGB(128, 64, 64)	// the Go board itself
#define rgbGOLIGHTLINE	RGB(255, 255, 255)	// lines on the Go board
#define rgbGODARKLINE	RGB(128, 128, 128)	// shadow lines
#define rgbGOPLAYER1	RGB(255, 255, 255)	// player 1's stones
#define rgbGOPLAYER2	RGB(0, 0, 0)		// player 2's stones
#define SHWD		2			// dark (shadow) line width
#define SPOT_SHRINK	100			// board:spot size ratio


/* AppPaint(hdc, medid, prcPaint, ptClientSize)
 *
 * Paint rectangle <*prcPaint> of the Go board resource <medid> into <hdc>.
 * <ptClient> is the size of the Go board.  It is assumed that <medid>
 * is already accessed (loaded) if <medid> is not NULL.
 */
VOID AppPaint( HDC hdc, MEDID medid, LPRECT prcPaint, POINT ptClientSize )
{
	PGoBoard	pGoBoard;	// pointer to GoBoard mem. repr.
	HBRUSH		hbrGoBoard;	// the Go board itself
	HBRUSH		hbrLightLine;	// lines on the Go board
	HBRUSH		hbrDarkLine;	// shadow lines
	HBRUSH		hbrStone[3];	// color of stone for player i (1 or 2)
	HBRUSH		hbrPrev = NULL;	// previously selected brush
	int		r, c;		// row, column index
	int		x, y;		// window coordinates
	int		xMin, xMax;	// left-, right-most line positions
	int		yMin, yMax;	// top-, bottom-most line position
	int		iCell;		// cell contents (0=empty, 1,2=stone)
	int		iRadiusSpot;	// radius of each of the 9 spots
	int		iRadiusStone;	// radius of each stone
	int		dx, dy;

	/* lock the resource (unless there isn't one) */
	if (medid != NULL)
		pGoBoard = (PGoBoard) medLock(medid, MEDF_READ, 0L);

	/* create brushes for drawing Go board and stones */
	if ( ((hbrGoBoard = CreateSolidBrush(rgbGOBOARD)) == NULL) ||
	     ((hbrLightLine = CreateSolidBrush(rgbGOLIGHTLINE)) == NULL) ||
	     ((hbrDarkLine = CreateSolidBrush(rgbGODARKLINE)) == NULL) ||
	     ((hbrStone[1] = CreateSolidBrush(rgbGOPLAYER1)) == NULL) ||
	     ((hbrStone[2] = CreateSolidBrush(rgbGOPLAYER2)) == NULL) )
		goto AppPaint_EXIT;
	
	/* calculate extent of lines */
	xMin = ptClientSize.x / (GO_DIMEN + 1);
	xMax = (ptClientSize.x * GO_DIMEN) / (GO_DIMEN + 1);
	yMin = ptClientSize.y / (GO_DIMEN + 1);
	yMax = (ptClientSize.y * GO_DIMEN) / (GO_DIMEN + 1);

	/* calculate radius of stones and spots */
	dx = ptClientSize.x / (GO_DIMEN + 1) / 2,
	dy = ptClientSize.y / (GO_DIMEN + 1) / 2,
	iRadiusStone = min(dx, dy);
	dx = ptClientSize.x / SPOT_SHRINK;
	dy = ptClientSize.y / SPOT_SHRINK;
	iRadiusSpot = min(dx, dy);

	/* draw Go board itself */
	hbrPrev = SelectObject(hdc, hbrGoBoard);
	PatBlt(hdc, 0, 0, ptClientSize.x, ptClientSize.y, PATCOPY);

	/* if <medid> is null (i.e. there is no current resource) or if
	 * the medLock() failed, just leave the Go board drawn without
	 * lines or stones as an indication to the user
	 */
	if ((medid == NULL) || (pGoBoard == NULL))
		goto AppPaint_EXIT;

	/* draw dark lines */
	SelectObject(hdc, hbrDarkLine);
	for (r = 0; r < GO_DIMEN; r++)		// draw horizontal lines
		PatBlt(hdc, xMin - SHWD,
			(ptClientSize.y * (r + 1)) / (GO_DIMEN + 1) - SHWD,
			xMax - xMin + SHWD + 1, SHWD, PATCOPY);
	for (c = 0; c < GO_DIMEN; c++)		// draw vertical lines
		PatBlt(hdc, (ptClientSize.x * (c + 1)) / (GO_DIMEN + 1) - SHWD,
			yMin - SHWD, SHWD, yMax - yMin + SHWD + 1, PATCOPY);

	/* draw light lines */
	SelectObject(hdc, hbrLightLine);
	for (r = 0; r < GO_DIMEN; r++)		// draw horizontal lines
		PatBlt(hdc, xMin,
			(ptClientSize.y * (r + 1)) / (GO_DIMEN + 1),
			xMax - xMin + 1, 1, PATCOPY);
	for (c = 0; c < GO_DIMEN; c++)		// draw vertical lines
		PatBlt(hdc, (ptClientSize.x * (c + 1)) / (GO_DIMEN + 1),
			yMin, 1, yMax - yMin + 1, PATCOPY);
	
	/* draw the 9 spots */
	SelectObject(hdc, hbrDarkLine);
	for (r = 3; r < GO_DIMEN; r += 6)
	{
		y = (ptClientSize.y * (r + 1)) / (GO_DIMEN + 1);
		for (c = 3; c < GO_DIMEN; c += 6)
		{
			x = (ptClientSize.x * (c + 1)) / (GO_DIMEN + 1);

			/* draw spot centered at (x,y) */
			PatBlt(hdc, x - iRadiusSpot, y - iRadiusSpot,
				2 * iRadiusSpot, 2 * iRadiusSpot, PATCOPY);
		}
	}

	/* draw stones */
	for (r = 0; r < GO_DIMEN; r++)
	{
		y = (ptClientSize.y * (r + 1)) / (GO_DIMEN + 1);

		/* speed optimization: don't paint rows unnecessarily */
		if ((y + iRadiusStone < prcPaint->top) ||
		    (y - iRadiusStone > prcPaint->bottom))
			continue;

		for (c = 0; c < GO_DIMEN; c++)
		{
			x = (ptClientSize.x * (c + 1)) / (GO_DIMEN + 1);

			/* draw stone centered at (x,y) */
			if ((iCell = (*pGoBoard)[r][c]) != 0)
			{
				SelectObject(hdc, hbrStone[iCell]);
				Ellipse(hdc,
					x - iRadiusStone, y - iRadiusStone,
					x + iRadiusStone, y + iRadiusStone);
			}
		}
	}

AppPaint_EXIT:

	/* select the original brush before deleting any brush */
	if (hbrPrev != NULL)
		SelectObject(hdc, hbrPrev);

	/* delete brushes */
	if (hbrGoBoard != NULL)
		DeleteObject(hbrGoBoard);
	if (hbrLightLine != NULL)
		DeleteObject(hbrLightLine);
	if (hbrDarkLine != NULL)
		DeleteObject(hbrDarkLine);
	if (hbrStone[1] != NULL)
		DeleteObject(hbrStone[1]);
	if (hbrStone[2] != NULL)
		DeleteObject(hbrStone[2]);
	
	/* unlock the resource */
	if (medid != NULL)
		medUnlock(medid, MEDF_NOCHANGE, 0L, 0L);
}


/* AppClick(hwnd, medid, ptClick, fRight)
 *
 * The user clicked at point <ptClick> in window <hwnd>, that's currently
 * displaying Go board resource <medid>.  It is assumed that <medid> is
 * already accessed (loaded) if <medid> is not NULL.  <fRight> is TRUE
 * if the right mouse button was used, FALSE if the left button was used.
 */
VOID AppClick( HWND hwnd, MEDID medid, POINT ptClick, BOOL fRight )
{
	PGoBoard	pGoBoard;	// pointer to GoBoard mem. repr.
	int		r, c;		// row, column index
	int		x, y;		// window coordinates
	int		xCell, yCell;	// size of a Go board cell
	int		iCell;		// cell contents (0=empty, 1,2=stone)
	RECT		rcClientSize;	// size of client area of <hwnd>

	if (medid == NULL)
		return;			// no go board
	
	/* lock the resource */
	pGoBoard = (PGoBoard) medLock(medid, MEDF_WRITE, 0L);
	
	/* set (r,c) to the row/column that was clicked */
	GetClientRect(hwnd, &rcClientSize);
	xCell = rcClientSize.right / (GO_DIMEN + 1);
	yCell = rcClientSize.bottom / (GO_DIMEN + 1);
	r = (ptClick.y + yCell / 2) * (GO_DIMEN + 1) / rcClientSize.bottom - 1;
	r = (r < 0 ? 0 : (r >= GO_DIMEN ? (GO_DIMEN - 1) : r));
	c = (ptClick.x + xCell / 2) * (GO_DIMEN + 1) / rcClientSize.right - 1;
	c = (c < 0 ? 0 : (c >= GO_DIMEN ? (GO_DIMEN - 1) : c));

	/* change the cell at (r,c) -- on left-click, change:
	 *		empty to player 1
	 *	or...	player 1 to player 2
	 *	or...	player 2 to empty
	 * on right-click, change:
	 *		empty to player 2
	 *	or...	player 2 to player 1
	 *	or...	player 1 to empty
	 */
	iCell = (*pGoBoard)[r][c];
	if (fRight)
		iCell = (iCell + 2) % 3;	// cycle empty-player2-player1
	else
		iCell = (iCell + 1) % 3;	// cycle empty-player1-player2
	(*pGoBoard)[r][c] = (BYTE) iCell;

	/* unlock the resource -- this will automatically update the display;
	 * the DWORD of change information for a Go MED_CHANGE message contains
	 * the row number to update in the low word and the column number
	 * to update in the high word
	 */
	medUnlock(medid, MEDF_CHANGED, MAKELONG(r,c), 0L);
}


