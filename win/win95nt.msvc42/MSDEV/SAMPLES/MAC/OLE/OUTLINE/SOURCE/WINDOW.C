/*****************************************************************************\
*                                                                             *
*    Window.c                                                                 *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#if !defined(_MSC_VER) && !defined(THINK_C)
#include "OLine.h"
#endif

#include "Types.h"

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#include "Debug.h"
#include "App.h"
#include "Window.h"
#if qFrameTools
	#include "Layers.h"
#endif
#include "Util.h"
#include "OleXcept.h"
#include <ToolUtils.h>

extern ApplicationPtr	gApplication;

static WinVtblPtr		gWinVtbl = nil;

//#pragma segment VtblInitSeg
WinVtblPtr WinGetVtbl()
{
	ASSERTCOND(gWinVtbl != nil);
	return gWinVtbl;
}

//#pragma segment VtblInitSeg
void WinInitVtbl()
{
	WinVtblPtr	vtbl;

	vtbl = gWinVtbl = (WinVtblPtr)NewPtrClear(sizeof(*vtbl));
	ASSERTCOND(vtbl != nil);
	FailNIL(vtbl);

	vtbl->m_DisposeProcPtr =			WinDispose;
	vtbl->m_FreeProcPtr =				WinFree;

	vtbl->m_DoActivateProcPtr =			WinDoActivate;
	vtbl->m_DoUpdateProcPtr = 			WinDoUpdate;
	vtbl->m_DoGrowProcPtr = 			WinDoGrow;
	vtbl->m_DoGrowWindowProcPtr = 		WinDoGrowWindow;
	vtbl->m_DoZoomProcPtr =				WinDoZoom;
	vtbl->m_DoZoomWindowProcPtr =		WinDoZoomWindow;
	vtbl->m_GetDragBoundsProcPtr =		WinGetDragBounds;
	vtbl->m_DoDragProcPtr =				WinDoDrag;
	vtbl->m_MoveWindowProcPtr =			WinMoveWindow;
	vtbl->m_ShiftWindowProcPtr =		WinShiftWindow;
	vtbl->m_DoContentProcPtr =			WinDoContent;

	vtbl->m_SetWindowTitleProcPtr =		WinSetWindowTitle;
	vtbl->m_GetWindowProcPtr =			WinGetWindow;
	vtbl->m_ShowProcPtr =				WinShow;
	vtbl->m_HideProcPtr =				WinHide;
	vtbl->m_IsVisibleProcPtr =			WinIsVisible;
	vtbl->m_BringToFrontProcPtr =		WinBringToFront;
	vtbl->m_DoIdleProcPtr = 			WinDoIdle;

	ASSERTCOND(ValidVtbl(vtbl, sizeof(*vtbl)));
}

//#pragma segment VtblDisposeSeg
void WinDisposeVtbl()
{
	ASSERTCOND(gWinVtbl != nil);
	DisposePtr((Ptr)gWinVtbl);
}

//#pragma segment WindowSeg
void WinInit(WinPtr pWin, short resID, Boolean floating)
{
	WindowPtr	theWindow;

#if qFrameTools
	WindowPtr	pSavedLayer,
				pLayer;
#endif

	pWin->vtbl = WinGetVtbl();

	pWin->m_GrowSizeRect = qd.screenBits.bounds;
	pWin->m_GrowSizeRect.top  = kMinimumWindowHeight;
	pWin->m_GrowSizeRect.left = kMinimumWindowWidth;

	pWin->m_fFloating = floating;

#if qFrameTools
	pLayer = (floating ? gApplication->m_pFloatLayer : gApplication->m_pDocLayer);
	pSavedLayer = SwapLayer(pLayer);
#endif

	if (gApplication->m_QuickdrawVersion == kOriginalQD)
		pWin->m_WindowPtr = theWindow = GetNewWindow(resID, nil, (WindowPtr)-1);
	else
		pWin->m_WindowPtr = theWindow = GetNewCWindow(resID, nil, (WindowPtr)-1);
	ASSERTCOND(theWindow != nil);

#if qFrameTools
	SetLayer(pSavedLayer);
#endif

	FailNIL(theWindow);
	SetPort(theWindow);

	SetWRefCon(theWindow, (long)pWin);
}

//#pragma segment WindowSeg
void WinDispose(WinPtr pWin)
{
	ASSERTCOND(pWin->m_WindowPtr != nil);
	DisposeWindow(pWin->m_WindowPtr);

	DisposePtr((Ptr)pWin);
}

//#pragma segment WindowSeg
void WinFree(WinPtr pWin)
{
	ASSERTCOND(pWin->vtbl->m_DisposeProcPtr != nil);
	(*pWin->vtbl->m_DisposeProcPtr)(pWin);
}

//#pragma segment WindowSeg
void WinDoActivate(WinPtr pWin, Boolean becomingActive)
{
}

//#pragma segment WindowSeg
void WinDoUpdate(WinPtr pWin)
{
}

//#pragma segment WindowSeg
long WinDoGrow(WinPtr pWin, EventRecord* theEvent)
{
	ASSERTCOND(*pWin->vtbl->m_DoGrowWindowProcPtr != nil);
	return (*pWin->vtbl->m_DoGrowWindowProcPtr)(pWin, theEvent);
}

//#pragma segment WindowSeg
long WinDoGrowWindow(WinPtr pWin, EventRecord* theEvent)
{
	long	newSize;

	newSize = GrowWindow(pWin->m_WindowPtr, theEvent->where, &pWin->m_GrowSizeRect);

	if (newSize != 0)
		SizeWindow(pWin->m_WindowPtr, LoWord(newSize), HiWord(newSize), true);

	return newSize;
}

//#pragma segment WindowSeg
void WinDoZoom(WinPtr pWin, short partCode)
{
	ASSERTCOND(*pWin->vtbl->m_DoZoomWindowProcPtr != nil);
	(*pWin->vtbl->m_DoZoomWindowProcPtr)(pWin, partCode);
}

//#pragma segment WindowSeg
void WinDoZoomWindow(WinPtr pWin, short partCode)
{
	GrafPtr		oldPort;

	GetPort(&oldPort);
	SetPort(pWin->m_WindowPtr);

	EraseRect(&pWin->m_WindowPtr->portRect);

#if qFrameTools
	if (partCode == inZoomOut)
	{
		Rect	rBounds;
		Rect	rIndent;

		rBounds = qd.screenBits.bounds;
		rBounds.top += GetMBarHeight() + 18;

		ASSERTCOND(gApplication->vtbl->m_GetBorderSpaceProcPtr != nil);
		(gApplication->vtbl->m_GetBorderSpaceProcPtr)(gApplication, &rIndent);

		rBounds.left   += rIndent.left   + 2;
		rBounds.top    += rIndent.top    + 2;
		rBounds.right  -= rIndent.right  + 1;
		rBounds.bottom -= rIndent.bottom + 1;

		(*(WStateDataHandle)(((WindowPeek)pWin->m_WindowPtr)->dataHandle))->stdState = rBounds;
	}
#endif

	ZoomWindow(pWin->m_WindowPtr, partCode, false);

	SetPort(oldPort);
}

//#pragma segment WindowSeg
void WinGetDragBounds(WinPtr pWin, Rect* rBounds)
{
	*rBounds = qd.screenBits.bounds;
}

//#pragma segment WindowSeg
void WinDoDrag(WinPtr pWin, EventRecord* theEvent, Boolean front)
{
	GrafPtr		oldPort;
	GrafPtr		wMgrPort;
	RgnHandle	tempRgn;
	RgnHandle	dragRgn;
	long		result;

	if (!StillDown())
		return;

	GetPort(&oldPort);
	GetWMgrPort(&wMgrPort);
	SetPort(wMgrPort);

	tempRgn = NewRgn();
	GetClip(tempRgn);

	SetClip(GetGrayRgn());

	dragRgn = NewRgn();
	CopyRgn(((WindowPeek)pWin->m_WindowPtr)->strucRgn, dragRgn);
	result = DragGrayRgn(dragRgn, theEvent->where, &qd.screenBits.bounds, &qd.screenBits.bounds, noConstraint, NULL);

	SetClip(tempRgn);
	DisposeRgn(tempRgn);
	DisposeRgn(dragRgn);

	SetPort(oldPort);

	if (result != 0x80008000)
	{
		Point		ptWhere;
		Boolean		covered;

		ptWhere = topLeft((**((WindowPeek)pWin->m_WindowPtr)->contRgn).rgnBBox);
		ptWhere.h += LoWord(result);
		ptWhere.v += HiWord(result);

#if qFrameTools
		ASSERTCOND(gApplication->vtbl->m_ToolsCoverWindowProcPtr != nil);
		if ((*gApplication->vtbl->m_ToolsCoverWindowProcPtr)(gApplication, pWin->m_WindowPtr, ptWhere.h, ptWhere.v))
			return;
#endif

		ASSERTCOND(pWin->vtbl->m_MoveWindowProcPtr != nil);
		(*pWin->vtbl->m_MoveWindowProcPtr)( pWin, ptWhere.h, ptWhere.v, front);
	}
}

//#pragma segment WindowSeg
void WinMoveWindow(WinPtr pWin, short h, short v, Boolean front)
{
	MoveWindow(pWin->m_WindowPtr, h, v, front);
}

//#pragma segment WindowSeg
void WinShiftWindow(WinPtr pWin, short hShift, short vShift)
{
	GrafPtr		oldPort;
	Point		ptCorner;

	GetPort(&oldPort);
	SetPort(pWin->m_WindowPtr);

	ptCorner.h = ptCorner.v = 0;
	LocalToGlobal(&ptCorner);

	SetPort(oldPort);

	ASSERTCOND(pWin->vtbl->m_MoveWindowProcPtr != nil);
	(*pWin->vtbl->m_MoveWindowProcPtr)(pWin, (short)(ptCorner.h + hShift), (short)(ptCorner.v + vShift), false);
}

//#pragma segment WindowSeg
void WinDoContent(WinPtr pWin, EventRecord* theEvent)
{
}

//#pragma segment WindowSeg
void WinSetWindowTitle(WinPtr pWin, StringPtr pTitle)
{
	ASSERTCOND(pWin->m_WindowPtr != nil);
	SetWTitle(pWin->m_WindowPtr, pTitle);
}

//#pragma segment WindowSeg
WindowPtr WinGetWindow(WinPtr pWin)
{
	return pWin->m_WindowPtr;
}

//#pragma segment WindowSeg
void WinShow(WinPtr pWin)
{
	ASSERTCOND(pWin->m_WindowPtr != nil);
	ShowWindow(pWin->m_WindowPtr);
}

//#pragma segment WindowSeg
void WinHide(WinPtr pWin)
{
	ASSERTCOND(pWin->m_WindowPtr != nil);
	HideWindow(pWin->m_WindowPtr);
}

//#pragma segment WindowSeg
Boolean WinIsVisible(WinPtr pWin)
{
	ASSERTCOND(pWin->m_WindowPtr != nil);
	return ((WindowPeek)pWin->m_WindowPtr)->visible;
}

//#pragma segment WindowSeg
void WinBringToFront(WinPtr pWin)
{
	// make sure we are the frontmost application
	ASSERTCOND(gApplication->vtbl->m_ShowProcPtr != nil);
	(*gApplication->vtbl->m_ShowProcPtr)(gApplication);

	SelectWindow(pWin->m_WindowPtr);
}

//#pragma segment WindowSeg
void WinDoIdle(WinPtr pWin)
{
}
