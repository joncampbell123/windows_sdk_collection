/**[f******************************************************************
 * stubs.c - 
 *
 * Copyright (C) 1988 Aldus Corporation.  All rights reserved.
 * Copyright (C) 1989 Microsoft Corporation.
 * Company confidential.
 *
 **f]*****************************************************************/

/***************************************************************************
 * 22Jul87	msd		Module added for Windows 2.0 support.
 ***************************************************************************/
#include "pscript.h"
#include "driver.h"


int far PASCAL DeviceBitmap(lpdv, command, lpBitmap, lpBits)
	LPDV lpdv;
	int command;
	LPBITMAP lpBitmap;
	LPSTR lpBits;
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


void far PASCAL WEP(int fSystemExit)
{
}
