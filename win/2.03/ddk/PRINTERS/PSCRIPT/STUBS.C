/***************************************************************************
 * 22Jul87	msd		Module added for Windows 2.0 support.
 ***************************************************************************/
#include "pscript.h"


int far PASCAL DeviceBitmap(lpdv, command, lpBitmap, lpBits)
	LPDV lpdv;
	int command;
	BITMAP far *lpBitmap;
	BYTE far *lpBits;
	{
	return (0);
	}


int far PASCAL FastBorder(lpRect, borderWidth, borderDepth,
		rasterOp, lpdv, lpPBrush, lpDrawMode, lpClipRect)
	LPRECT lpRect;
	WORD borderWidth;
	WORD borderDepth;
	DWORD rasterOp;
	LPDV lpdv;
	LPSTR lpPBrush;
	LPDRAWMODE lpDrawMode;
	LPRECT lpClipRect;
	{
	return (0);
	}


int far PASCAL SetAttribute(lpdv, stateNum, index, attribute)
	LPDV lpdv;
	int stateNum;
	int index;
	int attribute;
	{
	return (0);
	}


