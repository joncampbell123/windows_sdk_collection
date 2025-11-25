/*****************************************************************************\
*                                                                             *
*    Gopher.c                                                                 *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#if !defined(_MSC_VER) && !defined(THINK_C)
#include "OLine.h"
#endif

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#include "Debug.h"
#include "Gopher.h"
#include "App.h"
#include "OleXcept.h"

GopherPtr				gGopher = nil;
extern ApplicationPtr	gApplication;

//#pragma segment GopherSeg
GopherPtr GopherNewUpdateMenus(UpdateMenusProcPtr pUpdateMenusProcPtr, void* pUserData)
{
	GopherPtr	theGopher;

	theGopher = (GopherPtr)NewPtrClear(sizeof(GopherRec));
	if (theGopher != nil)
	{
		theGopher->m_Type = kUpdateMenusType;
		theGopher->m_UserData = pUserData;

		theGopher->m_Proc.m_UpdateMenusProcPtr = pUpdateMenusProcPtr;
	}

	return theGopher;
}

//#pragma segment GopherSeg
GopherPtr GopherNewFilterCmd(FilterCmdProcPtr pFilterCmdProcPtr, void* pUserData)
{
	GopherPtr	theGopher;

	theGopher = (GopherPtr)NewPtrClear(sizeof(GopherRec));
	if (theGopher != nil)
	{
		theGopher->m_Type = kFilterCmdType;
		theGopher->m_UserData = pUserData;

		theGopher->m_Proc.m_FilterCmdProcPtr = pFilterCmdProcPtr;
	}

	return theGopher;
}

//#pragma segment GopherSeg
GopherPtr GopherNewDoCmd(DoCmdProcPtr pDoCmdProcPtr, void* pUserData)
{
	GopherPtr	theGopher;

	theGopher = (GopherPtr)NewPtrClear(sizeof(GopherRec));
	if (theGopher != nil)
	{
		theGopher->m_Type = kDoCmdType;
		theGopher->m_UserData = pUserData;

		theGopher->m_Proc.m_DoCmdProcPtr = pDoCmdProcPtr;
	}

	return theGopher;
}

//#pragma segment GopherSeg
GopherPtr GopherNewAdjustCursor(AdjustCursorProcPtr pAdjustCursorProcPtr, void* pUserData)
{
	GopherPtr	theGopher;

	theGopher = (GopherPtr)NewPtrClear(sizeof(GopherRec));
	if (theGopher != nil)
	{
		theGopher->m_Type = kAdjustCusrorType;
		theGopher->m_UserData = pUserData;

		theGopher->m_Proc.m_AdjustCursorProcPtr = pAdjustCursorProcPtr;
	}

	return theGopher;
}

//#pragma segment GopherSeg
void GopherDispose(GopherPtr theGopher)
{
	if (theGopher->m_NextGopher != nil)
		GopherRemove(theGopher);

	DisposePtr((Ptr)theGopher);
}

//#pragma segment GopherSeg
void GopherAdd(GopherPtr theGopher)
{
	ASSERTCOND(theGopher->m_NextGopher == nil && theGopher->m_PreviousGopher == nil);

	theGopher->m_PreviousGopher = nil;

	if (gGopher == nil)
		theGopher->m_NextGopher = nil;
	else
	{
		theGopher->m_NextGopher = gGopher;
		gGopher->m_PreviousGopher = theGopher;
	}

	gGopher = theGopher;
}

//#pragma segment GopherSeg
void GopherRemove(GopherPtr theGopher)
{
	if (theGopher->m_NextGopher != nil)
		theGopher->m_NextGopher->m_PreviousGopher = theGopher->m_PreviousGopher;

	if (theGopher->m_PreviousGopher == nil)
	{
		ASSERTCOND(gGopher == theGopher);
		gGopher = theGopher->m_NextGopher;
	}
	else
		theGopher->m_PreviousGopher->m_NextGopher = theGopher->m_NextGopher;

	theGopher->m_NextGopher = nil;
	theGopher->m_PreviousGopher = nil;
}

//#pragma segment GopherSeg
void GopherUpdateMenus(GopherPtr theGopher)
{
	while(theGopher != nil && theGopher->m_Type != kUpdateMenusType)
		theGopher = theGopher->m_NextGopher;

	if (theGopher == nil)
		return;

	ASSERTCOND(theGopher->m_Proc.m_UpdateMenusProcPtr != nil);
	(*theGopher->m_Proc.m_UpdateMenusProcPtr)(theGopher->m_UserData);
}

//#pragma segment GopherSeg
void GopherFilterCmd(GopherPtr theGopher, long cmd)
{
	while(theGopher != nil && theGopher->m_Type != kFilterCmdType)
		theGopher = theGopher->m_NextGopher;

	if (theGopher == nil)
		return;

	ASSERTCOND(theGopher->m_Proc.m_FilterCmdProcPtr != nil);
	(*theGopher->m_Proc.m_FilterCmdProcPtr)(theGopher->m_UserData, cmd);
}

//#pragma segment GopherSeg
void GopherDoCmd(GopherPtr theGopher, long cmd)
{
	while(theGopher != nil && theGopher->m_Type != kDoCmdType)
		theGopher = theGopher->m_NextGopher;

	if (theGopher == nil)
		return;

	ASSERTCOND(theGopher->m_Proc.m_DoCmdProcPtr != nil);
	(*theGopher->m_Proc.m_DoCmdProcPtr)(theGopher->m_UserData, cmd);
}

//#pragma segment GopherSeg
void GopherAdjustCursor(GopherPtr theGopher)
{
	while(theGopher != nil && theGopher->m_Type != kAdjustCusrorType)
		theGopher = theGopher->m_NextGopher;

	if (theGopher == nil)
		return;

	ASSERTCOND(theGopher->m_Proc.m_AdjustCursorProcPtr != nil);
	(*theGopher->m_Proc.m_AdjustCursorProcPtr)(theGopher->m_UserData);
}

//#pragma segment GopherSeg
void UpdateMenus(void)
{
	ASSERTCOND(gApplication->vtbl->m_PreUpdateMenusProcPtr != nil);
	(*gApplication->vtbl->m_PreUpdateMenusProcPtr)(gApplication);

	GopherUpdateMenus(gGopher);
}

//#pragma segment GopherSeg
void DoCmd(long cmd)
{
	TRY
	{
		GopherFilterCmd(gGopher, cmd);
		GopherDoCmd(gGopher, cmd);
	}
	CATCH
	{
		SysBeep(1);
		
		NO_PROPAGATE;
	}
	ENDTRY
}
//#pragma segment GopherSeg
void PostCmd(long cmd)
{
	AppleEvent				theAppleEvent;
	AEAddressDesc			theTarget;
	ProcessSerialNumber 	targetPSN;
	OSErr					err;

	GetCurrentProcess(&targetPSN);

	err = AECreateDesc(typeProcessSerialNumber, (Ptr)&targetPSN, sizeof(ProcessSerialNumber), &theTarget);
	if (err == noErr)
	{
		err = AECreateAppleEvent(kAEOutlineEvent, kAEDoCmd, &theTarget, kAutoGenerateReturnID, 0, &theAppleEvent);
		if (err == noErr)
		{
			err = AEPutParamPtr(&theAppleEvent, keyDirectObject, typeLongInteger, (Ptr)&cmd, sizeof(cmd));

			if (err == noErr)
				err = AESend(&theAppleEvent, nil, kAENoReply, kAENormalPriority + kAEAlwaysInteract, 500, nil, nil);

			AEDisposeDesc(&theAppleEvent);
		}

		AEDisposeDesc(&theTarget);
	}

	if (err != noErr)
	{
		ASSERTCONDSZ(0, "Unable to post asynchronous command");
		DoCmd(cmd);
	}
}
