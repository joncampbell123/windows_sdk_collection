/* 
 * meduser.c  Procedure to process the change notification
 */

#include <windows.h>
#include <wincom.h>
#include <mediaman.h>
#include "medgo.h"
#include "go.h"


/* GoUserProc()
 *
 * This is the resource user procedure for this application.
 * The window handle of the window that this message is for
 * is in the dwInst instance data field of the resource user.
 *
 */
DWORD GoUserProc( HWND hwnd, MEDID medid, 
	MEDMSG medmsg, MEDINFO medinfo,
	LONG lParam, DWORD dwInst)
{
	RECT		rcClientSize;	// size of client area of <hwnd>
	int		r, c;		// row, column index
	int		x, y;		// window coordinates
	int		iRadiusStone;	// radius of each stone
	int		dx, dy;		// width, height of a cell
	RECT		rcUpdate;	// rectangle to update

	switch (medmsg)
	{

	case MED_CHANGE:
		/* row and column to update are stored in <lParam> */
		r = LOWORD(lParam);
		c = HIWORD(lParam);

		/* calculate radius of a stone */
		GetClientRect(hwnd, (LPRECT) &rcClientSize);
		dx = rcClientSize.right / (GO_DIMEN + 1) / 2,
		dy = rcClientSize.bottom / (GO_DIMEN + 1) / 2,
		iRadiusStone = min(dx, dy);

		/* calculate the position of a stone */
		x = (rcClientSize.right * (c + 1)) / (GO_DIMEN + 1);
		y = (rcClientSize.bottom * (r + 1)) / (GO_DIMEN + 1);

		/* calculate the rectangle in <hwnd> that needs updating */
		rcUpdate.left = x - iRadiusStone;
		rcUpdate.top = y - iRadiusStone;
		rcUpdate.right = x + iRadiusStone;
		rcUpdate.bottom = y + iRadiusStone;

		/* let Windows tell AppPaint() to redraw it */
		InvalidateRect(hwnd, (LPRECT) &rcUpdate, TRUE);
		break;

	default:

		return 0L;

	}
	return 1L;
}

