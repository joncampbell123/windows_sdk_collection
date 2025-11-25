/*****************************************************************************\
*                                                                             *
*    Toolbar.c                                                                *
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
#include "Toolbar.h"
#include "Const.h"
#include "Util.h"
#include "OleXcept.h"
#include <ToolUtils.h>
#include <Resources.h>

extern ApplicationPtr	gApplication;
static ToolbarVtblPtr		gToolbarVtbl = nil;

//#pragma segment VtblInitSeg
ToolbarVtblPtr ToolbarGetVtbl()
{
	ASSERTCOND(gToolbarVtbl != nil);
	return gToolbarVtbl;
}

//#pragma segment VtblInitSeg
void ToolbarInitVtbl()
{
	ToolbarVtblPtr	vtbl;

	vtbl = gToolbarVtbl = (ToolbarVtblPtr)NewPtrClear(sizeof(*vtbl));
	ASSERTCOND(vtbl != nil);
	FailNIL(vtbl);
		
	InheritFromVtbl(vtbl, WinGetVtbl());

	vtbl->m_DisposeProcPtr =			ToolbarDispose;

	vtbl->m_DoUpdateProcPtr = 			ToolbarDoUpdate;
	vtbl->m_ShowProcPtr =				ToolbarShow;
	vtbl->m_HideProcPtr =				ToolbarHide;
	vtbl->m_MoveWindowProcPtr =			ToolbarMoveWindow;

	vtbl->m_EnableProcPtr =				ToolbarEnable;
	vtbl->m_GetStateProcPtr =			ToolbarGetState;
	vtbl->m_SetStateProcPtr =			ToolbarSetState;
	vtbl->m_SpaceNeededProcPtr =		ToolbarSpaceNeeded;

	ASSERTCOND(ValidVtbl(vtbl, sizeof(*vtbl)));
}

//#pragma segment VtblDisposeSeg
void ToolbarDisposeVtbl()
{
	ASSERTCOND(gToolbarVtbl != nil);
	DisposePtr((Ptr)gToolbarVtbl);
}

//#pragma segment ToolbarSeg
void ToolbarInit(ToolbarPtr pToolbar, short resID)
{
	PicHandle	hToolsPict;

	WinInit((WinPtr)pToolbar, resID, true);

	pToolbar->baseVtbl = (WinVtblPtr)pToolbar->vtbl;
	pToolbar->vtbl = ToolbarGetVtbl();
	
	hToolsPict = GetPicture(kToolbar_PICT);
	ASSERTCOND(hToolsPict != nil);

	pToolbar->m_hToolsPict = hToolsPict;
	pToolbar->m_State = TOOLBAR_FLOATING;

	ASSERTCOND(pToolbar->vtbl->m_SetStateProcPtr != nil);
	(*pToolbar->vtbl->m_SetStateProcPtr)(pToolbar, TOOLBAR_TOP);

	// the toolbar window is never really active but we pretend
	// like it's always active by leaving it hilited.
	HiliteWindow(pToolbar->m_WindowPtr, true);

	SendBehind(pToolbar->m_WindowPtr, nil);

	ASSERTCOND(pToolbar->vtbl->m_SetWindowTitleProcPtr != nil);
	(*pToolbar->vtbl->m_SetWindowTitleProcPtr)(pToolbar, "\pToolbar");

	pToolbar->m_fVisible = !gApplication->m_InBackground;
}

//#pragma segment WindowSeg
void ToolbarDispose(ToolbarPtr pToolbar)
{
	if (pToolbar->m_hToolsPict != nil)
		ReleaseResource((Handle)pToolbar->m_hToolsPict);

	ASSERTCOND(pToolbar->baseVtbl->m_DisposeProcPtr != nil);
	(*pToolbar->baseVtbl->m_DisposeProcPtr)((WinPtr)pToolbar);
}

//#pragma segment WindowSeg
void ToolbarDoUpdate(ToolbarPtr pToolbar)
{
	Rect	rPict;

	rPict = (**pToolbar->m_hToolsPict).picFrame;
	DrawPicture(pToolbar->m_hToolsPict, &rPict);
}

//#pragma segment WindowSeg
void ToolbarShow(ToolbarPtr pToolbar)
{
	GrafPtr		oldPort;

	pToolbar->m_fVisible = true;

	ASSERTCOND(pToolbar->baseVtbl->m_ShowProcPtr != nil);
	(*pToolbar->baseVtbl->m_ShowProcPtr)((WinPtr)pToolbar);

	GetPort(&oldPort);
	SetPort(pToolbar->m_WindowPtr);

	ToolbarDoUpdate(pToolbar);
	ValidRect(&pToolbar->m_WindowPtr->portRect);

	SetPort(oldPort);
}

//#pragma segment WindowSeg
void ToolbarHide(ToolbarPtr pToolbar)
{
	pToolbar->m_fVisible = false;

	ASSERTCOND(pToolbar->baseVtbl->m_HideProcPtr != nil);
	(*pToolbar->baseVtbl->m_HideProcPtr)((WinPtr)pToolbar);
}

//#pragma segment WindowSeg
void ToolbarMoveWindow(ToolbarPtr pToolbar, short h, short v, Boolean front)
{
	if (pToolbar->m_State != TOOLBAR_FLOATING)
		return;

	ASSERTCOND(pToolbar->baseVtbl->m_MoveWindowProcPtr != nil);
	(*pToolbar->baseVtbl->m_MoveWindowProcPtr)((WinPtr)pToolbar, h, v, front);
}

//#pragma segment WindowSeg
void ToolbarEnable(ToolbarPtr pToolbar, Boolean fEnable)
{
	BORDERWIDTHS	borderWidths = { 0, 0, 0, 0 };

	if (fEnable == pToolbar->m_fEnabled)
		return;

	if (pToolbar->m_State == TOOLBAR_TOP)
	{
		if (fEnable)
			SetRect(&borderWidths, 0, (**pToolbar->m_hToolsPict).picFrame.bottom, 0, 0);
	}

	// REVIEW: need to handle toolbar at left of screen

	pToolbar->m_BorderWidths = borderWidths;
	pToolbar->m_fEnabled = fEnable;

	if (fEnable)
	{
		ASSERTCOND(pToolbar->vtbl->m_ShowProcPtr != nil);
		(*pToolbar->vtbl->m_ShowProcPtr)(pToolbar);
	}
	else
	{
		ASSERTCOND(pToolbar->vtbl->m_HideProcPtr != nil);
		(*pToolbar->vtbl->m_HideProcPtr)(pToolbar);
	}
}

//#pragma segment WindowSeg
TOOLBAR_STATE ToolbarGetState(ToolbarPtr pToolbar)
{
//	if (pToolbar->m_fEnabled)
		return pToolbar->m_State;
//	else
//		return TOOLBAR_DISABLED;
}

//#pragma segment WindowSeg
void ToolbarSetState(ToolbarPtr pToolbar, TOOLBAR_STATE state)
{
	PicHandle		hToolsPict = pToolbar->m_hToolsPict;
	BORDERWIDTHS	borderWidths;
	Boolean			fVisible;

	if (state == pToolbar->m_State)
		return;

	ASSERTCOND(state == TOOLBAR_FLOATING || state == TOOLBAR_TOP || state == TOOLBAR_BOTTOM);

	// if the current state is floating then we're about to doc the
	// toolbar so save the current position in global coordinates.
	if (pToolbar->m_State == TOOLBAR_FLOATING)
	{
		pToolbar->m_ptPosition = topLeft(pToolbar->m_WindowPtr->portRect);
		LocalToGlobal(&pToolbar->m_ptPosition);
	}

	ASSERTCOND(pToolbar->baseVtbl->m_IsVisibleProcPtr != nil);
	fVisible = (*pToolbar->baseVtbl->m_IsVisibleProcPtr)((WinPtr)pToolbar);

	if (fVisible)
		ShowHide(pToolbar->m_WindowPtr, false);

	switch (state)
	{
		case TOOLBAR_FLOATING:
			MoveWindow(pToolbar->m_WindowPtr, pToolbar->m_ptPosition.h, pToolbar->m_ptPosition.v, false);
			SizeWindow(pToolbar->m_WindowPtr, (**hToolsPict).picFrame.right, (**hToolsPict).picFrame.bottom, false);
			SetRect(&borderWidths, 0, 0, 0, 0);
			break;

		case TOOLBAR_TOP:
			MoveWindow(pToolbar->m_WindowPtr, 0, GetMBarHeight(), false);
			SizeWindow(pToolbar->m_WindowPtr, qd.screenBits.bounds.right, (**hToolsPict).picFrame.bottom, false);
			SetRect(&borderWidths, 0, (**hToolsPict).picFrame.bottom, 0, 0);
			break;

		case TOOLBAR_BOTTOM:
			MoveWindow(pToolbar->m_WindowPtr, 0, (short)(qd.screenBits.bounds.bottom - (**hToolsPict).picFrame.bottom), false);
			SizeWindow(pToolbar->m_WindowPtr, qd.screenBits.bounds.right, (**hToolsPict).picFrame.bottom, false);
			SetRect(&borderWidths, 0, 0, 0, (**hToolsPict).picFrame.bottom);
			break;
	}

	if (fVisible)
		ShowHide(pToolbar->m_WindowPtr, true);

	pToolbar->m_State = state;
}

//#pragma segment WindowSeg
void ToolbarSpaceNeeded(ToolbarPtr pToolbar, Rect* prectSpace)
{
	Rect	rSpace = { 0, 0, 0, 0 };

	if (pToolbar->m_State == TOOLBAR_TOP)
		SetRect(&rSpace, 0, (**pToolbar->m_hToolsPict).picFrame.bottom, 0, 0);

	*prectSpace = rSpace;
}
