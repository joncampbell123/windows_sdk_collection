/*****************************************************************************\
*                                                                             *
*    Application.c                                                            *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#if !defined(THINK_C)  && !defined(_MSC_VER)
#include "OLine.h"
#endif

#include <Types.h>

#if !defined(THINK_C)
#include <ToolUtils.h>
#include <StandardFile.h>
#include <Errors.h>
#include <stdio.h>
#include <Fonts.h>
#include <Resources.h>
#include <Desk.h>
#include <LowMem.h>

#endif

#if defined(USEHEADER)
#include "OleHdrs.h"
#endif
#include "Debug.h"
#include "App.h"
#include "Doc.h"
#include "Const.h"
#include "Gopher.h"
#include "Util.h"
#if qFrameTools
	#include "Layers.h"
	#include "Toolbar.h"
#endif
#include "OleXcept.h"
#if qOle
#include "DXferDoc.h"
#include "OleDebug.h"
#if qOleContainerApp
	#include "Line.h"
#endif

#endif //qOle

#if !defined(_MSC_VER) && !defined(THINK_C) && !defined(__MWERKS__)
	// from MPW standard library
	void _DataInit();
#endif

#ifdef __powerc
static GrowZoneUPP gMyGrowZoneProc = NULL;
#endif

#ifdef __PPCC__
#include "LowMem.h"
#include "ToolUtils.h"

   QDGlobals		qd;
#endif

// OLDNAME: Application.c

ApplicationPtr			gApplication = nil;

CursHandle				gWatchCursor = nil;
CursHandle				gIBeamCursor = nil;

unsigned int			gDocumentSequence = 1;

#ifndef _MSC_VER
static pascal OSErr AppAEOpenDocuments(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp);
static pascal OSErr AppAEOpenApplication(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp);
static pascal OSErr AppAEPrintDocuments(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp);
static pascal OSErr AppAEQuitApplication(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp);
static pascal OSErr AppAEDoCmd(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp);
#else
static OSErr __pascal AppAEOpenDocuments(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp);
static OSErr __pascal AppAEOpenApplication(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp);
static OSErr __pascal AppAEPrintDocuments(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp);
static OSErr __pascal AppAEQuitApplication(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp);
static OSErr __pascal AppAEDoCmd(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp);
#endif

static ApplicationVtblPtr		gAppVtbl = nil;

//#pragma segment VtblInitSeg
ApplicationVtblPtr AppGetVtbl()
{
	ASSERTCOND(gAppVtbl != nil);
	return gAppVtbl;
}

//#pragma segment VtblInitSeg
void AppInitVtbl()
{
	ApplicationVtblPtr	vtbl;

	vtbl = gAppVtbl = (ApplicationVtblPtr)NewPtrClear(sizeof(*vtbl));
	ASSERTCOND(vtbl != nil);
	FailNIL(vtbl);
		
	vtbl->m_DisposeProcPtr =				AppDispose;
	vtbl->m_FreeProcPtr =					AppFree;

	vtbl->m_DoStartupProcPtr =				AppDoStartup;
	vtbl->m_DoDefaultStartupActionProcPtr =	AppDoDefaultStartupAction;
	
	vtbl->m_ReadPreferencesProcPtr =		AppReadPreferences;
	vtbl->m_WritePreferencesProcPtr =		AppWritePreferences;

	vtbl->m_EventLoopProcPtr =				AppEventLoop;
	vtbl->m_IsReadyToQuitProcPtr =			AppIsReadyToQuit;
	vtbl->m_IsFrontWindowProcPtr =			AppIsFrontWindow;
	vtbl->m_DoMouseDownProcPtr =			AppDoMouseDown;
	vtbl->m_DoMouseUpProcPtr =				AppDoMouseUp;
	vtbl->m_DoKeyDownProcPtr = 				AppDoKeyDown;
	vtbl->m_DoUpdateEventProcPtr =			AppDoUpdateEvent;
	vtbl->m_DoDiskEventProcPtr =			AppDoDiskEvent;
	vtbl->m_DoActivateEventProcPtr =		AppDoActivateEvent;
	vtbl->m_DoOSEventProcPtr = 				AppDoOSEvent;
	vtbl->m_HighLevelEventProcPtr =			AppHighLevelEvent;
	vtbl->m_UpdateCurrentDocProcPtr =		AppUpdateCurrentDoc;
	vtbl->m_SetIdleCursorProcPtr =			AppSetIdleCursor;
	vtbl->m_DoIdleProcPtr = 				AppDoIdle;
	vtbl->m_ProcessEventProcPtr = 			AppProcessEvent;

	vtbl->m_CreateDocProcPtr =				AppCreateDoc;
	vtbl->m_DoNewProcPtr =					AppDoNew;
	vtbl->m_DoOpenProcPtr =					AppDoOpen;
	vtbl->m_OpenADocProcPtr =				AppOpenADoc;
	vtbl->m_PositionNewDocProcPtr =			AppPositionNewDoc;
	vtbl->m_GetFrontDocWindowProcPtr =		AppGetFrontDocWindow;
	vtbl->m_GetCurrentDocProcPtr =			AppGetCurrentDoc;
	vtbl->m_FindWinProcPtr =				AppFindWin;
	vtbl->m_FindDocProcPtr =				AppFindDoc;
	vtbl->m_DocFromFSSpecProcPtr =			AppDocFromFSSpec;
	vtbl->m_GetNextNewFileNameProcPtr =		AppGetNextNewFileName;
	vtbl->m_DoAboutProcPtr =				AppDoAbout;

	vtbl->m_MenuItemToCmdProcPtr =			AppMenuItemToCmd;
	vtbl->m_CmdToMenuItemProcPtr =			AppCmdToMenuItem;
	vtbl->m_GetCmdItemProcPtr =				AppGetCmdItem;
	vtbl->m_SetCmdItemProcPtr =				AppSetCmdItem;
	vtbl->m_EnableCmdProcPtr =				AppEnableCmd;
	vtbl->m_CheckCmdProcPtr =				AppCheckCmd;
	vtbl->m_PreUpdateMenusProcPtr =			AppPreUpdateMenus;
	vtbl->m_DoUnknownMenuKeyProcPtr =		AppDoUnknownMenuKey;
	vtbl->m_DoUnknownMenuItemProcPtr =		AppDoUnknownMenuItem;
	vtbl->m_GetWindowsMenuHandleProcPtr =	AppGetWindowsMenuHandle;
	vtbl->m_HideOthersCmdProcPtr =			AppHideOthersCmd;

	vtbl->m_DoSuspendProcPtr =				AppDoSuspend;
	vtbl->m_DoResumeProcPtr =				AppDoResume;

	vtbl->m_DoMouseInSysWindowProcPtr =		AppDoMouseInSysWindow;
	vtbl->m_DoGoAwayProcPtr =				AppDoGoAway;
	vtbl->m_DoDragProcPtr =					AppDoDrag;

	vtbl->m_DoQuitProcPtr =					AppDoQuit;
	vtbl->m_FlushClipboardProcPtr = 		AppFlushClipboard;

	vtbl->m_GetFrameRectProcPtr =			AppGetFrameRect;
	vtbl->m_GetBorderSpaceProcPtr =			AppGetBorderSpace;
	vtbl->m_SetBorderSpaceProcPtr =			AppSetBorderSpace;
	vtbl->m_ShiftAllWindowsProcPtr =		AppShiftAllWindows;

#if qFrameTools
	vtbl->m_CreateFrameToolsProcPtr =		AppCreateFrameTools;
	vtbl->m_DisposeFrameToolsProcPtr =		AppDisposeFrameTools;
	vtbl->m_FrameSpaceNeededProcPtr =		AppFrameSpaceNeeded;
	vtbl->m_IsClickInFrameToolsProcPtr =	AppIsClickInFrameTools;
	vtbl->m_FloatWindowsStrucRgnsProcPtr =	AppFloatWindowsStrucRgns;
	vtbl->m_ShowFrameToolsProcPtr =			AppShowFrameTools;
	vtbl->m_HideFrameToolsProcPtr =			AppHideFrameTools;
	vtbl->m_ToolsCoverWindowProcPtr =		AppToolsCoverWindow;
#endif

	vtbl->m_HideProcPtr =					AppHide;
	vtbl->m_ShowProcPtr =					AppShow;

	vtbl->m_WaitContextSwitchProcPtr =		AppWaitContextSwitch;
	
	vtbl->m_GetCurrentEventRecordProcPtr =	AppGetCurrentEventRecord;
	
	ASSERTCOND(ValidVtbl(vtbl, sizeof(*vtbl)));
}

//#pragma segment VtblDisposeSeg
void AppDisposeVtbl()
{
	ASSERTCOND(gAppVtbl != nil);
	DisposePtr((Ptr)gAppVtbl);
}

//#pragma segment ApplicationInitSeg
void AppInit(ApplicationPtr pApp, OSType creator)
{
	// initialize Mac Toolbox components
	InitGraf((Ptr) &qd.thePort);
	InitFonts();
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(0L);
	InitCursor();

#if !defined(_MSC_VER) && !defined(THINK_C) && !defined(__MWERKS__)
	// Unload data segment: note that _DataInit must not be in Main!
#if !defined(_PPCMAC)
	UnloadSeg((ProcPtr) _DataInit);
#endif
#endif

	// incrase stack space by kStackNeeded bytes
	SetApplLimit((Ptr)((long)GetApplLimit() - (kStackNeeded*1024)));
	MaxApplZone();

	gWatchCursor = GetCursor(watchCursor);
	ASSERTCOND(gWatchCursor != nil);
	FailNIL(gWatchCursor);

	gIBeamCursor = GetCursor(iBeamCursor);
	ASSERTCOND(gIBeamCursor != nil);
	FailNIL(gIBeamCursor);

	SetCursor(*gWatchCursor);

	pApp->vtbl = AppGetVtbl();

	ASSERTCOND(pApp->vtbl->m_ReadPreferencesProcPtr != nil);
	(*pApp->vtbl->m_ReadPreferencesProcPtr)(pApp);

	pApp->m_GopherUpdateMenus = GopherNewUpdateMenus((UpdateMenusProcPtr)AppUpdateMenus, pApp);
	FailNIL(pApp->m_GopherUpdateMenus);
	GopherAdd(pApp->m_GopherUpdateMenus);

	pApp->m_GopherDoCmd = GopherNewDoCmd((DoCmdProcPtr)AppDoCmd, pApp);
	FailNIL(pApp->m_GopherDoCmd);
	GopherAdd(pApp->m_GopherDoCmd);

	pApp->m_GopherAdjustCursor = GopherNewAdjustCursor((AdjustCursorProcPtr)AppAdjustCursor, pApp);
	FailNIL(pApp->m_GopherAdjustCursor);
	GopherAdd(pApp->m_GopherAdjustCursor);

	pApp->m_QuickdrawVersion = QuickdrawVersion();
	pApp->m_HaveWaitNextEvent = WaitNextEventAvailable();

	pApp->m_Creator = creator;
	pApp->m_fIsReadyToQuit = false;
	pApp->m_InBackground = false;
	pApp->m_MouseRgn = nil;
	pApp->m_WhichWindow = nil;
	pApp->m_CurDoc = nil;
	
	pApp->m_SleepVal = 0;					// a small value

	pApp->m_NumTypes = -1;					// all types
	pApp->m_pFileFilter = nil;

	pApp->m_MenuCommands = Get1Resource(kMenuItemToCmdResType, kMenuItemToCmdResID);
	FailNIL(pApp->m_MenuCommands);

#if qFrameTools
	pApp->m_pDocLayer = nil;
	pApp->m_pFloatLayer = nil;
	pApp->m_pToolbar = nil;

	// Store original layer
	pApp->m_pRootLayer = GetLayer();

	// Create always active document layer.
	if (NewLayer(&pApp->m_pDocLayer, true, false, (WindowPtr)-1, 0) != noErr)
		ASSERTCONDSZ(0, "Failed in call to NewLayer");

	// Create never active floating layer in front of document layer.
	if (NewLayer(&pApp->m_pFloatLayer, true, true, (WindowPtr)-1, 0) != noErr)
		ASSERTCONDSZ(0, "Failed in call to NewLayer");

	ASSERTCOND(pApp->vtbl->m_CreateFrameToolsProcPtr != nil);
	(*pApp->vtbl->m_CreateFrameToolsProcPtr)(pApp);
#endif

	pApp->m_pIdleCursor = &qd.arrow;
}

//#pragma segment ApplicationSeg
void AppDispose(ApplicationPtr pApp)
{
	GopherDispose(pApp->m_GopherUpdateMenus);
	GopherDispose(pApp->m_GopherDoCmd);
	GopherDispose(pApp->m_GopherAdjustCursor);
	
#if qFrameTools
	ASSERTCOND(pApp->vtbl->m_DisposeFrameToolsProcPtr != nil);
	(*pApp->vtbl->m_DisposeFrameToolsProcPtr)(pApp);
#endif

	ASSERTCOND(pApp->vtbl->m_WritePreferencesProcPtr != nil);
	(*pApp->vtbl->m_WritePreferencesProcPtr)(pApp);

	gApplication = nil;
	DisposePtr((Ptr)pApp);
}

//#pragma segment ApplicationSeg
void AppFree(ApplicationPtr pApp)
{
	ASSERTCOND(pApp->vtbl->m_DisposeProcPtr != nil);
	(*pApp->vtbl->m_DisposeProcPtr)(pApp);
}

//#pragma segment ApplicationInitSeg
void AppDoDefaultStartupAction(ApplicationPtr pApp)
{
	// Do nothing if there's already a window open
	if (FrontWindow())
		return;

	if (IsOptionKeyDown())
		DoCmd(cmdOpen);
	else
		DoCmd(cmdNew);
}

//#pragma segment ApplicationInitSeg
void AppReadPreferences(ApplicationPtr pApp)
{
#if qAssert
	short		vRefNum;
	long		dirID;
	short		refNum;
	long		count;
	OSErr		err;

	if (FindPrefFolder(&vRefNum, &dirID) != noErr)
		return;

	err = HOpen(vRefNum, dirID, kPrefFileName, fsRdWrPerm, &refNum);
	if (err == noErr)
	{
		count = sizeof(gEnableAsserts);
		FSRead(refNum, &count, (Ptr)&gEnableAsserts);
		FSClose(refNum);
	}
#endif
}

//#pragma segment ApplicationInitSeg
void AppWritePreferences(ApplicationPtr pApp)
{
#if qAssert
	short		vRefNum;
	long		dirID;
	short		refNum;
	long		count;
	OSErr		err;

	if (FindPrefFolder(&vRefNum, &dirID) != noErr)
		return;

	HDelete(vRefNum, dirID, kPrefFileName);

	err = HCreate(vRefNum, dirID, kPrefFileName, kOutlineType, 'pref');
	if (err == noErr)
	{
		err = HOpen(vRefNum, dirID, kPrefFileName, fsRdWrPerm, &refNum);
		if (err == noErr)
		{
			count = sizeof(gEnableAsserts);
			FSWrite(refNum, &count, (Ptr)&gEnableAsserts);
			FSClose(refNum);
		}
	}
#endif
}

//#pragma segment ApplicationInitSeg
void AppDoStartup(ApplicationPtr pApp)
{
	if (AppleEventsInstalled())
	{
		AEInstallEventHandler(kCoreEventClass, kAEOpenApplication, NewAEEventHandlerProc(AppAEOpenApplication), (long)pApp, false);
		AEInstallEventHandler(kCoreEventClass, kAEOpenDocuments, NewAEEventHandlerProc(AppAEOpenDocuments), (long)pApp, false);
		AEInstallEventHandler(kCoreEventClass, kAEPrintDocuments, NewAEEventHandlerProc(AppAEPrintDocuments), (long)pApp, false);
		AEInstallEventHandler(kCoreEventClass, kAEQuitApplication, NewAEEventHandlerProc(AppAEQuitApplication), (long)pApp, false);
		AEInstallEventHandler(kAEOutlineEvent, kAEDoCmd, NewAEEventHandlerProc(AppAEDoCmd), (long)pApp, false);
	}
#ifdef OBSOLETE
	else
	{
		short	messages;
		short	numFiles;
		short	curFile;

		CountAppFiles(&messages, &numFiles);

		if (numFiles == 0)
		{
			ASSERTCOND(pApp->vtbl->m_DoDefaultStartupActionProcPtr != nil);
			(*pApp->vtbl->m_DoDefaultStartupActionProcPtr)(pApp);
		}
		else
		{
			for (curFile = 1; curFile <= numFiles; curFile++)
			{
				AppFile		fileInfo;

				// get the info
				GetAppFiles(curFile, &fileInfo);

				if (messages != appPrint)
				{
					TRY
					{
						FSSpec	spec;

						spec.vRefNum = fileInfo.vRefNum;
						spec.parID = 0;
						pStrCopy(spec.name, fileInfo.fName);

						ASSERTCOND(pApp->vtbl->m_OpenADocProcPtr != nil);
						(*pApp->vtbl->m_OpenADocProcPtr)(pApp, &spec, fileInfo.fType);
					}
					CATCH
					{
						NO_PROPAGATE;
					}
					ENDTRY
				}
				ClrAppFiles(curFile);
			}
		}
	}
#endif // OBSOLETE
}


//#pragma segment ApplicationSeg
Boolean AppIsReadyToQuit(ApplicationPtr pApp)
{
	return pApp->m_fIsReadyToQuit;
}

//#pragma segment ApplicationSeg
void AppEventLoop(ApplicationPtr pApp)
{
	EventRecord		theEvent;
#ifdef __powerc
	ToolbarPtr		pToolbar;
#endif
	
	ASSERTCOND(pApp->vtbl->m_DoIdleProcPtr != nil);
	(*pApp->vtbl->m_DoIdleProcPtr)(pApp);

	ASSERTCOND(pApp->vtbl->m_IsReadyToQuitProcPtr != nil);

	do
	{
		Boolean		haveEvent;

#ifdef __powerc

// since we aren't using the layer manager on the PPC, we need to make sure
// that the toolbar is always in back (or bad things will happen...)
    	pToolbar = pApp->m_pToolbar;

		SendBehind (pToolbar->m_WindowPtr, nil);
#endif

		ASSERTCOND(pApp->vtbl->m_UpdateCurrentDocProcPtr != nil);
		(*pApp->vtbl->m_UpdateCurrentDocProcPtr)(pApp);

		AdjustCursor();

		if (pApp->m_HaveWaitNextEvent)
		{
			haveEvent = WaitNextEvent(everyEvent, &theEvent,
							pApp->m_SleepVal, pApp->m_MouseRgn);
		}
		else
		{
			SystemTask();
			haveEvent = GetNextEvent(everyEvent, &theEvent);
		}

		// process event if we received one
		if (haveEvent)
		{
			ASSERTCOND(pApp->vtbl->m_ProcessEventProcPtr != nil);
			(*pApp->vtbl->m_ProcessEventProcPtr)(pApp, &theEvent);
		}
		
		if (!haveEvent /* || pApp->m_fNeedsIdle */)
		{
			ASSERTCOND(pApp->vtbl->m_DoIdleProcPtr != nil);
			(*pApp->vtbl->m_DoIdleProcPtr)(pApp);
			
			pApp->m_fNeedsIdle = false;
		}
		
	} while (!(*pApp->vtbl->m_IsReadyToQuitProcPtr)(pApp));
}

//#pragma segment ApplicationSeg
void AppProcessEvent(ApplicationPtr pApp, EventRecord* pEvent)
{
	ApplicationVtblPtr	vtbl;

	// make sure we know what the front window is for the given event
	ASSERTCOND(pApp->vtbl->m_UpdateCurrentDocProcPtr != nil);
	(*pApp->vtbl->m_UpdateCurrentDocProcPtr)(pApp);

	pApp->m_TheEvent = *pEvent;
	
	vtbl = pApp->vtbl;

	if (pApp->m_TheEvent.what == mouseDown || pApp->m_TheEvent.what == keyDown     || pApp->m_TheEvent.what == autoKey ||
		pApp->m_TheEvent.what == diskEvt   || pApp->m_TheEvent.what == activateEvt || pApp->m_TheEvent.what == osEvt)
		AdjustCursor();

	switch(pApp->m_TheEvent.what)
	{
		case mouseDown:
			ASSERTCOND(vtbl->m_DoMouseDownProcPtr != nil);
			(*vtbl->m_DoMouseDownProcPtr)(pApp);
			break;
		case mouseUp:
			ASSERTCOND(vtbl->m_DoMouseUpProcPtr != nil);
			(*vtbl->m_DoMouseUpProcPtr)(pApp);
			break;
		case keyDown:
		case autoKey:
			ASSERTCOND(vtbl->m_DoKeyDownProcPtr != nil);
			(*vtbl->m_DoKeyDownProcPtr)(pApp);
			break;
		case updateEvt:
			ASSERTCOND(vtbl->m_DoUpdateEventProcPtr != nil);
			(*vtbl->m_DoUpdateEventProcPtr)(pApp);
			break;
		case diskEvt:
			ASSERTCOND(vtbl->m_DoDiskEventProcPtr != nil);
			(*vtbl->m_DoDiskEventProcPtr)(pApp);
			break;
		case activateEvt:
			ASSERTCOND(vtbl->m_DoActivateEventProcPtr != nil);
			(*vtbl->m_DoActivateEventProcPtr)(pApp);
			break;
		case osEvt:
			ASSERTCOND(vtbl->m_DoOSEventProcPtr != nil);
			(*vtbl->m_DoOSEventProcPtr)(pApp);
			break;
		case kHighLevelEvent:
			ASSERTCOND(vtbl->m_HighLevelEventProcPtr != nil);
			(*vtbl->m_HighLevelEventProcPtr)(pApp);
			break;
	}
}

//#pragma segment ApplicationSeg
Boolean AppIsFrontWindow(ApplicationPtr pApp, WinPtr pWin)
{
	if (pWin->m_fFloating)
		return true;

	return (pWin->m_WindowPtr == FrontWindow());
}

//#pragma segment ApplicationSeg
void AppDoMouseDown(ApplicationPtr pApp)
{
	short				partCode;
	WindowPtr			theWindow;
	EventRecord			theEvent;
	ApplicationVtblPtr	vtbl;

	vtbl = pApp->vtbl;

	partCode = FindWindow(pApp->m_TheEvent.where, &theWindow);
	if (theWindow)
	{
		pApp->m_WhichWindow = theWindow;

		ASSERTCOND(vtbl->m_FindDocProcPtr != nil);
		pApp->m_CurDoc = (*vtbl->m_FindDocProcPtr)(pApp, theWindow);
	}
	theEvent = pApp->m_TheEvent;

	switch(partCode)
	{
		case inSysWindow:
			ASSERTCOND(vtbl->m_DoMouseInSysWindowProcPtr != nil);
			(*vtbl->m_DoMouseInSysWindowProcPtr)(pApp);
			break;
		case inMenuBar:
			{
				long	mResult;

				UpdateMenus();
				mResult = MenuSelect(theEvent.where);
				if (mResult != 0)
				{
					long	cmd;

					ASSERTCOND(vtbl->m_MenuItemToCmdProcPtr != nil);
					cmd = (*vtbl->m_MenuItemToCmdProcPtr)(pApp, HiWord(mResult), LoWord(mResult));
					if (cmd != 0)
						DoCmd(cmd);

					HiliteMenu(0);
				}
				else
				{
					long	choice = MenuChoice();

					// We have to do some special processing if the user chooses
					// the Hide Others item on the application menu.
					if (HiWord(choice) == -16489 && LoWord(choice) == 2)
					{
						ASSERTCOND(vtbl->m_HideOthersCmdProcPtr != nil);
						(*vtbl->m_HideOthersCmdProcPtr)(pApp);
					}
				}
			}
			break;
		case inGoAway:
			ASSERTCOND(vtbl->m_DoGoAwayProcPtr != nil);
			(*vtbl->m_DoGoAwayProcPtr)(pApp);
			break;
		case inDrag:
			ASSERTCOND(vtbl->m_DoDragProcPtr != nil);
			(*vtbl->m_DoDragProcPtr)(pApp);
			break;
		case inGrow:
			if (pApp->m_CurDoc != nil)
			{
				DocumentPtr		pDoc;

				pDoc = pApp->m_CurDoc;

				ASSERTCOND(pDoc->vtbl->m_DoGrowProcPtr != nil);
				(*pDoc->vtbl->m_DoGrowProcPtr)(pDoc, &theEvent);
			}
			break;
		case inZoomIn:
		case inZoomOut:
			if ((TrackBox(pApp->m_WhichWindow, theEvent.where, partCode)) &&
				(pApp->m_CurDoc != nil))
			{
				DocumentPtr		pDoc;

				pDoc = pApp->m_CurDoc;

				ASSERTCOND(pDoc->vtbl->m_DoZoomProcPtr != nil);
				(*pDoc->vtbl->m_DoZoomProcPtr)(pDoc, partCode);
			}
			break;
		case inContent:
			{
				DocumentPtr		pDoc;
	
				pDoc = pApp->m_CurDoc;

				ASSERTCOND(vtbl->m_IsFrontWindowProcPtr != nil);
				if ((*vtbl->m_IsFrontWindowProcPtr)(pApp, (WinPtr)pDoc))
				{	
					ASSERTCOND(pDoc->vtbl->m_DoContentProcPtr != nil);
					(*pDoc->vtbl->m_DoContentProcPtr)(pDoc, &theEvent);
				}
				else
				{
					if (!pDoc)
						SelectWindow(pApp->m_WhichWindow);
					else
					{
						ASSERTCOND(pDoc->vtbl->m_BringToFrontProcPtr != nil);
						(*pDoc->vtbl->m_BringToFrontProcPtr)(pDoc);
					}
				}
			}
			break;
	}
}

//#pragma segment ApplicationSeg
void AppDoMouseUp(ApplicationPtr pApp)
{
}

//#pragma segment ApplicationSeg
void AppDoKeyDown(ApplicationPtr pApp)
{
	if ((pApp->m_TheEvent.modifiers & cmdKey) && (pApp->m_TheEvent.what == keyDown))
	{
		long	mResult;

		UpdateMenus();

		mResult = MenuKey((char)(pApp->m_TheEvent.message & charCodeMask));
		if (mResult != 0)
		{
			long	cmd;

			ASSERTCOND(pApp->vtbl->m_MenuItemToCmdProcPtr != nil);
			cmd = (*pApp->vtbl->m_MenuItemToCmdProcPtr)(pApp, HiWord(mResult), LoWord(mResult));
			if (cmd != 0)
				DoCmd(cmd);

			HiliteMenu(0);

			return;
		}
		else
		{
			ASSERTCOND(pApp->vtbl->m_DoUnknownMenuKeyProcPtr != nil);
			(*pApp->vtbl->m_DoUnknownMenuKeyProcPtr)(pApp, &pApp->m_TheEvent);
		}
	}

	if (pApp->m_CurDoc != nil)
	{
		EventRecord		theEvent;
		DocumentPtr		pDoc;

		theEvent = pApp->m_TheEvent;
		pDoc = pApp->m_CurDoc;

		ASSERTCOND(pDoc->vtbl->m_DoKeyDownProcPtr != nil);
		(*pDoc->vtbl->m_DoKeyDownProcPtr)(pDoc, &theEvent);
	}
}

//#pragma segment ApplicationSeg
void AppDoUpdateEvent(ApplicationPtr pApp)
{
	GrafPtr			oldPort;
	DocumentPtr		pDoc;
	WindowPtr		whichWindow;
	
	whichWindow = (WindowPtr)pApp->m_TheEvent.message;

	// check if window belongs to a document
	ASSERTCOND(pApp->vtbl->m_FindDocProcPtr != nil);
	pDoc = (*pApp->vtbl->m_FindDocProcPtr)(pApp, whichWindow);

	GetPort(&oldPort);
	SetPort(whichWindow);
	
	BeginUpdate(whichWindow);

	if (pDoc != nil)
	{
		ASSERTCOND(pDoc->vtbl->m_DoUpdateProcPtr != nil);
		(*pDoc->vtbl->m_DoUpdateProcPtr)(pDoc);
	}

	EndUpdate(whichWindow);
	
	SetPort(oldPort);
}

//#pragma segment ApplicationSeg
void AppDoDiskEvent(ApplicationPtr pApp)
{
}

//#pragma segment ApplicationSeg
void AppDoActivateEvent(ApplicationPtr pApp)
{
	DocumentPtr		pDoc;
	WindowPtr		whichWindow;
	
	whichWindow = (WindowPtr)pApp->m_TheEvent.message;

	// check if window belongs to a document
	ASSERTCOND(pApp->vtbl->m_FindDocProcPtr != nil);
	pDoc = (*pApp->vtbl->m_FindDocProcPtr)(pApp, whichWindow);
	SetPort(whichWindow);

	if (pDoc != nil)
	{
		if (pDoc->m_Active != (pApp->m_TheEvent.modifiers & activeFlag))
		{
			ASSERTCOND(pDoc->vtbl->m_DoActivateProcPtr != nil);
			(*pDoc->vtbl->m_DoActivateProcPtr)(pDoc, (Boolean)(pApp->m_TheEvent.modifiers & activeFlag));
		}
	}
}

//#pragma segment ApplicationSeg
void AppDoSuspend(ApplicationPtr pApp)
{
#if qFrameTools
	ASSERTCOND(pApp->vtbl->m_HideFrameToolsProcPtr != nil);
	(*pApp->vtbl->m_HideFrameToolsProcPtr)(pApp);
#endif

	ASSERTCOND(pApp->vtbl->m_FlushClipboardProcPtr != nil);
	(*pApp->vtbl->m_FlushClipboardProcPtr)(pApp);

	if (pApp->m_CurDoc != nil)
	{
		DocumentPtr		pDoc;

		pDoc = pApp->m_CurDoc;

		if (pDoc->m_Active)
		{
			ASSERTCOND(pDoc->vtbl->m_DoActivateProcPtr != nil);
			(*pDoc->vtbl->m_DoActivateProcPtr)(pDoc, false);
		}
	}
}

//#pragma segment ApplicationSeg
void AppDoResume(ApplicationPtr pApp)
{
#if qFrameTools
	ASSERTCOND(pApp->vtbl->m_ShowFrameToolsProcPtr != nil);
	(*pApp->vtbl->m_ShowFrameToolsProcPtr)(pApp);
#endif

	if (pApp->m_CurDoc != nil)
	{
		DocumentPtr		pDoc;

		pDoc = pApp->m_CurDoc;

		if (!pDoc->m_Active)
		{
			ASSERTCOND(pDoc->vtbl->m_DoActivateProcPtr != nil);
			(*pDoc->vtbl->m_DoActivateProcPtr)(pDoc, true);
		}
	}
}

//#pragma segment ApplicationSeg
void AppDoOSEvent(ApplicationPtr pApp)
{
	switch ((unsigned char)(pApp->m_TheEvent.message >> 24) & 0x00ff)
	{
		case mouseMovedMessage:
			AdjustCursor();
			break;

		case suspendResumeMessage:
			pApp->m_InBackground = !(pApp->m_TheEvent.message & resumeFlag);
			if (pApp->m_InBackground)
			{
				ASSERTCOND(pApp->vtbl->m_DoSuspendProcPtr != nil);
				(*pApp->vtbl->m_DoSuspendProcPtr)(pApp);
			}
			else
			{
				ASSERTCOND(pApp->vtbl->m_DoResumeProcPtr != nil);
				(*pApp->vtbl->m_DoResumeProcPtr)(pApp);
			}
			break;
	}
}

//#pragma segment ApplicationSeg
void AppHighLevelEvent(ApplicationPtr pApp)
{
	OSErr	err;

	err = AEProcessAppleEvent(&pApp->m_TheEvent);
	
// the following line takes care of a stray AppleEvent that we are
// seeing when insert from Excel or Word. This code should be removed when
// the bug is fixed
#if qAssert
//if (err != errAEEventNotHandled)
#endif
	ASSERTCOND(err == noErr);
}

//#pragma segment ApplicationSeg
void AppAdjustCursor(ApplicationPtr pApp)
{
	ASSERTCOND(pApp->m_pIdleCursor);
	SetCursor(pApp->m_pIdleCursor);
}

//#pragma segment ApplicationSeg
void AppUpdateCurrentDoc(ApplicationPtr pApp)
{
	WinPtr theWind = NULL;

	pApp->m_WhichWindow = FrontWindow();
	if (pApp->m_WhichWindow != nil)
	{
		// find the document that goes with the window

		theWind = (WinPtr) GetWRefCon(pApp->m_WhichWindow);
		if (NULL != theWind)
		{
			if (!theWind->m_fFloating)
			{
				ASSERTCOND(pApp->vtbl->m_FindDocProcPtr != nil);
				pApp->m_CurDoc = (*pApp->vtbl->m_FindDocProcPtr)(pApp, pApp->m_WhichWindow);
				SetPort(pApp->m_WhichWindow);
			}
			else
			{
				pApp->m_CurDoc = nil;
			}
		}
		else
		{
			SetPort(pApp->m_WhichWindow);
		}
	}
	else
		pApp->m_CurDoc = nil;
}

//#pragma segment ApplicationSeg
void AppSetIdleCursor(ApplicationPtr pApp, CursPtr pCursor)
{
	ASSERTCOND(pCursor);
	pApp->m_pIdleCursor = pCursor;
}

//#pragma segment ApplicationSeg
void AppDoIdle(ApplicationPtr pApp)
{
	WindowPeek		theWindow;
	DocumentPtr		pDoc;
	
	theWindow = (WindowPeek)FrontWindow();
	
	while(theWindow != nil)
	{
		ASSERTCOND(pApp->vtbl->m_FindDocProcPtr != nil);
		pDoc = (pApp->vtbl->m_FindDocProcPtr)(pApp, (WindowPtr)theWindow);
		
		if (pDoc)
		{
			ASSERTCOND(pDoc->vtbl->m_DoIdleProcPtr != nil);
			(*pDoc->vtbl->m_DoIdleProcPtr)(pDoc);
		}
	
		theWindow = theWindow->nextWindow;
	}
}

//#pragma segment ApplicationSeg
DocumentPtr AppCreateDoc(ApplicationPtr pApp)
{
	return nil;
}

//#pragma segment ApplicationSeg
void AppDoNew(ApplicationPtr pApp)
{
}

//#pragma segment ApplicationSeg
Boolean AppDoOpen(ApplicationPtr pApp)
{
	StandardFileReply	theReply;

	StandardGetFile(pApp->m_pFileFilter, pApp->m_NumTypes, pApp->m_TypeList, &theReply);

	if (theReply.sfGood)
	{
		ASSERTCOND(pApp->vtbl->m_OpenADocProcPtr != nil);
		return (*pApp->vtbl->m_OpenADocProcPtr)(pApp, &theReply.sfFile, theReply.sfType);
	}

	return false;
}

//#pragma segment ApplicationSeg
Boolean AppOpenADoc(ApplicationPtr pApp, FSSpecPtr spec, OSType type)
{
	return false;
}

//#pragma segment ApplicationSeg
void AppPositionNewDoc(ApplicationPtr pApp, DocumentPtr pDoc)
{
	Rect	rBorder;

	ASSERTCOND(pApp->vtbl->m_GetBorderSpaceProcPtr != nil);
	(*pApp->vtbl->m_GetBorderSpaceProcPtr)(pApp, &rBorder);

	if (!rBorder.left && !rBorder.top)
		return;
	
	ASSERTCOND(pDoc->vtbl->m_ShiftWindowProcPtr != nil);
	(*pDoc->vtbl->m_ShiftWindowProcPtr)(pDoc, rBorder.left, rBorder.top);
}

//#pragma segment ApplicationSeg
long AppMenuItemToCmd(ApplicationPtr pApp, short menuID, short menuItem)
{
	MenuCommandPtr		p;
	short				numCmds;

	p = (MenuCommandPtr)*pApp->m_MenuCommands;

	numCmds = *(*(short **)&p)++;

	for(; numCmds>0; numCmds--, p++)
	{
		if (p->menuID == menuID && p->menuItem == menuItem)
			return p->menuCmd;
	}
	
	return -(((long)menuID) << 16) - menuItem;
}

//#pragma segment ApplicationSeg
Boolean AppCmdToMenuItem(ApplicationPtr pApp, long cmd, short* menuID, short* menuItem)
{
	MenuCommandPtr		p;
	short				numCmds;
	
	p = (MenuCommandPtr)*pApp->m_MenuCommands;

	numCmds = *(*(short **)&p)++;

	for(; numCmds>0; numCmds--, p++)
	{
		if (p->menuCmd == cmd)
		{
			*menuID = p->menuID;
			*menuItem = p->menuItem;
			
			return true;
		}
	}
	
	return false;
}

//#pragma segment ApplicationSeg
void EnableCmd(long cmd)
{
	ApplicationPtr		pApp;
	
	pApp = gApplication;
	
	ASSERTCOND(pApp->vtbl->m_EnableCmdProcPtr != nil);
	(*pApp->vtbl->m_EnableCmdProcPtr)(pApp, cmd);
}

//#pragma segment ApplicationSeg
void AppEnableCmd(ApplicationPtr pApp, long cmd)
{
	short		menuID;
	short		menuItem;
	MenuHandle	theMenu;
	
	ASSERTCOND(pApp->vtbl->m_CmdToMenuItemProcPtr != nil);
	if ((*pApp->vtbl->m_CmdToMenuItemProcPtr)(pApp, cmd, &menuID, &menuItem))
	{
		theMenu = GetMHandle(menuID);
		if (theMenu)
			EnableItem(theMenu, menuItem);
	}
}

//#pragma segment ApplicationSeg
void CheckCmd(long cmd, Boolean fCheck)
{
	ApplicationPtr		pApp;
	
	pApp = gApplication;
	
	ASSERTCOND(pApp->vtbl->m_CheckCmdProcPtr != nil);
	(*pApp->vtbl->m_CheckCmdProcPtr)(pApp, cmd, fCheck);
}

//#pragma segment ApplicationSeg
void AppCheckCmd(ApplicationPtr pApp, long cmd, Boolean fCheck)
{
	short		menuID;
	short		menuItem;
	MenuHandle	theMenu;
	
	ASSERTCOND(pApp->vtbl->m_CmdToMenuItemProcPtr != nil);
	if ((*pApp->vtbl->m_CmdToMenuItemProcPtr)(pApp, cmd, &menuID, &menuItem))
	{
		theMenu = GetMHandle(menuID);
		if (theMenu)
			CheckItem(theMenu, menuItem, fCheck);
	}
}

//#pragma segment ApplicationSeg
void AppGetCmdItem(ApplicationPtr pApp, long cmd, StringPtr pCmdString)
{
	short		menuID;
	short		menuItem;
	MenuHandle	theMenu;
	
	pCmdString[0] = 0;

	ASSERTCOND(pApp->vtbl->m_CmdToMenuItemProcPtr != nil);
	if ((*pApp->vtbl->m_CmdToMenuItemProcPtr)(pApp, cmd, &menuID, &menuItem))
	{
		theMenu = GetMHandle(menuID);
		if (theMenu)
			GetItem(theMenu, menuItem, pCmdString);
	}
}

//#pragma segment ApplicationSeg
void AppSetCmdItem(ApplicationPtr pApp, long cmd, StringPtr pCmdString)
{
	short		menuID;
	short		menuItem;
	MenuHandle	theMenu;
	
	ASSERTCOND(pApp->vtbl->m_CmdToMenuItemProcPtr != nil);
	if ((*pApp->vtbl->m_CmdToMenuItemProcPtr)(pApp, cmd, &menuID, &menuItem))
	{
		theMenu = GetMHandle(menuID);
		if (theMenu)
			SetItem(theMenu, menuItem, pCmdString);
	}
}

//#pragma segment ApplicationSeg
void AppUpdateMenus(ApplicationPtr pApp)
{
#if qFrameTools
	ToolbarPtr		pToolbar;
	TOOLBAR_STATE	state;
#endif

	EnableCmd(cmdNew);
	EnableCmd(cmdOpen);
	EnableCmd(cmdQuit);
	EnableCmd(cmdAbout);

#if qFrameTools
	EnableCmd(cmdToolbar);
	EnableCmd(cmdFormulaBar);
	EnableCmd(cmdHeadings);

#ifdef __LATER__
	EnableCmd(cmdToolbarAtTop);
	EnableCmd(cmdToolbarFloating);
	EnableCmd(cmdToolbarHidden);
#endif

	pToolbar = pApp->m_pToolbar;

	ASSERTCOND(pToolbar->vtbl->m_GetStateProcPtr != nil);
	state = (*pToolbar->vtbl->m_GetStateProcPtr)(pToolbar);

	CheckCmd(cmdToolbarAtTop,    (Boolean)(state == TOOLBAR_TOP));
	CheckCmd(cmdToolbarFloating, (Boolean)(state == TOOLBAR_FLOATING));
	CheckCmd(cmdToolbarHidden,   (Boolean)(state == TOOLBAR_DISABLED));
#endif

#if qDebug
	EnableCmd(cmdDebug);
#if qAssert
	EnableCmd(cmdAsserts);
	CheckCmd(cmdAsserts, gEnableAsserts);
#endif
#endif

	// DA is front window
	if (pApp->m_WhichWindow && ((WindowPeek)pApp->m_WhichWindow)->windowKind < 0)
	{
		EnableCmd(cmdUndo);
		EnableCmd(cmdCut);
		EnableCmd(cmdCopy);
		EnableCmd(cmdPaste);
	}
}

//#pragma segment ApplicationSeg
void AppPreUpdateMenus(ApplicationPtr pApp)
{
	MenuHandle	hMenu;
	
	ASSERTCOND(pApp->vtbl->m_GetWindowsMenuHandleProcPtr != nil);
	hMenu = (*pApp->vtbl->m_GetWindowsMenuHandleProcPtr)(pApp);
	if (hMenu != nil)
	{
	}
}

//#pragma segment ApplicationSeg
MenuHandle AppGetWindowsMenuHandle(ApplicationPtr pApp)
{
	return nil;
}

//#pragma segment ApplicationSeg
void AppHideOthersCmd(ApplicationPtr pApp)
{
}

//#pragma segment ApplicationSeg
void AppDoCmd(ApplicationPtr pApp, long cmd)
{
	if (cmd < 0)
	{		
		if (HiWord(-cmd) == kApple_MENU)
		{
			Str255		theDeskAccessory;
			
			GetItem(GetMHandle(kApple_MENU), LoWord(-cmd), theDeskAccessory);
			OpenDeskAcc(theDeskAccessory);
		}
		else
		{
			EventRecord		theEvent;
			
			theEvent = pApp->m_TheEvent;
			
			ASSERTCOND(pApp->vtbl->m_DoUnknownMenuItemProcPtr != nil);
			(*pApp->vtbl->m_DoUnknownMenuItemProcPtr)(pApp, &theEvent, HiWord(-cmd), LoWord(-cmd));
		}
	}
	else
	{
		ApplicationVtblPtr	vtbl;
#if qFrameTools
		WindowPtr	pSavedLayer;

		pSavedLayer = SwapLayer(pApp->m_pDocLayer);
#endif

		vtbl = pApp->vtbl;

		TRY
		{
			switch(cmd)
			{
				case cmdQuit:
					ASSERTCOND(vtbl->m_DoQuitProcPtr != nil);
					(*vtbl->m_DoQuitProcPtr)(pApp, true, yesResult);
					break;
		
				case cmdNew:
					ASSERTCOND(vtbl->m_DoNewProcPtr != nil);
					(*vtbl->m_DoNewProcPtr)(pApp);
					break;
		
				case cmdOpen:
					ASSERTCOND(vtbl->m_DoOpenProcPtr != nil);
					(*vtbl->m_DoOpenProcPtr)(pApp);
					break;

#if qDebug
				case cmdAsserts:
					gEnableAsserts = !gEnableAsserts;
					CheckCmd(cmdAsserts, gEnableAsserts);
					break;
#endif
			}
		}
		CATCH
		{
#if qFrameTools
			SetLayer(pSavedLayer);
#endif
		}
		ENDTRY

#if qFrameTools
			SetLayer(pSavedLayer);
#endif
	}
}

//#pragma segment ApplicationSeg
void AppDoUnknownMenuKey(ApplicationPtr pApp, EventRecord* pEvent)
{
}

//#pragma segment ApplicationSeg
void AppDoUnknownMenuItem(ApplicationPtr pApp, EventRecord* pEvent, short menuID, short menuItem)
{
}

//#pragma segment ApplicationSeg
void AppDoMouseInSysWindow(ApplicationPtr pApp)
{
	SystemClick(&pApp->m_TheEvent, pApp->m_WhichWindow);
}

//#pragma segment ApplicationSeg
void AppDoGoAway(ApplicationPtr pApp)
{
	if (TrackGoAway(pApp->m_WhichWindow, pApp->m_TheEvent.where))
	{
		if (pApp->m_CurDoc != nil)
		{
			DocumentPtr		pDoc;

			pDoc = pApp->m_CurDoc;

			ASSERTCOND(pDoc->vtbl->m_DoCloseProcPtr != nil);
			if ((*pDoc->vtbl->m_DoCloseProcPtr)(pDoc, true, yesResult, false) != cancelResult)
			{
            // pDoc is only valid at this point in the non-OLE version of
            // Outline.  For the OLE versions, the pointer is destroyed
            // when the reference count goes to zero
#if !qOle
				ASSERTCOND(pDoc->vtbl->m_FreeProcPtr != nil);
				(*pDoc->vtbl->m_FreeProcPtr)(pDoc);
#endif
			}
		}
		else
			CloseDeskAcc(((WindowPeek)pApp->m_WhichWindow)->windowKind);
	}
}

//#pragma segment ApplicationSeg
void AppDoDrag(ApplicationPtr pApp)
{
	DocumentPtr		pDoc;
	
	pDoc = pApp->m_CurDoc;
	
	if (pDoc == nil)
		DragWindow(pApp->m_WhichWindow, pApp->m_TheEvent.where, &qd.screenBits.bounds);
	else
	{
		ASSERTCOND(pDoc->vtbl->m_DoDragProcPtr != nil);
		(*pDoc->vtbl->m_DoDragProcPtr)(pDoc, &pApp->m_TheEvent, true);
	}
}

//#pragma segment ApplicationSeg
void AppDoQuit(ApplicationPtr pApp, Boolean askUser, YNCResult defaultResult)
{
	Boolean		fClosingVisible;
#if qFrameTools
	WindowPtr	pSavedLayer;

	pSavedLayer = SwapLayer(pApp->m_pDocLayer);
#endif

	// We will close the visible documents, flush the clipboard,
	//  then close any invisible documents (which are inplace active docs).
	fClosingVisible = true;
		
	while(true)
	{
		WindowPtr	theWindow;
		
		pApp->m_WhichWindow = fClosingVisible ? FrontWindow() : (WindowPtr)LMGetWindowList();

		if (pApp->m_WhichWindow == nil)
		{
			if (fClosingVisible)
			{
				// we are done with all visible windows,
				// now flush clipboard and go for invisible windows
				if (!gApplication->m_InBackground)
				{
					ASSERTCOND(pApp->vtbl->m_FlushClipboardProcPtr != nil);
					(*pApp->vtbl->m_FlushClipboardProcPtr)(pApp);
				}
				fClosingVisible = false;					
				continue;
			}
			else
			{
				// we are done closing all invisible windows, quit
				break;
			}
		}

		theWindow = pApp->m_WhichWindow;

		ASSERTCOND(pApp->vtbl->m_FindDocProcPtr != nil);
		pApp->m_CurDoc = (*pApp->vtbl->m_FindDocProcPtr)(pApp, theWindow);

		// got the toolbar
		if (((WinPtr)pApp->m_CurDoc)->m_fFloating)
		{
			ASSERTCONDSZ(0, "\pAppDoQuit: nabbed the toolbar at shutdown");
		    pApp->m_CurDoc = nil;
		}


		
		if (pApp->m_CurDoc != nil)
		{
			DocumentPtr		pDoc;

			pDoc = pApp->m_CurDoc;

			// user may cancel quit
			ASSERTCOND(pDoc->vtbl->m_DoCloseProcPtr != nil);
			if ((*pDoc->vtbl->m_DoCloseProcPtr)(pDoc, askUser, defaultResult, true) == cancelResult)
			{
#if qFrameTools
				SetLayer(pSavedLayer);
#endif
				return;
			}
			else
			{
            // pDoc is only valid at this point in the non-OLE version of
            // Outline.  For the OLE versions, the pointer is destroyed
            // when the reference count goes to zero
#if !qOle
				ASSERTCOND(pDoc->vtbl->m_FreeProcPtr != nil);
				(*pDoc->vtbl->m_FreeProcPtr)(pDoc);
#endif
			}
		}
		else
		{
			ASSERTCONDSZ(0, "AppDoQuit: FindDoc returned nil");
		}

		// make sure we are not in an infinite loop. Can occur if CloseDeskAcc
		// fails for some reason. Gives us a chance to close our other
		// windows.
		if (theWindow == (fClosingVisible ? FrontWindow() : (WindowPtr)LMGetWindowList()))
		{
			// send problem window to the back.
			SendBehind(theWindow, nil);

			// check front window now to see if still the same
			if (theWindow == (fClosingVisible ? FrontWindow() : (WindowPtr)LMGetWindowList()))
			{
				if (fClosingVisible)
				{
					// we are done with all visible window,
					// now flush clipboard and go for invisible windows
					if (!gApplication->m_InBackground)
					{
						ASSERTCOND(pApp->vtbl->m_FlushClipboardProcPtr != nil);
						(*pApp->vtbl->m_FlushClipboardProcPtr)(pApp);
					}
					fClosingVisible = false;					
					continue;
				}
				else
				{
					// we are done closing all invisible windows, quit
					break;
				}
			}
		}
	}

	pApp->m_fIsReadyToQuit = true;
	pApp->m_WhichWindow = nil;
	pApp->m_CurDoc = nil;

#if qFrameTools
	SetLayer(pSavedLayer);
#endif
}

//#pragma segment ApplicationSeg
void AppFlushClipboard(ApplicationPtr pApp)
{
}

//#pragma segment ApplicationSeg
void AppGetFrameRect(ApplicationPtr pApp, Rect* prectFrame)
{
	// calculate the rectangle that we offer to an inplace active
	// object from which to get frame tool space.
	*prectFrame = qd.screenBits.bounds;
	prectFrame->top += GetMBarHeight();
}

//#pragma segment ApplicationSeg
void AppGetBorderSpace(ApplicationPtr pApp, Rect* prectBorder)
{
	*prectBorder = pApp->m_BorderWidths;
}

//#pragma segment ApplicationSeg
void AppSetBorderSpace(ApplicationPtr pApp, Rect* prectBorder, Boolean fCallDocSetBorder)
{
	Rect			rBorder = { 0, 0, 0, 0 };
	DocumentPtr		pDocument;
	short			hOffset,
					vOffset;

	if (prectBorder != nil)
		rBorder = *prectBorder;

	if (fCallDocSetBorder)
	{
		ASSERTCOND(pApp->vtbl->m_GetCurrentDocProcPtr != nil);
		pDocument = (*pApp->vtbl->m_GetCurrentDocProcPtr)(pApp);

		if (pDocument != nil)
		{
			ASSERTCOND(pDocument->vtbl->m_SetBorderSpaceProcPtr != nil);
			(*pDocument->vtbl->m_SetBorderSpaceProcPtr)(pDocument, prectBorder);

			rBorder = pDocument->m_BorderWidths;
		}
	}

	hOffset = rBorder.left - pApp->m_BorderWidths.left;
	vOffset = rBorder.top  - pApp->m_BorderWidths.top;

	if (hOffset || vOffset)
	{
		ASSERTCOND(pApp->vtbl->m_ShiftAllWindowsProcPtr != nil);
		(*pApp->vtbl->m_ShiftAllWindowsProcPtr)(pApp, hOffset, vOffset);
	}

	pApp->m_BorderWidths = rBorder;
}

//#pragma segment ApplicationSeg
void AppShiftAllWindows(ApplicationPtr pApp, short hShift, short vShift)
{
#define kDocLayer		0
#define kFloatLayer		1

	GrafPtr			oldPort;
	WindowPeek		pWindow;
	WinPtr			pWin;
#if qFrameTools
	WindowPtr		pSavedLayer;
	short			layer;
#endif

	GetPort(&oldPort);

#if qFrameTools
	for (layer = kDocLayer; layer <= kFloatLayer; layer++)
	{
		pSavedLayer = SwapLayer(layer == kDocLayer ? pApp->m_pDocLayer : pApp->m_pFloatLayer);
#endif

		for (pWindow = LMGetWindowList(); pWindow; pWindow = pWindow->nextWindow)
		{
			ASSERTCOND(pApp->vtbl->m_FindDocProcPtr != nil);
			pWin = (*pApp->vtbl->m_FindWinProcPtr)(pApp, (WindowPtr)pWindow);

			if (pWin)
			{
				ASSERTCOND(pWin->vtbl->m_ShiftWindowProcPtr != nil);
				(*pWin->vtbl->m_ShiftWindowProcPtr)(pWin, hShift, vShift);
			}
		}

#if qFrameTools
		SetLayer(pSavedLayer);
	}
#endif

	SetPort(oldPort);
}

#if qFrameTools
//#pragma segment ApplicationSeg
void AppCreateFrameTools(ApplicationPtr pApp)
{
	ToolbarPtr		pToolbar;

	pToolbar = (ToolbarPtr)NewPtrClear(sizeof(ToolbarRec));
	
	ASSERTCOND(pToolbar != nil);
	FailNIL(pToolbar);
	
	ToolbarInit(pToolbar, kToolbar_WIND);

	pApp->m_pToolbar = pToolbar;
}

//#pragma segment ApplicationSeg
void AppDisposeFrameTools(ApplicationPtr pApp)
{
	ToolbarPtr		pToolbar;

	pToolbar = pApp->m_pToolbar;

	if (pToolbar)
	{
		ASSERTCOND(pToolbar->vtbl->m_FreeProcPtr != nil);
		(*pToolbar->vtbl->m_FreeProcPtr)(pToolbar);
	}
}

//#pragma segment ApplicationSeg
void AppFrameSpaceNeeded(ApplicationPtr pApp, Rect* prectBounds)
{
	ToolbarPtr		pToolbar;

	pToolbar = gApplication->m_pToolbar;

	ASSERTCOND(pToolbar->vtbl->m_SpaceNeededProcPtr != nil);
	(*pToolbar->vtbl->m_SpaceNeededProcPtr)(pToolbar, prectBounds);
}

//#pragma segment ApplicationSeg
Boolean AppIsClickInFrameTools(ApplicationPtr pApp, EventRecord* pEvent)
{
	WindowPtr		pSavedLayer;
	WindowPtr		pWindow;
	
	pSavedLayer = SwapLayer(pApp->m_pFloatLayer);
	FindWindow(pEvent->where, &pWindow);
	SetLayer(pSavedLayer);

	return (pWindow != nil);
}

//#pragma segment ApplicationSeg
void AppFloatWindowsStrucRgns(ApplicationPtr pApp, RgnHandle rgnFloats)
{
	WindowPtr		pSavedLayer;
	
	pSavedLayer = SwapLayer(pApp->m_pFloatLayer);
	GetUnionWindowStrucRgns(rgnFloats);
	SetLayer(pSavedLayer);
}

//#pragma segment ApplicationSeg
void AppShowFrameTools(ApplicationPtr pApp)
{
	DocumentPtr		pDocument;
	ToolbarPtr		pToolbar;

	ASSERTCOND(pApp->vtbl->m_GetCurrentDocProcPtr != nil);
	pDocument = (*pApp->vtbl->m_GetCurrentDocProcPtr)(pApp);

	if (pDocument != nil && !pDocument->m_fShowFrameTools)
		return;

	if (!pApp->m_InBackground)
	{
		pToolbar = pApp->m_pToolbar;

		ASSERTCOND(pToolbar->vtbl->m_ShowProcPtr != nil);
		(*pToolbar->vtbl->m_ShowProcPtr)(pToolbar);
	}
}

//#pragma segment ApplicationSeg
void AppHideFrameTools(ApplicationPtr pApp)
{
	ToolbarPtr		pToolbar;

	pToolbar = pApp->m_pToolbar;

	ASSERTCOND(pToolbar->vtbl->m_HideProcPtr != nil);
	(*pToolbar->vtbl->m_HideProcPtr)(pToolbar);
}

//#pragma segment ApplicationSeg
Boolean AppToolsCoverWindow(ApplicationPtr pApp, WindowPtr pWindow, short hPos, short vPos)
{
	Rect		rBorder;
	Rect		rTools,
				rTitle,
				rSect;

	ASSERTCOND(pApp->vtbl->m_GetBorderSpaceProcPtr != nil);
	(*pApp->vtbl->m_GetBorderSpaceProcPtr)(pApp, &rBorder);

	rTitle = (**((WindowPeek)pWindow)->strucRgn).rgnBBox;
	rTitle.bottom = (**((WindowPeek)pWindow)->contRgn).rgnBBox.top;
	OffsetRect(&rTitle, (short)(hPos - rTitle.left),(short)(vPos - rTitle.bottom));

	if (rBorder.top > 0)
	{
		rTools = qd.screenBits.bounds;
		rTools.bottom = rTools.top + GetMBarHeight() + rBorder.top;
		SectRect(&rTitle, &rTools, &rSect);

		if ((rTitle.bottom - rTitle.top) - (rSect.bottom - rSect.top) < 7 &&
			(rTitle.right - rTitle.left) - (rSect.right - rSect.left) < 7)
			return true;
	}

	if (rBorder.bottom > 0)
	{
		rTools = qd.screenBits.bounds;
		rTools.top = rTools.bottom - rBorder.bottom;
		SectRect(&rTitle, &rTools, &rSect);

		if ((rTitle.bottom - rTitle.top) - (rSect.bottom - rSect.top) < 7 &&
			(rTitle.right - rTitle.left) - (rSect.right - rSect.left) < 7)
			return true;
	}

	return false;
}
#endif

//#pragma segment ApplicationSeg
void AppHide(ApplicationPtr pApp)
{
	// REVIEW: how are we going to hide?
}

//#pragma segment ApplicationSeg
void AppShow(ApplicationPtr pApp)
{
	ProcessSerialNumber		currentProcess;
	ProcessSerialNumber		frontProcess;
	Boolean					fSame;
	EventRecord				theEvent;
	unsigned long			ulTimeout = TickCount() + 60 * 5;

	GetCurrentProcess(&currentProcess);
	SetFrontProcess(&currentProcess);

	// spin until the this process is in front
	while(true)
	{
		GetFrontProcess(&frontProcess);
		SameProcess(&currentProcess, &frontProcess, &fSame);

		if (fSame)
			break;

		if (TickCount() > ulTimeout)
		{
			ASSERTCONDSZ(false, "Timed out trying to switch to the foreground");
			return;
		}

		WaitNextEvent(0, &theEvent, 3, 0);
	}

	pApp->m_InBackground = !AmIFrontProcess();
}

//#pragma segment ApplicationSeg
void AppWaitContextSwitch(ApplicationPtr pApp, Boolean comingForward, Boolean eatEvent)
{
	Boolean			haveEvent,
					inBackground;
	EventRecord		theEvent;
	unsigned long	ulTimeout = TickCount() + 60 * 5;

	if (comingForward == AmIFrontProcess())
		return;

	while (true)
	{
		if (eatEvent)
			haveEvent = WaitNextEvent(osMask, &theEvent, 3, 0);
		else
			haveEvent = EventAvail(osMask, &theEvent);

		if (haveEvent)
		{
			if (((unsigned char)(theEvent.message >> 24) & 0x00ff) == suspendResumeMessage)
			{
				inBackground = !(theEvent.message & resumeFlag);
				if (eatEvent)
					pApp->m_InBackground = inBackground;
				break;
			}
		}

		if (TickCount() > ulTimeout)
		{
			ASSERTCONDSZ(false, "Timed out waiting to switch processes");
			return;
		}
	}

	ASSERTCONDSZ(inBackground != comingForward, "Switch the wrong way!");
}

//#pragma segment ApplicationSeg
WindowPtr AppGetFrontDocWindow(ApplicationPtr pApp)
{
	WindowPtr	pWindow;
#if qFrameTools
	WindowPtr	pSavedLayer;

	pSavedLayer = SwapLayer(pApp->m_pDocLayer);
#endif

	pWindow = (WindowPtr)LMGetWindowList();

#if qFrameTools
	SetLayer(pSavedLayer);
#endif

	return pWindow;
}

//#pragma segment ApplicationSeg
DocumentPtr AppGetCurrentDoc(ApplicationPtr pApp)
{
	return pApp->m_CurDoc;
}

//#pragma segment ApplicationSeg
void AppGetNextNewFileName(ApplicationPtr pApp, FSSpecPtr pFile)
{
	StringHandle	hStrBase;
	Str255			fileName;
	Str32			s;

	hStrBase = GetString(kNewDocumentName_STR);
	ASSERTCOND(hStrBase != nil);
	pStrCopy(fileName, *hStrBase);

	if (gDocumentSequence > 1)
	{
		pStrCat(fileName, "\p ");
		NumToString(gDocumentSequence, s);
		pStrCat(fileName, s);
	}

	FSMakeFSSpec(0, 0, fileName, pFile);

	gDocumentSequence++;
}

//#pragma segment ApplicationSeg
void AppDoAbout(ApplicationPtr pApp)
{
}

//#pragma segment ApplicationSeg
WinPtr AppFindWin(ApplicationPtr pApp, WindowPtr whichWindow)
{
	ASSERTCOND(whichWindow != nil);

	// if desk accessory
	if (((WindowPeek)whichWindow)->windowKind < 0)
		return nil;
	
	// if a dialog
	if (((WindowPeek)whichWindow)->windowKind == dialogKind)
		return nil;

	return (WinPtr)GetWRefCon(whichWindow);
}

//#pragma segment ApplicationSeg
DocumentPtr AppFindDoc(ApplicationPtr pApp, WindowPtr whichWindow)
{
	ASSERTCOND(pApp->vtbl->m_FindWinProcPtr != nil);
	return (DocumentPtr)(*pApp->vtbl->m_FindWinProcPtr)(pApp, whichWindow);
}

//#pragma segment ApplicationSeg
DocumentPtr AppDocFromFSSpec(ApplicationPtr pApp, FSSpec* pFile)
{
	WindowPtr		pWindow;
	DocumentPtr		pDocument;
#if qFrameTools
	WindowPtr		pSavedLayer;

	pSavedLayer = SwapLayer(pApp->m_pDocLayer);
#endif

	ASSERTCOND(pFile != nil);

	pWindow = (WindowPtr)LMGetWindowList();
	while (pWindow != nil)
	{
		if (((WindowPeek)pWindow)->windowKind != dialogKind)
		{
			pDocument = (DocumentPtr)GetWRefCon(pWindow);
			if (pDocument != nil)
			{
				FSSpec	pTempFile;

				DocGetFSSpec(pDocument, &pTempFile);

				if (pTempFile.vRefNum == pFile->vRefNum &&
					pTempFile.parID   == pFile->parID &&
					EqualString(pTempFile.name, pFile->name, true, true))
				{
#if qFrameTools
					SetLayer(pSavedLayer);
#endif
					return pDocument;
				}
			}
		}

		pWindow = (WindowPtr)((WindowPeek)pWindow)->nextWindow;
	}

#if qFrameTools
	SetLayer(pSavedLayer);
#endif

	return nil;
}

//#pragma segment ApplicationSeg
void AppGetCurrentEventRecord(ApplicationPtr pApp, EventRecord* pEvent)
{
	*pEvent = pApp->m_TheEvent;
}

//#pragma segment ApplicationSeg
#ifndef _MSC_VER
static pascal OSErr
#else
static OSErr __pascal
#endif
AppAEOpenDocuments(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp)
{
	OSErr		err;
	AEDescList	docList;
	long		index;
	long		itemsInList;
	FSSpec		fs;
	
	// get the direct parameter--a descriptor list--and put
	// it into docList
	err = AEGetParamDesc(theAppleEvent, keyDirectObject, typeAEList, &docList);
	ASSERTCOND(err == noErr);
	if (err != noErr)
		return err;
		
	// check for missing required parameters
	err = MyGotRequiredParams(theAppleEvent);
	ASSERTCOND(err == noErr);
	if (err != noErr)
	{
		// an error occurred:  do the necessary error handling
		AEDisposeDesc(&docList);
		return err;
	}
	
	// count the number of descriptor records in the list
	err = AECountItems(&docList,&itemsInList);
	ASSERTCOND(err == noErr);

	// now get each descriptor record from the list, coerce
	// the returned data to an FSSpec record, and open the
	// associated file
	for (index=1; index<=itemsInList; index++)
	{
		Size		actualSize;
		DescType	returnedType;
		AEKeyword	keywd;

		err = AEGetNthPtr(&docList, index, typeFSS, &keywd,
							&returnedType, (Ptr)&fs,
							sizeof(fs), &actualSize);
		ASSERTCOND(err == noErr);

		if (err == noErr)
		{
			FInfo		fndrInfo;
			Boolean		fOpened;
			
			err = FSpGetFInfo(&fs, &fndrInfo);
			ASSERTCOND(err == noErr);
			
			ASSERTCOND(pApp->vtbl->m_OpenADocProcPtr != nil);
			fOpened = (*pApp->vtbl->m_OpenADocProcPtr)(pApp, &fs, fndrInfo.fdType);

			// if this is the last document and it was already in the foreground
			// it never received an activate event so we need to simulate one.
			if (!fOpened && index == itemsInList)
			{
				WindowPtr		pWindow;
				DocumentPtr		pDocument;

				pWindow = FrontWindow();
				if (pWindow != nil)
				{
					// find the document that goes with this window
					ASSERTCOND(pApp->vtbl->m_FindDocProcPtr != nil);
					pDocument = (*pApp->vtbl->m_FindDocProcPtr)(pApp, pWindow);

					// if the document isn't active, simulate an activate event
					if (!pDocument->m_Active)
					{
						ASSERTCOND(pDocument->vtbl->m_DoActivateProcPtr != nil);
						(*pDocument->vtbl->m_DoActivateProcPtr)(pDocument, true);
					}
				}
			}
		}
	}

	err = AEDisposeDesc(&docList);
	ASSERTCOND(err == noErr);
	
	return noErr;
}

//#pragma segment ApplicationSeg
#ifndef _MSC_VER
static pascal OSErr
#else
static OSErr __pascal
#endif
AppAEOpenApplication(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp)
{
	OSErr	err;

	err = MyGotRequiredParams(theAppleEvent);
	if (err != noErr)
		return err;

	ASSERTCOND(pApp->vtbl->m_DoDefaultStartupActionProcPtr != nil);
	(*pApp->vtbl->m_DoDefaultStartupActionProcPtr)(pApp);

	return noErr;
}

//#pragma segment ApplicationSeg
#ifndef _MSC_VER
static pascal OSErr
#else
static OSErr __pascal
#endif
AppAEPrintDocuments(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp)
{
	return noErr;
}

//#pragma segment ApplicationSeg
#ifndef _MSC_VER
static pascal OSErr
#else
static OSErr __pascal
#endif
AppAEQuitApplication(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp)
{
	OSErr		err;

	err = MyGotRequiredParams(theAppleEvent);
	if (err != noErr)
		return err;

	ASSERTCOND(pApp->vtbl->m_DoQuitProcPtr != nil);
	(*pApp->vtbl->m_DoQuitProcPtr)(pApp, true, yesResult);

	ASSERTCOND(pApp->vtbl->m_IsReadyToQuitProcPtr != nil);
	return (*pApp->vtbl->m_IsReadyToQuitProcPtr)(pApp) ? noErr : userCanceledErr;

	return noErr;
}

//#pragma segment ApplicationSeg
#ifndef _MSC_VER
static pascal OSErr
#else
static OSErr __pascal
#endif
AppAEDoCmd(AppleEvent* theAppleEvent, AppleEvent* reply, ApplicationPtr pApp)
{
	OSErr		err;
	long		cmd;
	DescType	typeCode;
	Size		actualSize;

	err = AEGetParamPtr(theAppleEvent, keyDirectObject, typeLongInteger, &typeCode, (Ptr)&cmd, sizeof(cmd), &actualSize);
	if (err != noErr)
		return err;

	err = MyGotRequiredParams(theAppleEvent);
	if (err != noErr)
		return err;

	DoCmd(cmd);

	return noErr;
}

// OLDFILE: Outline.c

static OutlineVtblPtr		gOutlineVtbl = nil;

//#pragma segment VtblInitSeg
OutlineVtblPtr OutlineGetVtbl()
{
	ASSERTCOND(gOutlineVtbl != nil);
	return gOutlineVtbl;
}

//#pragma segment VtblInitSeg
void OutlineInitVtbl()
{
	OutlineVtblPtr	vtbl;

	vtbl = gOutlineVtbl = (OutlineVtblPtr)NewPtrClear(sizeof(*vtbl));
	ASSERTCOND(vtbl != nil);
	FailNIL(vtbl);

	InheritFromVtbl(vtbl, AppGetVtbl());

	vtbl->m_DisposeProcPtr =				OutlineAppDispose;

	vtbl->m_CreateDocProcPtr =				OutlineAppCreateDoc;
	vtbl->m_DoNewProcPtr = 					OutlineAppDoNew;
	vtbl->m_PreUpdateMenusProcPtr =			OutlineAppPreUpdateMenus;
	vtbl->m_GetWindowsMenuHandleProcPtr =	OutlineAppGetWindowsMenuHandle;
	vtbl->m_OpenADocProcPtr =				OutlineAppOpenADoc;
	vtbl->m_DoAboutProcPtr =				OutlineAppDoAbout;

	ASSERTCOND(ValidVtbl(vtbl, sizeof(*vtbl)));
}

//#pragma segment VtblDisposeSeg
void OutlineDisposeVtbl()
{
	ASSERTCOND(gOutlineVtbl != nil);
	DisposePtr((Ptr)gOutlineVtbl);
}

//#pragma segment OutlineInitSeg
void OutlineAppInit(OutlineAppPtr pOutlineApp, OSType creator)
{
	ApplicationPtr		pApp;
	Handle				hMenuBar;
	MenuHandle			hMenu;
	
	pApp = (ApplicationPtr)pOutlineApp;

	AppInit(&pOutlineApp->superClass, creator);

	pOutlineApp->vtbl = ((ApplicationPtr)pOutlineApp)->vtbl;
	((ApplicationPtr)pOutlineApp)->vtbl = OutlineGetVtbl();

	pApp = (ApplicationPtr)pOutlineApp;
	
	pApp->m_NumTypes = 2;
	pApp->m_TypeList[0] = 'LMAN';
	pApp->m_TypeList[1] = kOutlineDocumentFileType;

	// The filter (OutlineAppStandardGetFileFilter) has been screwing
	// things up for some reason so it has been temporarily disabled.
	pApp->m_pFileFilter = nil;

#if qOleContainerApp
	pApp->m_NumTypes = 3;
	pApp->m_TypeList[2] = kOutlineContainerDocumentFileType;
#endif

	pOutlineApp->m_GopherDoCmd = GopherNewDoCmd((DoCmdProcPtr)OutlineAppDoCmd, pOutlineApp);
	FailNIL(pOutlineApp->m_GopherDoCmd);
	GopherAdd(pOutlineApp->m_GopherDoCmd);

	pOutlineApp->m_GopherUpdateMenus = GopherNewUpdateMenus((UpdateMenusProcPtr)OutlineAppUpdateMenus, pOutlineApp);
	FailNIL(pOutlineApp->m_GopherUpdateMenus);
	GopherAdd(pOutlineApp->m_GopherUpdateMenus);

	hMenuBar = GetNewMBar(kMenuBar_MBAR);
	SetMenuBar(hMenuBar);
	DisposeHandle(hMenuBar);

	hMenu = GetMHandle(kApple_MENU);
	ASSERTCOND(hMenu != nil);
	AddResMenu(hMenu, 'DRVR');
	DrawMenuBar();
	
#if qFrameTools
	hMenu = GetMenu(kToolbar_MENU);
	ASSERTCOND(hMenu != nil);
	InsertMenu(hMenu, hierMenu);

	hMenu = GetMenu(kFormulaBar_MENU);
	ASSERTCOND(hMenu != nil);
	InsertMenu(hMenu, hierMenu);

	hMenu = GetMenu(kHeadings_MENU);
	ASSERTCOND(hMenu != nil);
	InsertMenu(hMenu, hierMenu);
#endif

	// make mouse region
	// ((ApplicationPtr)pOutlineApp)->m_MouseRgn = NewRgn();
}

//#pragma segment OutlineSeg
void OutlineAppDispose(ApplicationPtr pApp)
{
	OutlineAppPtr	pOutlineApp;

	pOutlineApp = (OutlineAppPtr)pApp;

	GopherDispose(pOutlineApp->m_GopherDoCmd);
	GopherDispose(pOutlineApp->m_GopherUpdateMenus);

	ASSERTCOND(pOutlineApp->vtbl->m_DisposeProcPtr != nil);
	(*pOutlineApp->vtbl->m_DisposeProcPtr)(pApp);
}

//#pragma segment OutlineSeg
void OutlineAppDoCmd(OutlineAppPtr pOutlineApp, long cmd)
{
	ApplicationPtr	pApp;

	pApp = (ApplicationPtr)pOutlineApp;

	switch(cmd)
	{
		case cmdAbout:
			ASSERTCOND(pApp->vtbl->m_DoAboutProcPtr != nil);
			(*pApp->vtbl->m_DoAboutProcPtr)(pApp);
			break;

		default:
			InheritDoCmd(pOutlineApp->m_GopherDoCmd, cmd);
			break;
	}
}


//#pragma segment OutlineSeg
void OutlineAppUpdateMenus(OutlineAppPtr pOutlineApp)
{
	InheritUpdateMenus(pOutlineApp->m_GopherUpdateMenus);
}

//#pragma segment OutlineSeg
void OutlineAppPreUpdateMenus(ApplicationPtr pApp)
{
	MenuHandle	menu;

	menu = GetMHandle(kFile_MENU);
	if (menu)
		DisableAllItems(menu);

	menu = GetMHandle(kEdit_MENU);
	ASSERTCOND(menu != nil);
	if (menu)
		DisableAllItems(menu);

	menu = GetMHandle(kLines_MENU);
	ASSERTCOND(menu != nil);
	if (menu)
		DisableAllItems(menu);

	menu = GetMHandle(kName_MENU);
	ASSERTCOND(menu != nil);
	if (menu)
		DisableAllItems(menu);
}

//#pragma segment OutlineSeg
MenuHandle OutlineAppGetWindowsMenuHandle(ApplicationPtr pApp)
{
	return GetMHandle(kWindows_MENU);
}

//#pragma segment OutlineSeg
DocumentPtr OutlineAppCreateDoc(ApplicationPtr pApp)
{
	volatile DocumentPtr	pDoc;

	pDoc = nil;

	TRY
	{
		pDoc = (DocumentPtr)NewPtrClear(sizeof(OutlineDocRec));
		
		ASSERTCOND(pDoc != nil);
		FailNIL(pDoc);
		
		OutlineDocInit((OutlineDocPtr)pDoc, kOutline_WIND, kOutlineType, false);
	}
	CATCH
	{
		if (pDoc != nil)
		{
			ASSERTCOND(pDoc->vtbl->m_FreeProcPtr != nil);
			(*pDoc->vtbl->m_FreeProcPtr)(pDoc);
		}
		
		pDoc = nil;
	}
	ENDTRY

	return pDoc;
}

//#pragma segment OutlineSeg
void OutlineAppDoNew(ApplicationPtr pApp)
{
	OutlineDocPtr			pOutlineDoc;
	volatile DocumentPtr	pDoc;

	pOutlineDoc = nil;
	pDoc = nil;

	TRY
	{
		ASSERTCOND(pApp->vtbl->m_CreateDocProcPtr != nil);
		pOutlineDoc = (OutlineDocPtr)(*pApp->vtbl->m_CreateDocProcPtr)(pApp);
		ASSERTCOND(pOutlineDoc != nil);
		FailNIL(pOutlineDoc);
		pDoc = (DocumentPtr)pOutlineDoc;

		ASSERTCOND(pDoc->vtbl->m_DoNewProcPtr != nil);
		(*pDoc->vtbl->m_DoNewProcPtr)(pDoc);

		ASSERTCOND(pDoc->vtbl->m_ShowProcPtr != nil);
		(*pDoc->vtbl->m_ShowProcPtr)(pDoc);
	}
	CATCH
	{
		if (pDoc)
		{
			ASSERTCOND(pDoc->vtbl->m_FreeProcPtr != nil);
			(*pDoc->vtbl->m_FreeProcPtr)(pDoc);
		}
	}
	ENDTRY
}

//#pragma segment OutlineSeg
/* OutlineAppCreateDataXferDoc
 * ------------------------
 *
 *      Create a document to be use to transfer data (either via a
 *  drag/drop operation or the clipboard). A data transfer document is
 *  the same as a document that is created by the user except that it is
 *  NOT made visible to the user. it is specially used to hold a copy of
 *  data that the user should not be able to change.
 *
 */
DocumentPtr OutlineAppCreateDataXferDoc(ApplicationPtr pApp)
{
	volatile DocumentPtr	pDoc;

	pDoc = nil;

	TRY
	{
		pDoc = (DocumentPtr)NewPtrClear(sizeof(OutlineDocRec));
		
		ASSERTCOND(pDoc != nil);
		FailNIL(pDoc);
		
		OutlineDocInit((OutlineDocPtr)pDoc, kOutline_WIND, kOutlineType, true);
	}
	CATCH
	{
		if (pDoc != nil)
		{
			ASSERTCOND(pDoc->vtbl->m_FreeProcPtr != nil);
			(*pDoc->vtbl->m_FreeProcPtr)(pDoc);
		}
	}
	ENDTRY

	return pDoc;
}

//#pragma segment OutlineSeg
void OutlineAppDoAbout(ApplicationPtr pApp)
{
	short		itemHit;
	DialogPtr	theDialog;
	char		minorVersion[12];
	char		majorVersion[12];
	char		appType[20];
	
	NumToString((long)APPMAJORVERSIONNO, (StringPtr)majorVersion);
	NumToString((long)APPMINORVERSIONNO, (StringPtr)minorVersion);
	appType[0] = sprintf(&appType[1], APPTYPE);
	ParamText((StringPtr)appType, (StringPtr)majorVersion, (StringPtr)minorVersion, nil);
	theDialog = GetNewDialog(kAbout_DLOG, nil, (WindowPtr)-1);
	FailNIL(theDialog);
	
	ModalDialog(nil, &itemHit);
	
	DisposDialog(theDialog);
}

//#pragma segment OutlineSeg
#ifndef _MSC_VER
pascal Boolean
#else
Boolean __pascal
#endif
OutlineAppStandardGetFileFilter(ParmBlkPtr p)
{
	OSType	creator;
	
	creator = p->fileParam.ioFlFndrInfo.fdCreator;

	// we will probable need more logic, not all mdos files will have this creator type, BK
	if (creator == 'mdos' || creator == 'LMAN')		// allows DOS file to show up
		return false;
	
	if (p->fileParam.ioFlFndrInfo.fdType == kOutlineDocumentFileType)
		return false;
				
#if qOleContainerApp
	// container can open container document in addition to outline document
	if (p->fileParam.ioFlFndrInfo.fdType == kOutlineContainerDocumentFileType)
		return false;
#endif

	return true;	// reject
}

//#pragma segment OutlineSeg
Boolean OutlineAppOpenADoc(ApplicationPtr pApp, struct FSSpec* spec, OSType type)
{
	Boolean					fOpened = false;
	OutlineDocPtr			pOutlineDoc;
	volatile DocumentPtr	pDoc;

	pOutlineDoc = nil;
	pDoc = nil;

	TRY
	{
		// see if a document is already open that refers to this file
		ASSERTCOND(pApp->vtbl->m_DocFromFSSpecProcPtr != nil);
		pOutlineDoc = (OutlineDocPtr)(*pApp->vtbl->m_DocFromFSSpecProcPtr)(pApp, spec);
		if (pOutlineDoc != nil)
		{
			WindowPtr	pWindow;

			// extract the window associated with this document
			ASSERTCOND(pOutlineDoc->vtbl->m_GetWindowProcPtr != nil);
			pWindow = (*pOutlineDoc->vtbl->m_GetWindowProcPtr)((DocumentPtr)pOutlineDoc);

			// the result of this function is true if a new window is opened
			// or activated and false if we don't switch windows. this result
			// is used later to determine whether inplace should be resumed
			// in the front window.
			if (pWindow != FrontWindow())
			{
				SelectWindow(pWindow);
				fOpened = true;
			}
		}
		else
		{
			ASSERTCOND(pApp->vtbl->m_CreateDocProcPtr != nil);
			pOutlineDoc = (OutlineDocPtr)(*pApp->vtbl->m_CreateDocProcPtr)(pApp);
			ASSERTCOND(pOutlineDoc != nil);
			FailNIL(pOutlineDoc);
			pDoc = (DocumentPtr)pOutlineDoc;

			ASSERTCOND(pDoc->vtbl->m_DoOpenProcPtr != nil);
			(*pDoc->vtbl->m_DoOpenProcPtr)(pDoc, spec, false);

			ASSERTCOND(pDoc->vtbl->m_SetDirtyProcPtr != nil);
			(*pDoc->vtbl->m_SetDirtyProcPtr)(pDoc, false);

			ASSERTCOND(pDoc->vtbl->m_ShowProcPtr != nil);
			(*pDoc->vtbl->m_ShowProcPtr)(pDoc);

			fOpened = true;
		}
	}
	CATCH
	{
		if (pDoc)
		{
			ASSERTCOND(pDoc->vtbl->m_FreeProcPtr != nil);
			(*pDoc->vtbl->m_FreeProcPtr)(pDoc);
		}
	}
	ENDTRY

	return fOpened;
}

#if qOle

// OLDNAME: OleApplication.c


#ifndef _MSC_VER
static pascal OSErr OleAppAEEmbedding(AppleEvent* theAppleEvent, AppleEvent* reply, OleApplicationPtr pOleApp);
#else
static OSErr __pascal OleAppAEEmbedding(AppleEvent* theAppleEvent, AppleEvent* reply, OleApplicationPtr pOleApp);
#endif

#ifdef _ASLM
OSErr InitLibraryManager(unsigned long poolsize, int zoneType, int memType);
void CleanupLibraryManager(void);
#endif

OleApplicationPtr		gOleApp = nil;
char					gApplicationName[33];			// max name is 32+1

//#pragma segment OleApplicationInitSeg
void OleAppInit(OleApplicationPtr pOleApp, AppImplPtr pIApp, IUnknown* pIUnknown)
{
	OLEDEBUG_BEGIN3("OleAppInit\n");
	
	gOleApp = pOleApp;

	pOleApp->m_pIApp =			pIApp;
	pOleApp->m_pIUnknown =	 	pIUnknown;

	pOleApp->m_GopherFilterCmd = GopherNewFilterCmd((FilterCmdProcPtr)OleAppFilterCmd, pOleApp);
	FailNIL(pOleApp->m_GopherFilterCmd);
	GopherAdd(pOleApp->m_GopherFilterCmd);


	// setup applicaton name from low memory global
	pStrCopy((StringPtr)gApplicationName, LMGetCurApName());
	p2cstr((StringPtr)gApplicationName);
	
	pOleApp->m_fOleInitialized = false;
	
	pOleApp->m_pClipboardDoc = nil;
	pOleApp->m_pDragSourceDoc = nil;	
	
#ifdef _ASLM
	{
		OSErr		err;
		
		err = InitLibraryManager(0, 4, 1);
		ASSERTCOND(err == noErr);
		FailOSErr(err);
	}
#else
	{
		HRESULT		theResult;

		OLEDEBUG_BEGIN2("InitOleManager called\n");
#ifndef __powerc
		SetGrowZone(MyGrowZoneProc);
#else
		if (gMyGrowZoneProc == NULL)
		    gMyGrowZoneProc = NewGrowZoneProc(MyGrowZoneProc);

		SetGrowZone(gMyGrowZoneProc);
#endif


#ifndef __powerc
		theResult = InitOleManager(0);

		OLEDEBUG_END2

		ASSERTCOND(theResult == NOERROR);
		FailOleErr(theResult);
#endif // __powerc
	}
#endif

	{
		HRESULT		theResult;

		OLEDEBUG_BEGIN2("OleInitialize called\n");

		theResult = OleInitialize(nil);

		OLEDEBUG_END2

		ASSERTCOND(theResult == NOERROR);
		FailOleErr(theResult);
	}

	// make sure we remember that Ole has been initialized
	pOleApp->m_fOleInitialized = true;

	OleDocInitInterfaces();

#if qOleClassFactory
	OleAppClassFactoryInitInterfaces();
#endif

#if qOleMessageFilter
	OleStdMessageFilterInitInterfaces();
#endif

	OleDataXferDocInitInterfaces();

	EnumFORMATETCInitInterface();	

	pOleApp->m_fUserInControl				= true;

	// Register OLE 2.0 clipboard
	pOleApp->m_cfEmbedSource				= cfEmbedSource;
	pOleApp->m_cfEmbeddedObject				= cfEmbeddedObject;
	pOleApp->m_cfLinkSource					= cfLinkSource;
	pOleApp->m_cfObjectDescriptor			= cfObjectDescriptor;
	pOleApp->m_cfLinkSrcDescriptor			= cfLinkSrcDescriptor;

	pOleApp->m_cDoc							= 0;
	pOleApp->m_dwRegisteredClassFactory		= 0;
	pOleApp->m_lpClassFactory				= nil;

	pOleApp->m_nDocGetFmts					= 0;
	pOleApp->m_nPasteEntries				= 0;
	pOleApp->m_nLinkTypes					= 0;

	OLEDEBUG_END3

#if qOleServerApp
	OleServerAppInit(pOleApp);
#elif qOleContainerApp
	OleContainerAppInit(pOleApp);
#endif
}

//#pragma segment OleApplicationSeg
void OleAppDispose(OleApplicationPtr pOleApp)
{
	// if we're shutting down because our last external reference has
	// dropped to zero rather than through user control we can still
	// have a clipboard doc sitting around that we need to flush.
	if (pOleApp->m_pClipboardDoc != nil)
		OleAppFlushClipboard(pOleApp);

	ASSERTCONDSZ((pOleApp->m_pClipboardDoc == nil),  "Exit without flushing clipboard");
	ASSERTCONDSZ((pOleApp->m_pDragSourceDoc == nil), "Exit with hanging drag source document");
	
#if qOleContainerApp
	OleContainerAppDispose(pOleApp);
#elif qOleServerApp
	OleServerAppDispose(pOleApp);
#endif // qOleContainerApp

#if qOleClassFactory
	OleAppRevokeClassFactory(pOleApp);
#endif

#if qOleMessageFilter
	OleMessageFilterRevoke(pOleApp);
#endif

	GopherDispose(pOleApp->m_GopherFilterCmd);

	if (pOleApp->m_fOleInitialized)
		OleUninitialize();
		
#ifdef _ASLM
	CleanupLibraryManager();
#else	
#ifndef __powerc
	UninitOleManager();
#endif // __powerc
#endif
}

//#pragma segment OleApplicationSeg
void OleAppDoStartup(OleApplicationPtr pOleApp)
{	
	OSErr err;
	
	err = AEInstallEventHandler(typeDDE, typeEMBD, NewAEEventHandlerProc(OleAppAEEmbedding), (long)pOleApp, false);
	ASSERTCOND(err == noErr);
	FailOSErr(err);

#if qOleContainerApp && qOleInPlace

	err = AEInstallEventHandler('OLE2', 'EVNT', NewAEEventHandlerProc(OleInPlaceContainerRemoteEvent), (long)pOleApp, false);
	ASSERTCOND(err == noErr);
	FailOSErr(err);
#endif

	// make sure application is registered in registration database
	// This should go into a resource, but for the moment it is hard coded
	{
		OLEREGSTATUS		regStatus;
		HKEY				hKey;

		// check and register OLE 1.0 information
#if qOleServerApp
#ifdef __NOTYET__
		hKey = OleregOpenRegistration();
		if (hKey == OLEREG_OK)
		{
			long	len;
			
			// check to see if we are already registered under OLE 1.0
			len = 4;
			regStatus = OleregGetValue(kOutlineServerProgID, OLEREGSVR_SIGNATURE, REG_IGNORE, kOutlineServerTypeStr, &len);
			if (regStatus != OLEREG_OK)
			{
#endif
				regStatus = OleregSetValue(kOutlineServerProgID, OLEREGSVR_SIGNATURE, REG_IGNORE, kOutlineServerTypeStr, 4);
				ASSERTCOND(regStatus == 0);
				FaileOleRegErr(regStatus);
				regStatus = OleregSetValue(kOutlineServerProgID, OLEREGSVR_HUMAN_READABLE, REG_IGNORE, kOutlineServerFullUserTypeName, strlen(kOutlineServerFullUserTypeName));
				ASSERTCOND(regStatus == 0);
				FaileOleRegErr(regStatus);
				regStatus = OleregSetValue(kOutlineServerProgID, OLEREGSVR_VERB, REG_REPLACE, "Edit", 4);
				ASSERTCOND(regStatus == 0);
				FaileOleRegErr(regStatus);
#ifdef __NOTYET__
			}
			
			OleregCloseRegistration(hKey);
		}
#endif
#endif

		// check and register OLE 2.0 information
#if qOleServerApp
		regStatus = RegOpenKey(HKEY_CLASSES_ROOT, kOutlineServerTypeStr, &hKey);
#elif qOleContainerApp
		regStatus = RegOpenKey(HKEY_CLASSES_ROOT, kOutlineContainerTypeStr, &hKey);
#endif
		if (regStatus == OLEREG_OK)
			RegCloseKey(hKey);
		else
		{	
#if qOleServerApp
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID, REG_SZ, kOutlineServerFullUserTypeName, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\LocalServer", REG_SZ, kOutlineServerTypeStr, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\InprocHandler", REG_SZ, "OLE2:Def$DefFSet", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\ProgID", REG_SZ, kOutlineServerProgID, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\Insertable",	 REG_SZ, "", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\AuxUserType\\2",	 REG_SZ, kOutlineServerShortUserTypeName, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\AuxUserType\\3",	 REG_SZ, kOutlineServerAppName, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\Conversion\\Readwritable\\Main", REG_SZ, kOutlineDocumentFile", TEXT", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\Conversion\\Readable\\Main", REG_SZ, kOutlineDocumentFile", TEXT", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\DataFormats\\GetSet\\0", REG_SZ, kOutlineDocumentFile",1,1,3", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\DataFormats\\GetSet\\1", REG_SZ, "EMBS,1,8,1", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\DataFormats\\GetSet\\2", REG_SZ, "TEXT,1,1,3", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\DataFormats\\GetSet\\3", REG_SZ, "PICT,1,32,1", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\MiscStatus", REG_SZ, "512", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);

#if qOleInPlace
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\MiscStatus\\1", REG_SZ, "896", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
#endif

			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\Verb\\0", REG_SZ, "Edit,0,2", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			
#if qOleInPlace			
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\Verb\\1", REG_SZ, "Open,0,2", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
#endif

			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\Conversion\\Readable\\Main", REG_SZ, kOutlineDocumentFile, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineServerCLSID"\\Conversion\\Readwritable\\Main", REG_SZ, kOutlineDocumentFile, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, kOutlineServerProgID, REG_SZ, kOutlineServerFullUserTypeName, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, kOutlineServerProgID"\\CLSID", REG_SZ, kOutlineServerCLSID, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, kOutlineServerProgID"\\Insertable", REG_SZ, "", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, kOutlineServerProgID"\\protocol\\StdFileEditing\\verb\\0", REG_SZ, "Edit", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);

#if qOleInPlace
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, kOutlineServerProgID"\\protocol\\StdFileEditing\\verb\\1", REG_SZ, "Open", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, kOutlineServerProgID"\\protocol\\StdFileEditing\\server", REG_SZ, kOutlineServerTypeStr, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
#endif


#elif qOleContainerApp
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineContainerCLSID, REG_SZ, kOutlineContainerFullUserTypeName, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineContainerCLSID"\\LocalServer", REG_SZ, kOutlineContainerTypeStr, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineContainerCLSID"\\InprocHandler", REG_SZ, "OLE2:Def$DefFSet", 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, "CLSID\\"kOutlineContainerCLSID"\\ProgID", REG_SZ, kOutlineContainerProgID, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
			regStatus = RegSetValue(HKEY_CLASSES_ROOT, kOutlineContainerProgID"\\CLSID", REG_SZ, kOutlineContainerCLSID, 0);
			ASSERTCOND(regStatus == 0);
			FaileOleRegErr(regStatus);
#endif
		}

	}

#if qOleClassFactory
	// just about there, now register the class factory
	OleAppRegisterClassFactory(pOleApp);
#endif

#if qOleMessageFilter
	OleMessageFilterRegister(pOleApp);
#endif
	
	// need to put a lock on the application for the user
	ASSERTCOND(pOleApp->m_fUserInControl == true);
	OleAppLock(pOleApp, true, false);
}

//#pragma segment OleApplicationSeg
#ifndef _MSC_VER
static pascal OSErr
#else
static OSErr __pascal
#endif
OleAppAEEmbedding(AppleEvent* theAppleEvent, AppleEvent* reply, OleApplicationPtr pOleApp)
{
	ASSERTCOND(pOleApp->m_fUserInControl == true);

	// we are embedded, so the user is currently not in control
	pOleApp->m_fUserInControl = false;
	
	// need to remove user lock, making sure that the app is not disposed
	OleAppLock(pOleApp, false, false);
		
	return noErr;
}

//#pragma segment OleApplicationSeg
void OleAppFilterCmd(OleApplicationPtr pOleApp, long cmd)
{	
	switch(cmd)
	{
		case cmdNew:
		case cmdOpen:
		case cmdSaveAs:
			// set flag indicating the user is in control
			if (!pOleApp->m_fUserInControl)
			{
				pOleApp->m_fUserInControl = true;
				
				OleAppLock(pOleApp, true, false);
			}
			break;
	}
	
	InheritFilterCmd(pOleApp->m_GopherFilterCmd, cmd);
}

//#pragma segment OleApplicationSeg
void OleAppDoQuit(OleApplicationPtr pOleApp, Boolean askUser, YNCResult defaultResult)
{
	OleAppAddRef(pOleApp);			// artificial addref
	
	// if the user is in control, do not forget to unlock the app
	if (pOleApp->m_fUserInControl)
		OleAppLock(pOleApp, false, true);

	CoDisconnectObject((LPUNKNOWN)pOleApp->m_pIUnknown, 0);
	
	OleAppRelease(pOleApp);			// release artificial addref
}

//#pragma segment OleApplicationSeg
void OleAppDoNew(OleApplicationPtr pOleApp)
{
#if qOleContainerApp && qOleInPlace
	OleInPlaceContainerAppDoNew(pOleApp);
#endif
}

//#pragma segment OleApplicationSeg
Boolean OleAppDoSuspend(OleApplicationPtr pOleApp)
{	
#if qOleInPlace && qOleServerApp
	return OleInPlaceServerAppDoSuspend(pOleApp);
#elif qOleInPlace && qOleContainerApp
	return OleInPlaceContainerAppDoSuspend(pOleApp);
#endif

	return true;
}

//#pragma segment OleApplicationSeg
Boolean OleAppDoResume(OleApplicationPtr pOleApp)
{
#if qOleInPlace
#if qOleServerApp
	return OleInPlaceServerAppDoResume(pOleApp);
#elif qOleContainerApp
	return OleInPlaceContainerAppDoResume(pOleApp);
#endif
#endif

	return true;
}

#if qOleClassFactory
//#pragma segment OleApplicationSeg
void OleAppRegisterClassFactory(OleApplicationPtr pOleApp)
{
	HRESULT		hrErr;

	ASSERTCOND(pOleApp->m_lpClassFactory == nil);

	pOleApp->m_lpClassFactory = OleAppClassFactoryCreate(pOleApp);
	ASSERTCOND(pOleApp->m_lpClassFactory != nil);
	FailNIL(pOleApp->m_lpClassFactory);
	
	TRY
	{
		hrErr = CoRegisterClassObject(
							&CLSID_Application,
							(LPUNKNOWN)pOleApp->m_lpClassFactory,
							CLSCTX_LOCAL_SERVER,
							REGCLS_MULTIPLEUSE,
							&pOleApp->m_dwRegisteredClassFactory
				);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
	}
	CATCH
	{
		OleStdRelease((LPUNKNOWN)pOleApp->m_lpClassFactory);
	}
	ENDTRY
}

//#pragma segment OleApplicationSeg
void OleAppRevokeClassFactory(OleApplicationPtr pOleApp)
{
	HRESULT				hrErr;
	unsigned long		cRef;

	if (pOleApp->m_lpClassFactory == nil)
		return;

	hrErr = CoRevokeClassObject(pOleApp->m_dwRegisteredClassFactory);
	ASSERTNOERROR(hrErr);
	FailOleErr(hrErr);

	cRef = OleStdVerifyRelease((LPUNKNOWN)pOleApp->m_lpClassFactory);
	ASSERTCOND(cRef == 0);

	pOleApp->m_dwRegisteredClassFactory = 0;
}
#endif // qOleClassFactory

//#pragma segment OleApplicationSeg
unsigned long OleAppAddRef(OleApplicationPtr pOleApp)
{
	return (*pOleApp->m_pIUnknown->lpVtbl->AddRef)(pOleApp->m_pIUnknown);
}

//#pragma segment OleApplicationSeg
unsigned long OleAppRelease(OleApplicationPtr pOleApp)
{
	return (*pOleApp->m_pIUnknown->lpVtbl->Release)(pOleApp->m_pIUnknown);
}

//#pragma segment OleApplicationSeg
HRESULT OleAppLocalQueryInterface(OleApplicationPtr pOleApp, REFIID riid, void* * lplpvObj)
{
#if qOleContainerApp
	return OleContainerAppLocalQueryInterface(pOleApp, riid, lplpvObj);
#endif

	*lplpvObj = NULL;
	return ResultFromScode(E_NOINTERFACE);
}

//#pragma segment OleApplicationSeg
HRESULT OleAppQueryInterface(OleApplicationPtr pOleApp, REFIID riid, void* * lplpvObj)
{
	return (*pOleApp->m_pIUnknown->lpVtbl->QueryInterface)(pOleApp->m_pIUnknown, riid, lplpvObj);
}

//#pragma segment OleApplicationSeg
HRESULT OleAppLock(OleApplicationPtr pOleApp, Boolean fLock, Boolean fLastUnlockReleases)
{
	HRESULT		hrErr;

	OleAppAddRef(pOleApp);			// artificial AddRef to make object stable

	hrErr = CoLockObjectExternal(pOleApp->m_pIUnknown, fLock, fLastUnlockReleases);
	ASSERTNOERROR(hrErr);

	OleAppRelease(pOleApp);			// release artificial AddRef above;

	return hrErr;
}

//#pragma segment OleApplicationSeg
void OleAppDocLock(OleApplicationPtr pOleApp)
{	
	++pOleApp->m_cDoc;
	
	OleAppLock(pOleApp, TRUE /* fLock */, FALSE);
	
	return;
}

//#pragma segment OleApplicationSeg
void OleAppDocUnlock(OleApplicationPtr pOleApp)
{
	ASSERTCOND(pOleApp->m_cDoc > 0);
	
	--pOleApp->m_cDoc;
	
	OleAppLock(pOleApp, FALSE /* fLock */, TRUE /* fLastUnlockReleases */);
}

//#pragma segment OleApplicationSeg
void OleAppAddDocGetFormat(OleApplicationPtr pOleApp, LPFORMATETC pFormatetc)
{
	ASSERTCOND(pOleApp && pFormatetc);
	
	if (pOleApp->m_nDocGetFmts < kMaxNumOfMTS)
		pOleApp->m_arrDocGetFmts[pOleApp->m_nDocGetFmts++] = *pFormatetc;
}

//#pragma segment OleApplicationSeg
void OleAppAddPasteEntry(OleApplicationPtr pOleApp, LPOLEUIPASTEENTRY pPasteEntry)
{
	ASSERTCOND(pOleApp && pPasteEntry);
	
	if (pOleApp->m_nPasteEntries < kMaxNumOfMTS)
		pOleApp->m_arrPasteEntries[pOleApp->m_nPasteEntries++] = *pPasteEntry;
}

//#pragma segment OleApplicationSeg
void OleAppAddLinkType(OleApplicationPtr pOleApp, unsigned int LinkType)
{
	ASSERTCOND(pOleApp && LinkType);
	
	if (pOleApp->m_nLinkTypes < kMaxNumOfMTS)
		pOleApp->m_arrLinkTypes[pOleApp->m_nLinkTypes++] = LinkType;
}

//#pragma segment OleApplicationSeg
void OleAppFlushClipboard(OleApplicationPtr pOleApp)
{
    OleDocumentPtr pClipboardDoc = pOleApp->m_pClipboardDoc;
    LPDATAOBJECT	pDataObj;
	HRESULT			hrErr;
	unsigned long	cRef;
	
	if (!pClipboardDoc)
		return;
		
	hrErr = OleGetClipboard(&pDataObj);
	ASSERTCOND(hrErr == S_OK);
	
    /* OLE2NOTE: if for some reason our clipboard data transfer
    **    document is still held on to by an external client, we want
    **    to forceably break all external connections.
    */
    ASSERTCOND(pClipboardDoc->m_pIUnknown != nil);
    hrErr = CoDisconnectObject(pClipboardDoc->m_pIUnknown, 0);
	ASSERTCOND(hrErr == S_OK);

    hrErr = OleFlushClipboard();
	ASSERTCOND(hrErr == S_OK);
	
	if (pDataObj) {
		cRef = pDataObj->lpVtbl->Release(pDataObj);
		ASSERTCOND(cRef == 0);
	}

	pOleApp->m_pClipboardDoc = nil;
}

//#pragma segment OleApplicationSeg
void OleAppEnableDialog(OleApplicationPtr pOleApp)
{
	OleDocumentPtr		pOleDoc;
	
	ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr != nil);
	pOleDoc = (*pOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr)(pOleApp->m_pIApp);
	
	if (pOleDoc)
		OleDocEnableDialog(pOleDoc);
}

//#pragma segment OleApplicationSeg
void OleAppDisableDialog(OleApplicationPtr pOleApp, Boolean resumeInPlace)
{
	OleDocumentPtr		pOleDoc;
	
	ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr != nil);
	pOleDoc = (*pOleApp->m_pIApp->lpVtbl->m_GetCurrentOleDocProcPtr)(pOleApp->m_pIApp);
	
	if (pOleDoc)
		OleDocDisableDialog(pOleDoc, resumeInPlace);
}

#ifndef _MSC_VER
pascal long
#else
long __pascal
#endif
MyGrowZoneProc(Size cbNeeded)
{
	return 0;	// indicate we have not been helpful
}

// OLDNAME: OleOutline.c

static OleOutlineVtblPtr	gOleOutlineVtbl = nil;



//#pragma segment VtblInitSeg
OleOutlineVtblPtr OleOutlineGetVtbl()
{
	ASSERTCOND(gOleOutlineVtbl != nil);
	return gOleOutlineVtbl;
}

//#pragma segment VtblInitSeg
void OleOutlineInitVtbl()
{
	OleOutlineVtblPtr	vtbl;

	vtbl = gOleOutlineVtbl = (OutlineVtblPtr)NewPtrClear(sizeof(*vtbl));
	ASSERTCOND(vtbl != nil);
	FailNIL(vtbl);

	InheritFromVtbl(vtbl, OutlineGetVtbl());

	vtbl->m_DisposeProcPtr =				OleOutlineAppDispose;
	vtbl->m_FreeProcPtr =					OleOutlineAppFree;
	
	vtbl->m_CreateDocProcPtr =				OleOutlineAppCreateDoc;
	vtbl->m_DoNewProcPtr =					OleOutlineAppDoNew;
	vtbl->m_DoOpenProcPtr =					OleOutlineAppDoOpen;
	vtbl->m_DoAboutProcPtr =				OleOutlineAppDoAbout;
	
	vtbl->m_DoStartupProcPtr =				OleOutlineAppDoStartup;
			
#if qOleContainerApp && qOleInPlace
	vtbl->m_SetIdleCursorProcPtr =			OleOutlineInPlaceContainerSetIdleCursor;
#endif

	vtbl->m_IsReadyToQuitProcPtr =			OleOutlineAppIsReadyToQuit;
	vtbl->m_DoQuitProcPtr =					OleOutlineAppDoQuit;
	vtbl->m_FlushClipboardProcPtr = 		OleOutlineAppFlushClipboard;
		
	vtbl->m_DoSuspendProcPtr =				OleOutlineAppDoSuspend;
	vtbl->m_DoResumeProcPtr =				OleOutlineAppDoResume;

	vtbl->m_GetNextNewFileNameProcPtr = 	OleOutlineAppGetNextNewFileName;
			
#if qOleServerApp && qOleInPlace
	vtbl->m_ProcessEventProcPtr =			OleOutlineInPlaceServerProcessEvent;
	vtbl->m_DoUnknownMenuKeyProcPtr =		OleOutlineInPlaceServerDoUnknownMenuKey;
	vtbl->m_DoUnknownMenuItemProcPtr =		OleOutlineInPlaceServerDoUnknownMenuItem;
	vtbl->m_HideOthersCmdProcPtr =			OleOutlineInPlaceServerHideOthersCmd;
#endif

#if qOleContainerApp && qOleInPlace
	vtbl->m_MenuItemToCmdProcPtr =			OleOutlineInPlaceContainerMenuItemToCmd;
#endif

	ASSERTCOND(ValidVtbl(vtbl, sizeof(*vtbl)));
}

//#pragma segment VtblDisposeSeg
void OleOutlineDisposeVtbl()
{
	ASSERTCOND(gOleOutlineVtbl != nil);
	DisposePtr((Ptr)gOleOutlineVtbl);
}


//#pragma segment OleOutlineInitSeg
void OleOutlineAppInit(OleOutlineAppPtr pOleOutlineApp, OSType creator)
{
	OleApplicationPtr	pOleApp;
	CursHandle			hCursor;
	
	OutlineAppInit(&pOleOutlineApp->superClass, creator);
	
	// now make sure that all the appropriate interfaces are initialized
	OleOutlineAppInitInterfaces();
	OleOutlineDocInitInterfaces();

#if qOleContainerApp
	OleOutlineContainerDocInitInterfaces();

	OleContainerLineInitInterfaces();
#endif
	
	OleOutlineAppIUnknownInit(&pOleOutlineApp->m_IUnknown, pOleOutlineApp);
	OleOutlineAppIAppInit(&pOleOutlineApp->m_IApp, pOleOutlineApp);

	pOleApp = &pOleOutlineApp->m_OleApp;

	OleAppInit(pOleApp, (AppImpl*)&pOleOutlineApp->m_IApp, (IUnknown*)&pOleOutlineApp->m_IUnknown);
	
#if qOleServerApp
	OleOutlineServerInit(pOleOutlineApp, creator);
#elif qOleContainerApp
	OleOutlineContainerInit(pOleOutlineApp, creator);
#endif // qOleContainerApp

	pOleOutlineApp->vtbl = ((ApplicationPtr)pOleOutlineApp)->vtbl;
	((ApplicationPtr)pOleOutlineApp)->vtbl = OleOutlineGetVtbl();
	
	pOleOutlineApp->m_GopherDoCmd = GopherNewDoCmd((DoCmdProcPtr)OleOutlineAppDoCmd, pOleOutlineApp);
	FailNIL(pOleOutlineApp->m_GopherDoCmd);
	GopherAdd(pOleOutlineApp->m_GopherDoCmd);

	pOleOutlineApp->m_GopherUpdateMenus = GopherNewUpdateMenus((UpdateMenusProcPtr)OleOutlineAppUpdateMenus, pOleOutlineApp);
	FailNIL(pOleOutlineApp->m_GopherUpdateMenus);
	GopherAdd(pOleOutlineApp->m_GopherUpdateMenus);

	pOleOutlineApp->m_cDoc = 0;
	pOleOutlineApp->m_cRef = 0;
	
}

//#pragma segment OleOutlineSeg
void OleOutlineAppDispose(ApplicationPtr pApp)
{
	OleOutlineAppPtr	pOleOutlineApp;
	
	pOleOutlineApp = (OleOutlineAppPtr)pApp;
	
	ASSERTCOND(pOleOutlineApp->m_cDoc == 0);
	ASSERTCOND(pOleOutlineApp->m_cRef == 0);
	
	GopherDispose(pOleOutlineApp->m_GopherDoCmd);
	GopherDispose(pOleOutlineApp->m_GopherUpdateMenus);

	OleAppDispose(&pOleOutlineApp->m_OleApp);
	
	ASSERTCOND(pOleOutlineApp->vtbl->m_DisposeProcPtr != nil);
	(*pOleOutlineApp->vtbl->m_DisposeProcPtr)(pApp);
}

//#pragma segment OleOutlineSeg
void OleOutlineAppFree(ApplicationPtr pApp)
{
	// calling Free has no effect on a component object. Instead
	// we wait until the reference count drops to zero, at which
	// point the IsReadyToQuit function returns true and we exit
	// out of the event loop.
}

//#pragma segment OutlineSeg
void OleOutlineAppDoCmd(OleOutlineAppPtr pOleOutlineApp, long cmd)
{
	ApplicationPtr	pApp;

	pApp = (ApplicationPtr)pOleOutlineApp;

	switch(cmd)
	{
#if qDebug && qOleMessageFilter
		case cmdRejectMessages:
			{
				Boolean		fReject;

				fReject = OleMessageFilterGetRejectStatus(&pOleOutlineApp->m_OleApp);
				OleMessageFilterSetRejectStatus(&pOleOutlineApp->m_OleApp, (Boolean)!fReject);
				CheckCmd(cmdRejectMessages, OleMessageFilterGetRejectStatus(&pOleOutlineApp->m_OleApp));
			}
			break;
#endif

		default:
			InheritDoCmd(pOleOutlineApp->m_GopherDoCmd, cmd);
			break;
	}
}

//#pragma segment OutlineSeg
void OleOutlineAppUpdateMenus(OleOutlineAppPtr pOleOutlineApp)
{
#if qDebug && qOleMessageFilter
	EnableCmd(cmdRejectMessages);
	CheckCmd(cmdRejectMessages, OleMessageFilterGetRejectStatus(&pOleOutlineApp->m_OleApp));
#endif

	InheritUpdateMenus(pOleOutlineApp->m_GopherUpdateMenus);
}

//#pragma segment OleOutlineSeg
unsigned long OleOutlineAppAddRef(OleOutlineAppPtr pOleOutlineApp)
{
	++pOleOutlineApp->m_cRef;

	return pOleOutlineApp->m_cRef;
}

//#pragma segment OleOutlineSeg
unsigned long OleOutlineAppRelease(OleOutlineAppPtr pOleOutlineApp)
{
	unsigned long			cRef;

	ASSERTCOND(pOleOutlineApp->m_cRef > 0);

	cRef = --pOleOutlineApp->m_cRef;

	if (cRef == 0)
	{
		// we do not release memory held here because we want to let the application
		// exit gracefully back to main. From main we free the memory the application
		// holds
	}

	return cRef;
}

//#pragma segment OleOutlineSeg
HRESULT OleOutlineAppQueryInterface(OleOutlineAppPtr pOleOutlineApp, REFIID riid, void* * lplpvObj)
{
	if (IsEqualIID(riid, &IID_IUnknown))
	{
		*lplpvObj = &pOleOutlineApp->m_IUnknown;
		OleOutlineAppAddRef(pOleOutlineApp);
		return NOERROR;
	}
	else
		return OleAppLocalQueryInterface(&pOleOutlineApp->m_OleApp, riid, lplpvObj);
}

//#pragma segment OleOutlineSeg
void OleOutlineAppDocLock(OleOutlineAppPtr pOleOutlineApp)
{
	++pOleOutlineApp->m_cDoc;
	
	OleAppLock(&pOleOutlineApp->m_OleApp, TRUE /* fLock */, FALSE);
	
	return;
}

//#pragma segment OleOutlineSeg
void OleOutlineAppDocUnlock(OleOutlineAppPtr pOleOutlineApp)
{
	ASSERTCOND(pOleOutlineApp->m_cDoc > 0);
	
	--pOleOutlineApp->m_cDoc;
	
	OleAppLock(&pOleOutlineApp->m_OleApp, FALSE /* fLock */, TRUE /* fLastUnlockReleases */);
}


//#pragma segment OleOutlineSeg
/* OleOutlineAppVersionNoCheck
 * ---------------------------
 *
 *      Check if the version stamp read from a file is compatible
 *      with the current instance of the application.
 *      Generate an exception if the file is not readable.
 */
void OleOutlineAppVersionNoCheck(OleOutlineAppPtr pOleOutlineApp, char* pszFormatName, short narrAppVersionNo[])
{
#if qOleContainerApp

    /* ContainerApp accepts both CF_OUTLINE and CF_CONTAINEROUTLINE formats */
    if (strcmp(pszFormatName, CONTAINERDOCFORMAT) &&
    	strcmp(pszFormatName, OUTLINEDOCFORMAT)) {
		ASSERTCONDSZ(0, "File is either corrupted or not of proper type.");
        FailNIL(NULL);
    }

#elif qOleServerApp

    /* OutlineApp accepts CF_OUTLINE format only */
    if (strcmp(pszFormatName, OUTLINEDOCFORMAT) != 0) {
		ASSERTCONDSZ(0, "File is either corrupted or not of proper type.");
        FailNIL(NULL);
    }
#endif

    if (narrAppVersionNo[0] < APPMAJORVERSIONNO) {
		ASSERTCONDSZ(0, "File was created by an older version; it can not be read.");
        FailNIL(NULL);
    }
}


//#pragma segment OleOutlineSeg
/* OleOutlineAppGetAppVersionNo
 * ----------------------------
 *
 *      Get the version number (major and minor) of the application
 */
void OleOutlineAppGetAppVersionNo(OleOutlineAppPtr pOleOutlineApp, short narrAppVersionNo[])
{
    narrAppVersionNo[0] = APPMAJORVERSIONNO;
    narrAppVersionNo[1] = APPMINORVERSIONNO;
}


//#pragma segment OleOutlineSeg
DocumentPtr OleOutlineAppCreateDoc(ApplicationPtr pApp)
{
	volatile DocumentPtr	pDoc;

	pDoc = nil;

	TRY
	{
		pDoc = (DocumentPtr)NewPtrClear(sizeof(OleOutlineDocRec));
		
		ASSERTCOND(pDoc != nil);
		FailNIL(pDoc);

#if qOleContainerApp
		OleOutlineDocInit((OleOutlineDocPtr)pDoc, kOutline_WIND, kOutlineContainerDocumentFileType, false);		
#else
		OleOutlineDocInit((OleOutlineDocPtr)pDoc, kOutline_WIND, kOutlineDocumentFileType, false);
#endif

	}
	CATCH
	{
		if (pDoc != nil)
		{
			ASSERTCOND(pDoc->vtbl->m_FreeProcPtr != nil);
			(*pDoc->vtbl->m_FreeProcPtr)(pDoc);
		}
	}
	ENDTRY

	return pDoc;
}

//#pragma segment OleOutlineSeg
void OleOutlineAppDoNew(ApplicationPtr pApp)
{
	OleOutlineAppPtr	pOleOutlineApp;
	Boolean				fOpened;
	
	pOleOutlineApp = (OleOutlineAppPtr)pApp;
	
	OleAppDoNew(&pOleOutlineApp->m_OleApp);

	ASSERTCOND(pOleOutlineApp->vtbl->m_DoNewProcPtr != nil);
	(*pOleOutlineApp->vtbl->m_DoNewProcPtr)(pApp);
}

//#pragma segment OleOutlineSeg
Boolean OleOutlineAppDoOpen(ApplicationPtr pApp)
{
	volatile OleOutlineAppPtr	pOleOutlineApp;
	Boolean						fOpened;

	pOleOutlineApp = (OleOutlineAppPtr)pApp;

	OleAppEnableDialog(&pOleOutlineApp->m_OleApp);

	TRY
	{	
		ASSERTCOND(pOleOutlineApp->vtbl->m_DoOpenProcPtr != nil);
		fOpened = (*pOleOutlineApp->vtbl->m_DoOpenProcPtr)(pApp);

		OleAppDisableDialog(&pOleOutlineApp->m_OleApp, (Boolean)!fOpened);
	}
	CATCH
	{
		// opening the document failed so we can resume inplace
		OleAppDisableDialog(&pOleOutlineApp->m_OleApp, true);
	}
	ENDTRY

	return fOpened;
}

//#pragma segment OleOutlineSeg
void OleOutlineAppDoAbout(ApplicationPtr pApp)
{
	OleOutlineAppPtr	pOleOutlineApp;

	pOleOutlineApp = (OleOutlineAppPtr)pApp;

	OleAppEnableDialog(&pOleOutlineApp->m_OleApp);

	ASSERTCOND(pOleOutlineApp->vtbl->m_DoAboutProcPtr != nil);
	(*pOleOutlineApp->vtbl->m_DoAboutProcPtr)(pApp);

	OleAppDisableDialog(&pOleOutlineApp->m_OleApp, true);
}

//#pragma segment OleOutlineSeg
/* OleOutlineAppCreateDataXferDoc
 * ---------------------------
 *
 *      Create a document to be use to transfer data (either via a
 *  drag/drop operation or the clipboard). A data transfer document is
 *  the same as a document that is created by the user except that it is
 *  NOT made visible to the user. it is specially used to hold a copy of
 *  data that the user should not be able to change.
 *
 *  OLE2NOTE: in the OLE version the data transfer document is used
 *      specifically to provide an IDataObject* that renders the data copied.
 */
DocumentPtr OleOutlineAppCreateDataXferDoc(OleOutlineAppPtr pOleApp)
{
	volatile DocumentPtr	pDoc;

	pDoc = nil;

	TRY
	{
		pDoc = (DocumentPtr)NewPtrClear(sizeof(OleOutlineDocRec));
		
		ASSERTCOND(pDoc != nil);
		FailNIL(pDoc);

#if qOleContainerApp		
		OleOutlineDocInit((OleOutlineDocPtr)pDoc, kOutline_WIND, kOutlineContainerDocumentFileType, true);
#else
		OleOutlineDocInit((OleOutlineDocPtr)pDoc, kOutline_WIND, kOutlineDocumentFileType, true);
#endif

	}
	CATCH
	{
		if (pDoc != nil)
		{
			ASSERTCOND(pDoc->vtbl->m_FreeProcPtr != nil);
			(*pDoc->vtbl->m_FreeProcPtr)(pDoc);
		}
	}
	ENDTRY

	return pDoc;
}


//#pragma segment OleOutlineSeg
void OleOutlineAppDoStartup(ApplicationPtr pApp)
{
	OleOutlineAppPtr	pOleOutlineApp;
	
	pOleOutlineApp = (OleOutlineAppPtr)pApp;
	
	OleOutlineAppAddRef(pOleOutlineApp);

	OleAppDoStartup(&pOleOutlineApp->m_OleApp);
	
	OleOutlineAppRelease(pOleOutlineApp);

	ASSERTCOND(pOleOutlineApp->vtbl->m_DoStartupProcPtr != nil);
	(*pOleOutlineApp->vtbl->m_DoStartupProcPtr)(pApp);
}

//#pragma segment OleOutlineSeg
Boolean OleOutlineAppIsReadyToQuit(ApplicationPtr pApp)
{
	OleOutlineAppPtr	pOleOutlineApp;
	
	pOleOutlineApp = (OleOutlineAppPtr)pApp;

	ASSERTCOND(pOleOutlineApp->m_cRef >= 0);
	
	// if we are still reference count beyond 1, we are not ready to quit
	// because someone other than main holds a reference to our application
	if (pOleOutlineApp->m_cRef > 0)
		return false;

	return true;
}

//#pragma segment OleOutlineSeg
void OleOutlineAppDoQuit(ApplicationPtr pApp, Boolean askUser, YNCResult defaultResult)
{
	OleOutlineAppPtr	pOleOutlineApp;
	
	pOleOutlineApp = (OleOutlineAppPtr)pApp;

	pOleOutlineApp->m_fIsQuitting = true;

	ASSERTCOND(pOleOutlineApp->vtbl->m_DoQuitProcPtr != nil);
	(*pOleOutlineApp->vtbl->m_DoQuitProcPtr)(pApp, askUser, defaultResult);

	// if our parent is ready to quit, then do the approriate Ole quit stuff
	ASSERTCOND(pOleOutlineApp->vtbl->m_IsReadyToQuitProcPtr != nil);
	if ((*pOleOutlineApp->vtbl->m_IsReadyToQuitProcPtr)(pApp))
		OleAppDoQuit(&pOleOutlineApp->m_OleApp, askUser, defaultResult);
	else
		pOleOutlineApp->m_fIsQuitting = false;
}

void OleOutlineAppFlushClipboard(ApplicationPtr pApp)
{
	OleOutlineAppPtr	pOleOutlineApp;
	
	pOleOutlineApp = (OleOutlineAppPtr)pApp;
	OleAppFlushClipboard(&pOleOutlineApp->m_OleApp);
}

//#pragma segment OleOutlineSeg
void OleOutlineAppDoSuspend(ApplicationPtr pApp)
{
	OleOutlineAppPtr	pOleOutlineApp;
	
	pOleOutlineApp = (OleOutlineAppPtr)pApp;

	if (OleAppDoSuspend(&pOleOutlineApp->m_OleApp))
	{
		ASSERTCOND(pOleOutlineApp->vtbl->m_DoSuspendProcPtr != nil);
		(*pOleOutlineApp->vtbl->m_DoSuspendProcPtr)(pApp);
	}
}

//#pragma segment OleOutlineSeg
void OleOutlineAppDoResume(ApplicationPtr pApp)
{
	OleOutlineAppPtr	pOleOutlineApp;
	
	pOleOutlineApp = (OleOutlineAppPtr)pApp;

	if (OleAppDoResume(&pOleOutlineApp->m_OleApp))
	{
		ASSERTCOND(pOleOutlineApp->vtbl->m_DoResumeProcPtr != nil);
		(*pOleOutlineApp->vtbl->m_DoResumeProcPtr)(pApp);
	}
}

//#pragma segment OleOutlineSeg
ResType OleOutlineAppGetClipboardFormat(OleOutlineAppPtr pOleOutlineApp)
{
#if qOleServerApp
		return kOutlineServerType;
#elif qOleContainerApp
		return kOutlineContainerType;
#endif
}

//#pragma segment OleOutlineSeg
void OleOutlineAppGetNextNewFileName(ApplicationPtr pApp, FSSpecPtr pFile)
{
	/* OLE2NOTE: choose a unique name for a Moniker so that
	**    potential clients can link to our new, untitled document.
	**    if links are established (and currently are connected),
	**    then they will be notified that we have been renamed when
	**    this document is saved to a file.
	*/
	StringHandle	hStrBase;
	Str255			fileName;
	unsigned int	docNum = gDocumentSequence;
	LPMONIKER		lpmk;

	hStrBase = GetString(kNewDocumentName_STR);
	ASSERTCOND(hStrBase != nil);
	pStrCopy(fileName, *hStrBase);

//	pStrCat(fileName, "\p ");

	FSMakeFSSpec(0, 0, fileName, pFile);

	OleStdCreateTempFileMoniker(pFile, &docNum, &lpmk);
	
	gDocumentSequence = docNum;
	
	OleStdRelease((LPUNKNOWN)lpmk);
}

// OLDNAME: OleOutlineInterface.c
static IUnknownVtbl			gOleOutlineAppIUnknownVtbl;
static IAppVtbl				gOleOutlineAppIAppVtbl;

//#pragma segment OleOutlineApplicationInterfaceInitSeg
void OleOutlineAppInitInterfaces(void)
{
	// OleOutlineApp::IUnknown method table
	{
		IUnknownVtbl*	p;

		p = &gOleOutlineAppIUnknownVtbl;

		p->QueryInterface = 				IOleOutlineAppUnknownQueryInterface;
		p->AddRef = 						IOleOutlineAppUnknownAddRef;
		p->Release = 						IOleOutlineAppUnknownRelease;

		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}

	{
		IAppVtbl*		p;
		
		p = &gOleOutlineAppIAppVtbl;
		
		p->m_HideProcPtr =					IOleOutlineHide;
		p->m_ShowProcPtr =					IOleOutlineShow;

		p->m_WaitContextSwitchProcPtr =		IOleOutlineWaitContextSwitch;
		
		p->m_ProcessEventProcPtr =			IOleProcessEvent;

		p->m_CreateDocProcPtr =				IOleOutlineCreateDoc;
		p->m_GetCurrentOleDocProcPtr =		IOleGetCurrentOleDoc;
		p->m_FindOleDocProcPtr =			IOleOutlineFindOleDoc;
		
		p->m_GetClipboardFormatProcPtr =	IOleOutlineGetClipboardFormat;
		
		p->m_GetCmdItemProcPtr =			IOleOutlineGetCmdItem;
		p->m_SetCmdItemProcPtr =			IOleOutlineSetCmdItem;

		p->m_IsQuittingProcPtr =			IOleOutlineIsQuitting;
		
#if qOleContainerApp && qOleInPlace
		p->m_InPlaceInsertMenusProcPtr =	IOleInPlaceInsertMenus;
		p->m_InPlaceDoMenuProcPtr =			IOleInPlaceDoMenu;
		p->m_InPlaceGetBorderProcPtr =		IOleInPlaceGetBorder;
		p->m_InPlaceSetBorderSpaceProcPtr =	IOleInPlaceSetBorderSpace;
#endif

		ASSERTCOND(ValidVtbl(p, sizeof(*p)));
	}
	
}

//#pragma segment OleOutlineApplicationInterfaceSeg
void OleOutlineAppIUnknownInit(OutlineAppUnknownImplPtr pOutlineAppUnknownImpl, OleOutlineAppPtr pOleOutlineApp)
{
	pOutlineAppUnknownImpl->lpVtbl				= &gOleOutlineAppIUnknownVtbl;
	pOutlineAppUnknownImpl->lpOleOutlineApp		= pOleOutlineApp;
	pOutlineAppUnknownImpl->cRef				= 0;
}

//#pragma segment OleOutlineApplicationInterfaceSeg
STDMETHODIMP IOleOutlineAppUnknownQueryInterface(LPUNKNOWN lpThis, REFIID riid, void* * lplpvObj)
{
	OleOutlineAppPtr		pOleOutlineApp;

	OleDbgEnterInterface();
	
	pOleOutlineApp = ((OutlineAppUnknownImplPtr)lpThis)->lpOleOutlineApp;

	return OleOutlineAppQueryInterface(pOleOutlineApp, riid, lplpvObj);
}


//#pragma segment OleOutlineApplicationInterfaceSeg
STDMETHODIMP_(unsigned long) IOleOutlineAppUnknownAddRef(LPUNKNOWN lpThis)
{
	OleOutlineAppPtr		pOleOutlineApp;

	OleDbgEnterInterface();

	pOleOutlineApp = ((OutlineAppUnknownImplPtr)lpThis)->lpOleOutlineApp;

	return OleOutlineAppAddRef(pOleOutlineApp);
}


//#pragma segment OleOutlineApplicationInterfaceSeg
STDMETHODIMP_(unsigned long) IOleOutlineAppUnknownRelease (LPUNKNOWN lpThis)
{
	OleOutlineAppPtr		pOleOutlineApp;

	OleDbgEnterInterface();

	pOleOutlineApp = ((OutlineAppUnknownImplPtr)lpThis)->lpOleOutlineApp;

	return OleOutlineAppRelease(pOleOutlineApp);
}

//#pragma segment OleOutlineApplicationInterfaceSeg
void OleOutlineAppIAppInit(OleOutlineAppAppImplPtr lpThis, OleOutlineAppPtr pOleOutlineApp)
{
	lpThis->lpVtbl = 			&gOleOutlineAppIAppVtbl;
	lpThis->lpOleOutlineApp = 	pOleOutlineApp;
}

//#pragma segment OleOutlineApplicationInterfaceSeg
void IOleOutlineHide(AppImplPtr lpThis)
{
	ApplicationPtr	pApp;
	
	pApp = (ApplicationPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;

	ASSERTCOND(pApp->vtbl->m_HideProcPtr != nil);
	(*pApp->vtbl->m_HideProcPtr)(pApp);
}

//#pragma segment OleOutlineApplicationInterfaceSeg
void IOleOutlineShow(AppImplPtr lpThis)
{
	ApplicationPtr	pApp;
	
	pApp = (ApplicationPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;

	ASSERTCOND(pApp->vtbl->m_ShowProcPtr != nil);
	(*pApp->vtbl->m_ShowProcPtr)(pApp);
}

//#pragma segment OleOutlineApplicationInterfaceSeg
void IOleOutlineWaitContextSwitch(AppImplPtr lpThis, Boolean comingForward, Boolean eatEvent)
{
	ApplicationPtr	pApp;
	
	pApp = (ApplicationPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;

	ASSERTCOND(pApp->vtbl->m_WaitContextSwitchProcPtr != nil);
	(*pApp->vtbl->m_WaitContextSwitchProcPtr)(pApp, comingForward, eatEvent);
}

//#pragma segment OleOutlineApplicationInterfaceSeg
OleDocumentPtr IOleOutlineCreateDoc(AppImplPtr lpThis)
{
	ApplicationPtr	pApp;
	DocumentPtr		pDoc;
	
	pApp = (ApplicationPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;

	ASSERTCOND(pApp->vtbl->m_CreateDocProcPtr != nil);
	pDoc = (*pApp->vtbl->m_CreateDocProcPtr)(pApp);
	ASSERTCOND(pDoc != nil);
	
	if (pDoc == nil)
		return nil;
		
	return &((OleOutlineDocPtr)pDoc)->m_OleDoc;
}

//#pragma segment OleOutlineApplicationInterfaceSeg
OleDocumentPtr IOleGetCurrentOleDoc(AppImplPtr lpThis)
{
	ApplicationPtr		pApp;
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pApp = (ApplicationPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;
	
	ASSERTCOND(pApp->vtbl->m_GetCurrentDocProcPtr != nil);
	pOleOutlineDoc = (OleOutlineDocPtr)(*pApp->vtbl->m_GetCurrentDocProcPtr)(pApp);
	
	if (pOleOutlineDoc)
		return &pOleOutlineDoc->m_OleDoc;
		
	return nil;
}

//#pragma segment OleOutlineApplicationInterfaceSeg
OleDocumentPtr IOleOutlineFindOleDoc(AppImplPtr lpThis, WindowPtr whichWindow)
{
	ApplicationPtr		pApp;
	OleOutlineDocPtr	pOleOutlineDoc;
	
	pApp = (ApplicationPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;
	
	ASSERTCOND(pApp->vtbl->m_FindDocProcPtr != nil);
	pOleOutlineDoc = (OleOutlineDocPtr)(*pApp->vtbl->m_FindDocProcPtr)(pApp, whichWindow);
	
	if (pOleOutlineDoc)
		return &pOleOutlineDoc->m_OleDoc;
		
	return nil;
}

//#pragma segment OleOutlineApplicationInterfaceSeg
ResType IOleOutlineGetClipboardFormat(AppImplPtr lpThis)
{
	OleOutlineAppPtr	pOleOutlineApp;
	
	pOleOutlineApp = ((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;

	return OleOutlineAppGetClipboardFormat(pOleOutlineApp);
}

//#pragma segment OleOutlineApplicationInterfaceSeg
void IOleOutlineSetCmdItem(AppImplPtr lpThis, long cmd, StringPtr pCmdString)
{
	ApplicationPtr	pApp;
	
	pApp = (ApplicationPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;
	
	ASSERTCOND(pApp->vtbl->m_SetCmdItemProcPtr != nil);
	(*pApp->vtbl->m_SetCmdItemProcPtr)(pApp, cmd, pCmdString);
}

//#pragma segment OleOutlineApplicationInterfaceSeg
void IOleOutlineGetCmdItem(AppImplPtr lpThis, long cmd, StringPtr pCmdString)
{
	ApplicationPtr	pApp;
	
	pApp = (ApplicationPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;
	
	ASSERTCOND(pApp->vtbl->m_GetCmdItemProcPtr != nil);
	(*pApp->vtbl->m_GetCmdItemProcPtr)(pApp, cmd, pCmdString);
}

//#pragma segment OleOutlineApplicationInterfaceSeg
Boolean IOleOutlineIsQuitting(AppImplPtr lpThis)
{
	OleOutlineAppPtr	pApp;
	
	pApp = (OleOutlineAppPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;
	
	return pApp->m_fIsQuitting;
}

//#pragma segment OleOutlineApplicationInterfaceSeg
void IOleProcessEvent(AppImplPtr lpThis, EventRecord* pEvent)
{
	ApplicationPtr	pApp;
	
	pApp = (ApplicationPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;

	ASSERTCOND(pApp->vtbl->m_ProcessEventProcPtr != nil);
	(*pApp->vtbl->m_ProcessEventProcPtr)(pApp, pEvent);
}

#if qOleContainerApp && qOleInPlace

//#pragma segment OleOutlineApplicationInterfaceSeg
HRESULT IOleInPlaceInsertMenus(AppImplPtr lpThis, OleMBarHandle hOleMBar)
{
	ApplicationPtr	pApp;
	MenuHandle		hMenu;
	
	pApp = (ApplicationPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;

	/* OLE2NOTE: It is a good idea to make sure that a menu is available,
	 *				and it's ID has not been munged.
	 */
	
	// make sure the menus are updated before handing
	// them to the server
	UpdateMenus();

	hMenu = GetMHandle(kFile_MENU);
	ASSERTCOND(hMenu != nil);
	OleAddMBarMenu(hOleMBar, hMenu, 0);
	
#if qDebug
	hMenu = GetMHandle(kDebug_MENU);
	ASSERTCOND(hMenu != nil);
	OleAddMBarMenu(hOleMBar, hMenu, 4);
#endif
	
	hMenu = GetMHandle(kWindows_MENU);
	ASSERTCOND(hMenu != nil);
	OleAddMBarMenu(hOleMBar, hMenu, 4);

	return NOERROR;
}

//#pragma segment OleOutlineApplicationInterfaceSeg
HRESULT IOleInPlaceDoMenu(AppImplPtr lpThis, EventRecord* pEvent, long mResult)
{
	ApplicationPtr	pApp;
	long	cmd;
	
	pApp = (ApplicationPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;
	
	if (mResult != 0)
	{
		ASSERTCOND(pApp->vtbl->m_MenuItemToCmdProcPtr != nil);
		cmd = (*pApp->vtbl->m_MenuItemToCmdProcPtr)(pApp, HiWord(mResult), LoWord(mResult));
		if (cmd != 0)
		{
			// If a close or quit command comes through TranslateAccelerator
			// we need to post an event to ourselves to handle the command.
			// If we handle it immediately we'll end up closing down the
			// active object in the middle of an LRPC conversation and this
			// is bad.
			if (cmd == cmdClose || cmd == cmdQuit)
				PostCmd(cmd);
			else
				DoCmd(cmd);
		}
	}

	return NOERROR;
}

//#pragma segment OleOutlineApplicationInterfaceSeg
void IOleInPlaceGetBorder(AppImplPtr lpThis, Rect * lprectBorder)
{
	ApplicationPtr	pApp;

	pApp = (ApplicationPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;

	ASSERTCOND(pApp->vtbl->m_GetFrameRectProcPtr != nil);
	(*pApp->vtbl->m_GetFrameRectProcPtr)(pApp, lprectBorder);
}

//#pragma segment OleOutlineApplicationInterfaceSeg
HRESULT IOleInPlaceSetBorderSpace(AppImplPtr lpThis, LPCBORDERWIDTHS pBorder)
{
	ApplicationPtr	pApp;

	pApp = (ApplicationPtr)((OleOutlineAppAppImplPtr)lpThis)->lpOleOutlineApp;

	ASSERTCOND(pApp->vtbl->m_SetBorderSpaceProcPtr != nil);
	(*pApp->vtbl->m_SetBorderSpaceProcPtr)(pApp, (Rect*)pBorder, true);

	return NOERROR;
}

#endif  // qOleInPlace


// OLDNAME: OleClassFactoryInterface.c

static IClassFactoryVtbl		gClassFactoryVtbl;

//#pragma segment OleClassFactoryInitSeg
void OleAppClassFactoryInitInterfaces()
{
	// OleApp::ClassFactory method table
	{
		IClassFactoryVtbl*	p;

		p = &gClassFactoryVtbl;

		p->QueryInterface		= OleAppIntClassFactoryQueryInterface;
		p->AddRef				= OleAppIntClassFactoryAddRef;
		p->Release				= OleAppIntClassFactoryRelease;
		p->CreateInstance		= OleAppIntClassFactoryCreateInstance;
		p->LockServer			= OleAppIntClassFactoryLockServer;
		
		ASSERTCOND(ValidVtbl(((char *)p) + sizeof(p->b), sizeof(*p) - sizeof(p->b)));
	}
}

//#pragma segment OleClassFactorySeg
OleAppclassFactoryPtr OleAppClassFactoryCreate(OleApplicationPtr pOleApp)
{
	OleAppclassFactoryPtr	pAppClassFactory;
	LPMALLOC				pMalloc;
	HRESULT					hrErr;

	hrErr = CoGetMalloc(MEMCTX_TASK, &pMalloc);
	ASSERTNOERROR(hrErr);
	FailOleErr(hrErr);

	pAppClassFactory = (OleAppclassFactoryPtr)pMalloc->lpVtbl->Alloc(pMalloc, sizeof(OleAppClassFactory));
	pMalloc->lpVtbl->Release(pMalloc);

	ASSERTCOND(pAppClassFactory != nil);
	if (pAppClassFactory == nil)
		return nil;

	pAppClassFactory->m_lpVtbl	= &gClassFactoryVtbl;
	pAppClassFactory->m_OleApp	= pOleApp;
	pAppClassFactory->m_cRef 	= 1;
	pAppClassFactory->m_cLock	= 0;

	return (OleAppclassFactoryPtr)pAppClassFactory;
}

//#pragma segment OleClassFactorySeg
STDMETHODIMP OleAppIntClassFactoryQueryInterface(LPCLASSFACTORY lpThis, REFIID riid, void* * lplpvObj)
{
	OleAppclassFactoryPtr	pAppClassFactory;
	SCODE					scode;
	
	OleDbgEnterInterface();

	pAppClassFactory = (OleAppclassFactoryPtr)lpThis;

	// Interfaces support: IUnkonw, IClassFactory
	if (IsEqualIID(riid, &IID_IClassFactory) || IsEqualIID(riid, &IID_IUnknown))
	{
		pAppClassFactory->m_cRef++;				// a pointer to this object is returned
		*lplpvObj = lpThis;
		scode = S_OK;
	}
	else		// unsupported interface
	{
		*lplpvObj = NULL;
		scode = E_NOINTERFACE;
	}

	return ResultFromScode(scode);
}

//#pragma segment OleClassFactorySeg
STDMETHODIMP_(unsigned long) OleAppIntClassFactoryAddRef(LPCLASSFACTORY lpThis)
{
	OleAppclassFactoryPtr	pAppClassFactory;

	OleDbgEnterInterface();

	pAppClassFactory = (OleAppclassFactoryPtr)lpThis;

	return ++pAppClassFactory->m_cRef;
}

//#pragma segment OleClassFactorySeg
STDMETHODIMP_(unsigned long) OleAppIntClassFactoryRelease(LPCLASSFACTORY lpThis)
{
	OleAppclassFactoryPtr	pAppClassFactory;

	OleDbgEnterInterface();

	pAppClassFactory = (OleAppclassFactoryPtr)lpThis;

	if (--pAppClassFactory->m_cRef != 0)		// still used by others
		return pAppClassFactory->m_cRef;

	// free the storage
	{
		LPMALLOC	lpMalloc;
		HRESULT		hrErr;

		hrErr = CoGetMalloc(MEMCTX_TASK, (LPMALLOC *)&lpMalloc);
		ASSERTNOERROR(hrErr);

		if (hrErr != NOERROR)
			return 0;

		lpMalloc->lpVtbl->Free(lpMalloc, pAppClassFactory);
		lpMalloc->lpVtbl->Release(lpMalloc);
	}

	return 0;
}

//#pragma segment OleClassFactorySeg
STDMETHODIMP OleAppIntClassFactoryCreateInstance(LPCLASSFACTORY lpThis, LPUNKNOWN lpUnkOuter, REFIID riid, void* * lplpvObj)
{
	OleAppclassFactoryPtr	pAppClassFactory;
	HRESULT					hrErr;
	OleDocumentPtr			pOleDoc;
	OleApplicationPtr		pOleApp;
	
	OleDbgEnterInterface();

	// our object doesn't support aggregation
	if (lpUnkOuter)
		return ResultFromScode(CLASS_E_NOAGGREGATION);
		
	pAppClassFactory = (OleAppclassFactoryPtr)lpThis;

	*lplpvObj = NULL;

	pOleApp = pAppClassFactory->m_OleApp;

	ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_CreateDocProcPtr != nil);
	pOleDoc = (*pOleApp->m_pIApp->lpVtbl->m_CreateDocProcPtr)(pOleApp->m_pIApp);
	ASSERTCOND(pOleDoc != nil);
	if (pOleDoc == nil)
		return ResultFromScode(CLASS_E_NOAGGREGATION);

	OleDocAddRef(pOleDoc);

	hrErr = OleDocQueryInterface(pOleDoc, riid, lplpvObj);
	ASSERTNOERROR(hrErr);

	OleDocRelease(pOleDoc);

	return hrErr;
}

//#pragma segment OleClassFactorySeg

STDMETHODIMP OleAppIntClassFactoryLockServer(LPCLASSFACTORY lpThis, unsigned long fLock)
{
	HRESULT					hrErr;
	OleApplicationPtr		pOleApp;
	
	OleDbgEnterInterface();

	pOleApp = ((OleAppclassFactoryPtr)lpThis)->m_OleApp;
	
	hrErr = OleAppLock(pOleApp, (Boolean)fLock, TRUE /* fLastUnlockReleases */);
	ASSERTNOERROR(hrErr);

	return hrErr;
}
// OLDNAME: OleMessageFilter.c

#if qOleMessageFilter

extern char					gApplicationName[];

//#pragma segment OleMessageFilterSeg
void OleMessageFilterRegister(OleApplicationPtr pOleApp)
{
	HRESULT				hrErr;
	unsigned long		cRef;
	
	ASSERTCOND(pOleApp->m_pIMessageFilter == nil);
	
	pOleApp->m_pIMessageFilter = OleStdMsgFilter_Create(
						gApplicationName,
						OleMessageFilterMessagePending,
						OleOutlineDocUIDialogHook,
						(long)pOleApp);
	ASSERTCOND(pOleApp->m_pIMessageFilter != nil);
	FailNIL(pOleApp->m_pIMessageFilter);
	
	TRY
	{
		LPMESSAGEFILTER		pPreviousMessageFilter;
		
		hrErr = CoRegisterMessageFilter(pOleApp->m_pIMessageFilter, &pPreviousMessageFilter);
		ASSERTNOERROR(hrErr);
		FailOleErr(hrErr);
		ASSERTCOND(pPreviousMessageFilter == nil);
		
		if (pPreviousMessageFilter != nil) {
			cRef = pPreviousMessageFilter->lpVtbl->Release(pPreviousMessageFilter);
			ASSERTCOND(cRef == 0);
		}
		
		OleStdRelease((LPUNKNOWN)pOleApp->m_pIMessageFilter);
	}
	CATCH
	{
		OleStdRelease((LPUNKNOWN)pOleApp->m_pIMessageFilter);
	}
	ENDTRY
}

//#pragma segment OleMessageFilterSeg
void OleMessageFilterRevoke(OleApplicationPtr pOleApp)
{
	HRESULT				hrErr;
	unsigned long		cRef;
	LPMESSAGEFILTER		pPreviousMessageFilter;

	hrErr = CoRegisterMessageFilter(nil, &pPreviousMessageFilter);
	ASSERTNOERROR(hrErr);
	ASSERTCOND(pPreviousMessageFilter == pOleApp->m_pIMessageFilter || pOleApp->m_pIMessageFilter == nil);
	
	if (pPreviousMessageFilter != nil) {
		cRef = OleStdRelease((LPUNKNOWN)pPreviousMessageFilter);
		ASSERTCOND(cRef == 0);
	}
	
	pOleApp->m_pIMessageFilter = nil;
}

//#pragma segment OleMessageFilterSeg
Boolean OleMessageFilterMessagePending(long userData)
{
	OleApplicationPtr	pOleApp;
	EventRecord			theEvent;
	
	pOleApp = (OleApplicationPtr)userData;
	ASSERTCOND(pOleApp != nil);
	
	if (WaitNextEvent(UserInputMask | UrgentEvtMask, &theEvent, 10, NULL))
	{
		switch (theEvent.what)
		{
			case mouseDown:
			case mouseUp:
			case keyUp:
				break;

			default:
				{
					EventRecord		tmpEvent;

					// We might be calling back to process an event in the middle
					// of another event. Since Outline stores the current EventRecord
					// in a member variable we need to save and restore it.
					tmpEvent = gApplication->m_TheEvent;

					ASSERTCOND(pOleApp->m_pIApp->lpVtbl->m_ProcessEventProcPtr != nil);
					(*pOleApp->m_pIApp->lpVtbl->m_ProcessEventProcPtr)(pOleApp->m_pIApp, &theEvent);

					gApplication->m_TheEvent = tmpEvent;

					break;
				}
		}
	}

	return true;
}

void OleMessageFilterSetRejectStatus(OleApplicationPtr pOleApp, Boolean fReject)
{
	OleStdMsgFilter_SetInComingCallStatus(pOleApp->m_pIMessageFilter,
										  (fReject ? SERVERCALL_REJECTED : SERVERCALL_ISHANDLED));
}

Boolean OleMessageFilterGetRejectStatus(OleApplicationPtr pOleApp)
{
	unsigned long dwReturn;

	dwReturn = OleStdMsgFilter_GetInComingCallStatus(pOleApp->m_pIMessageFilter);

	return (dwReturn == SERVERCALL_REJECTED);
}

#endif // qOleMessageFilter



#endif  // qOle

