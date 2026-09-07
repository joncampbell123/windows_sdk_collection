/*****************************************************************************\
*                                                                             *
*    Application.h                                                            *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __APP_H__
#define __APP_H__

#include "Const.h"
#include <StandardFile.h>

#if qOle
#include "ole2ui.h"
#endif


#if defined(__MWERKS__)
struct ApplicationRec;
struct WinRec;
struct DocumentRec;

#if qOle
struct OleApplicationRec;
struct OleDocumentRec;
struct AppImpl;
#endif

#endif

typedef void (*AppDisposeProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppFreeProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppEventLoopProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDoMouseUpProcPtr)(struct ApplicationRec* pApp);
typedef Boolean (*AppIsFrontWindowProcPtr)(struct ApplicationRec* pApp, struct WinRec* pWin);
typedef void (*AppDoMouseDownProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDoKeyDownProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDoUpdateEventProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDoDiskEventProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDoActivateEventProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDoOSEventProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppHighLevelEventProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppUpdateCurrentDocProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppSetIdleCursorProcPtr)(struct ApplicationRec* pApp, CursPtr pCursor);
typedef void (*AppDoIdleProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppProcessEventProcPtr)(struct ApplicationRec* pApp, EventRecord* pEvent);
typedef void (*AppDoNewProcPtr)(struct ApplicationRec* pApp);
typedef Boolean (*AppDoOpenProcPtr)(struct ApplicationRec* pApp);
typedef Boolean (*AppOpenADocProcPtr)(struct ApplicationRec* pApp, struct FSSpec* spec, OSType type);
typedef void (*AppPositionNewDocProcPtr)(struct ApplicationRec* pApp, struct DocumentRec* pDoc);
typedef void (*AppGetNextNewFileNameProcPtr)(struct ApplicationRec* pApp, FSSpecPtr pFile);
typedef void (*AppDoAboutProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDoSuspendProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDoResumeProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDoMouseInSysWindowProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDoGoAwayProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDoDragProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDoQuitProcPtr)(struct ApplicationRec* pApp, Boolean askUser, YNCResult defaultResult);
typedef long (*AppMenuItemToCmdProcPtr)(struct ApplicationRec* pApp, short menuID, short menuItem);
typedef Boolean (*AppCmdToMenuItemProcPtr)(struct ApplicationRec* pApp, long cmd, short* menuID, short* menuItem);
typedef void (*AppEnableCmdProcPtr)(struct ApplicationRec* pApp, long cmd);
typedef void (*AppCheckCmdProcPtr)(struct ApplicationRec* pApp, long cmd, Boolean fCheck);
typedef void (*AppPreUpdateMenusProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppGetCmdItemProcPtr)(struct ApplicationRec* pApp, long cmd, StringPtr pCmdString);
typedef void (*AppSetCmdItemProcPtr)(struct ApplicationRec* pApp, long cmd, StringPtr pCmdString);
typedef void (*AppDoUnknownMenuKeyProcPtr)(struct ApplicationRec* pApp, EventRecord* pEvent);
typedef void (*AppDoUnknownMenuItemProcPtr)(struct ApplicationRec* pApp, EventRecord* pEvent, short menuID, short menuItem);
typedef void (*AppDoStartupProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDoDefaultStartupActionProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppReadPreferencesProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppWritePreferencesProcPtr)(struct ApplicationRec* pApp);

typedef void (*AppGetFrameRectProcPtr)(struct ApplicationRec* pApp, Rect* prectBorder);
typedef void (*AppGetBorderSpaceProcPtr)(struct ApplicationRec* pApp, Rect* prectBorder);
typedef void (*AppSetBorderSpaceProcPtr)(struct ApplicationRec* pApp, Rect* prectBorder, Boolean fCallDocSetBorder);
typedef void (*AppShiftAllWindowsProcPtr)(struct ApplicationRec* pApp, short hShift, short vShift);

#if qFrameTools
typedef void (*AppCreateFrameToolsProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppDisposeFrameToolsProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppFrameSpaceNeededProcPtr)(struct ApplicationRec* pApp, Rect* prectBounds);
typedef Boolean (*AppIsClickInFrameToolsProcPtr)(struct ApplicationRec* pApp, EventRecord* pEvent);
typedef void (*AppFloatWindowsStrucRgnsProcPtr)(struct ApplicationRec* pApp, RgnHandle rgnFloats);
typedef void (*AppShowFrameToolsProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppHideFrameToolsProcPtr)(struct ApplicationRec* pApp);
typedef Boolean (*AppToolsCoverWindowPocPtr)(struct ApplicationRec* pApp, WindowPtr pWindow, short hPos, short vPos);
#endif

typedef void (*AppHideProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppShowProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppWaitContextSwitchProcPtr)(struct ApplicationRec* pApp, Boolean comingForward, Boolean eatEvent);
typedef struct DocumentRec* (*AppCreateDocProcPtr)(struct ApplicationRec* pApp);
typedef struct WinRec* (*AppFindWinProcPtr)(struct ApplicationRec* pApp, WindowPtr whichWindow);
typedef struct DocumentRec* (*AppFindDocProcPtr)(struct ApplicationRec* pApp, WindowPtr whichWindow);
typedef struct DocumentRec* (*AppDocFromFSSpecProcPtr)(struct ApplicationRec* pApp, FSSpec* pFile);
typedef Boolean (*AppIsReadyToQuitProcPtr)(struct ApplicationRec* pApp);
typedef MenuHandle (*AppGetWindowsMenuHandleProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppHideOthersCmdProcPtr)(struct ApplicationRec* pApp);
typedef WindowPtr (*AppGetFrontDocWindowProcPtr)(struct ApplicationRec* pApp);
typedef struct DocumentRec* (*AppGetCurrentDocProcPtr)(struct ApplicationRec* pApp);
typedef void (*AppGetCurrentEventRecordProcPtr)(struct ApplicationRec* pApp, EventRecord* pEvent);
typedef void (*AppFlushClipboardProcPtr)(struct ApplicationRec* pApp);

struct ApplicationVtblRec {

	AppDisposeProcPtr					m_DisposeProcPtr;
	AppFreeProcPtr						m_FreeProcPtr;

	AppDoStartupProcPtr					m_DoStartupProcPtr;
	AppDoDefaultStartupActionProcPtr	m_DoDefaultStartupActionProcPtr;

	AppReadPreferencesProcPtr			m_ReadPreferencesProcPtr;
	AppWritePreferencesProcPtr			m_WritePreferencesProcPtr;
	
	AppEventLoopProcPtr					m_EventLoopProcPtr;
	AppIsReadyToQuitProcPtr				m_IsReadyToQuitProcPtr;
	AppDoMouseUpProcPtr					m_DoMouseUpProcPtr;
	AppDoMouseDownProcPtr				m_DoMouseDownProcPtr;
	AppIsFrontWindowProcPtr				m_IsFrontWindowProcPtr;
	AppDoKeyDownProcPtr					m_DoKeyDownProcPtr;
	AppDoUpdateEventProcPtr				m_DoUpdateEventProcPtr;
	AppDoDiskEventProcPtr				m_DoDiskEventProcPtr;
	AppDoActivateEventProcPtr			m_DoActivateEventProcPtr;
	AppDoOSEventProcPtr					m_DoOSEventProcPtr;
	AppHighLevelEventProcPtr			m_HighLevelEventProcPtr;
	AppUpdateCurrentDocProcPtr			m_UpdateCurrentDocProcPtr;
	AppSetIdleCursorProcPtr				m_SetIdleCursorProcPtr;
	AppDoIdleProcPtr					m_DoIdleProcPtr;
	AppProcessEventProcPtr				m_ProcessEventProcPtr;

	AppCreateDocProcPtr					m_CreateDocProcPtr;
	AppDoNewProcPtr						m_DoNewProcPtr;
	AppDoOpenProcPtr					m_DoOpenProcPtr;
	AppOpenADocProcPtr					m_OpenADocProcPtr;
	AppPositionNewDocProcPtr			m_PositionNewDocProcPtr;
	AppGetFrontDocWindowProcPtr			m_GetFrontDocWindowProcPtr;
	AppGetCurrentDocProcPtr				m_GetCurrentDocProcPtr;
	AppFindWinProcPtr					m_FindWinProcPtr;
	AppFindDocProcPtr					m_FindDocProcPtr;
	AppDocFromFSSpecProcPtr				m_DocFromFSSpecProcPtr;
	AppGetNextNewFileNameProcPtr		m_GetNextNewFileNameProcPtr;
	AppDoAboutProcPtr					m_DoAboutProcPtr;

	AppMenuItemToCmdProcPtr				m_MenuItemToCmdProcPtr;
	AppCmdToMenuItemProcPtr				m_CmdToMenuItemProcPtr;
	AppGetCmdItemProcPtr				m_GetCmdItemProcPtr;
	AppSetCmdItemProcPtr				m_SetCmdItemProcPtr;
	AppEnableCmdProcPtr					m_EnableCmdProcPtr;
	AppCheckCmdProcPtr					m_CheckCmdProcPtr;
	AppPreUpdateMenusProcPtr			m_PreUpdateMenusProcPtr;
	AppDoUnknownMenuKeyProcPtr			m_DoUnknownMenuKeyProcPtr;
	AppDoUnknownMenuItemProcPtr			m_DoUnknownMenuItemProcPtr;
	AppGetWindowsMenuHandleProcPtr		m_GetWindowsMenuHandleProcPtr;
	AppHideOthersCmdProcPtr				m_HideOthersCmdProcPtr;
	
	AppDoSuspendProcPtr					m_DoSuspendProcPtr;
	AppDoResumeProcPtr					m_DoResumeProcPtr;

	AppDoMouseInSysWindowProcPtr		m_DoMouseInSysWindowProcPtr;
	AppDoGoAwayProcPtr					m_DoGoAwayProcPtr;
	AppDoDragProcPtr					m_DoDragProcPtr;

	AppDoQuitProcPtr					m_DoQuitProcPtr;
	AppFlushClipboardProcPtr			m_FlushClipboardProcPtr;
	
	AppGetFrameRectProcPtr				m_GetFrameRectProcPtr;
	AppGetBorderSpaceProcPtr			m_GetBorderSpaceProcPtr;
	AppSetBorderSpaceProcPtr			m_SetBorderSpaceProcPtr;
	AppShiftAllWindowsProcPtr			m_ShiftAllWindowsProcPtr;

#if qFrameTools
	AppCreateFrameToolsProcPtr			m_CreateFrameToolsProcPtr;
	AppDisposeFrameToolsProcPtr			m_DisposeFrameToolsProcPtr;
	AppFrameSpaceNeededProcPtr			m_FrameSpaceNeededProcPtr;
	AppIsClickInFrameToolsProcPtr		m_IsClickInFrameToolsProcPtr;
	AppFloatWindowsStrucRgnsProcPtr		m_FloatWindowsStrucRgnsProcPtr;
	AppShowFrameToolsProcPtr			m_ShowFrameToolsProcPtr;
	AppHideFrameToolsProcPtr			m_HideFrameToolsProcPtr;
	AppToolsCoverWindowPocPtr			m_ToolsCoverWindowProcPtr;
#endif

	AppHideProcPtr						m_HideProcPtr;
	AppShowProcPtr						m_ShowProcPtr;

	AppWaitContextSwitchProcPtr			m_WaitContextSwitchProcPtr;
	
	AppGetCurrentEventRecordProcPtr		m_GetCurrentEventRecordProcPtr;
};

typedef struct ApplicationVtblRec ApplicationVtblRec, *ApplicationVtblPtr;

ApplicationVtblPtr AppGetVtbl(void);
void AppInitVtbl(void);
void AppDisposeVtbl(void);

struct ApplicationRec {

	ApplicationVtblPtr					vtbl;

	struct GopherRec*					m_GopherUpdateMenus;
	struct GopherRec*					m_GopherDoCmd;
	struct GopherRec*					m_GopherAdjustCursor;

	OSType								m_Creator;

	short								m_QuickdrawVersion;		// 0 = Original, 1 = 8-Bit, 2 = 32-Bit
	Boolean								m_HaveWaitNextEvent;	// true if we have WaitNextEvent trap
	Boolean								m_fIsReadyToQuit;		// set to true when we are ready to quit
	EventRecord							m_TheEvent;				// our event record
	Boolean								m_InBackground;			// true if our app is suspended
	Boolean								m_fNeedsIdle;			// need to run an idle
	long								m_SleepVal;				// sleep in ticks for WaitNextEvent
	RgnHandle							m_MouseRgn;				// mouse moved region (set it in your DoIdle)
	WindowPtr							m_WhichWindow;			// currently active window
	struct DocumentRec*					m_CurDoc;				// currently active document (if any)

	short								m_NumTypes;
	SFTypeList							m_TypeList;
#ifdef __powerc
    FileFilterUPP						m_pFileFilter;
#else
	FileFilterProcPtr					m_pFileFilter;
#endif

	Handle								m_MenuCommands;

	BORDERWIDTHS						m_BorderWidths;

#if qFrameTools
	GrafPtr								m_pRootLayer;
	GrafPtr								m_pDocLayer;
	GrafPtr								m_pFloatLayer;
	struct ToolbarRec*					m_pToolbar;
#endif

	CursPtr								m_pIdleCursor;
};

typedef struct ApplicationRec ApplicationRec, *ApplicationPtr;

void AppInit(ApplicationPtr pApp, OSType creator);
void AppDispose(ApplicationPtr pApp);
void AppFree(ApplicationPtr pApp);

void AppDoStartup(ApplicationPtr pApp);
void AppDoDefaultStartupAction(ApplicationPtr pApp);

void AppReadPreferences(ApplicationPtr pApp);
void AppWritePreferences(ApplicationPtr pApp);

Boolean AppIsFrontWindow(ApplicationPtr pApp, struct WinRec* pWin);
void AppDoMouseDown(ApplicationPtr pApp);
void AppDoMouseUp(ApplicationPtr pApp);
void AppDoKeyDown(ApplicationPtr pApp);
void AppDoUpdateEvent(ApplicationPtr pApp);
void AppDoDiskEvent(ApplicationPtr pApp);
void AppDoActivateEvent(ApplicationPtr pApp);
void AppDoOSEvent(ApplicationPtr pApp);
void AppHighLevelEvent(ApplicationPtr pApp);
void AppDoSuspend(ApplicationPtr pApp);
void AppDoResume(ApplicationPtr pApp);
void AppEventLoop(ApplicationPtr pApp);
Boolean AppIsReadyToQuit(ApplicationPtr pApp);
void AppDoIdle(ApplicationPtr pApp);
void AppUpdateCurrentDoc(ApplicationPtr pApp);
void AppSetIdleCursor(ApplicationPtr pApp, CursPtr pCursor);
void AppProcessEvent(ApplicationPtr pApp, EventRecord* pEvent);
void AppAdjustCursor(ApplicationPtr pApp);

struct DocumentRec* AppCreateDoc(ApplicationPtr pApp);
struct WinRec* AppFindWin(ApplicationPtr pApp, WindowPtr whichWindow);
struct DocumentRec* AppFindDoc(ApplicationPtr pApp, WindowPtr whichWindow);
struct DocumentRec* AppDocFromFSSpec(ApplicationPtr pApp, FSSpec* pFile);
void AppDoNew(ApplicationPtr pApp);
Boolean AppDoOpen(ApplicationPtr pApp);
Boolean AppOpenADoc(ApplicationPtr pApp, struct FSSpec* spec, OSType type);
void AppPositionNewDoc(ApplicationPtr pApp, struct DocumentRec* pDoc);
void AppGetNextNewFileName(ApplicationPtr pApp, FSSpecPtr pFile);
void AppDoAbout(ApplicationPtr pApp);

void AppUpdateMenus(ApplicationPtr pApp);
MenuHandle AppGetWindowsMenuHandle(ApplicationPtr pApp);
void AppHideOthersCmd(ApplicationPtr pApp);
void AppPreUpdateMenus(ApplicationPtr pApp);
long AppMenuItemToCmd(ApplicationPtr pApp, short menuID, short menuItem);
Boolean AppCmdToMenuItem(ApplicationPtr pApp, long cmd, short* menuID, short* menuItem);
void EnableCmd(long cmd);
void AppEnableCmd(ApplicationPtr pApp, long cmd);
void CheckCmd(long cmd, Boolean fCheck);
void AppCheckCmd(ApplicationPtr pApp, long cmd, Boolean fCheck);
void AppDoCmd(ApplicationPtr pApp, long cmd);
void AppDoUnknownMenuKey(ApplicationPtr pApp, EventRecord* pEvent);
void AppDoUnknownMenuItem(ApplicationPtr pApp, EventRecord* pEvent, short menuID, short menuItem);
void AppGetCmdItem(ApplicationPtr pApp, long cmd, StringPtr pCmdString);
void AppSetCmdItem(ApplicationPtr pApp, long cmd, StringPtr pCmdString);

void AppDoMouseInSysWindow(ApplicationPtr pApp);
void AppDoGoAway(ApplicationPtr pApp);
void AppDoDrag(ApplicationPtr pApp);

void AppDoQuit(ApplicationPtr pApp, Boolean askUser, YNCResult defaultResult);

void AppFlushClipboard(ApplicationPtr pApp);

void AppGetFrameRect(ApplicationPtr pApp, Rect* prectFrame);
void AppGetBorderSpace(ApplicationPtr pApp, Rect* prectBorder);
void AppSetBorderSpace(ApplicationPtr pApp, Rect* prectBorder, Boolean fCallDocSetBorder);
void AppShiftAllWindows(ApplicationPtr pApp, short hShift, short vShift);

#if qFrameTools
void AppCreateFrameTools(ApplicationPtr pApp);
void AppDisposeFrameTools(ApplicationPtr pApp);
void AppFrameSpaceNeeded(ApplicationPtr pApp, Rect* prectBounds);
Boolean AppIsClickInFrameTools(ApplicationPtr pApp, EventRecord* pEvent);
void AppFloatWindowsStrucRgns(ApplicationPtr pApp, RgnHandle rgnFloats);
void AppShowFrameTools(ApplicationPtr pApp);
void AppHideFrameTools(ApplicationPtr pApp);
Boolean AppToolsCoverWindow(ApplicationPtr pApp, WindowPtr pWindow, short hPos, short vPos);
#endif

void AppHide(ApplicationPtr pApp);
void AppShow(ApplicationPtr pApp);

void AppWaitContextSwitch(ApplicationPtr pApp, Boolean comingForward, Boolean eatEvent);

WindowPtr AppGetFrontDocWindow(ApplicationPtr pApp);
struct DocumentRec* AppGetCurrentDoc(ApplicationPtr pApp);
void AppGetCurrentEventRecord(ApplicationPtr pApp, EventRecord* pEvent);

#define kLineListStreamName			"LineList"			// Stream name for line list
#define kNameTableStreamName		"NameTable"			// Stream name for name table

#define 	APPMAJORVERSIONNO		3
#define		APPMINORVERSIONNO		4

#if qOleInPlace

#if qOleServerApp
#define 	APPTYPE			"In-Place Server"
#elif qOleContainerApp
#define 	APPTYPE			"In-Place Container"
#endif

#else

#if qOleServerApp
#define 	APPTYPE			"Server"
#elif qOleContainerApp
#define 	APPTYPE			"Container"
#else
#define 	APPTYPE			"Base"
#endif

#endif

typedef ApplicationVtblRec OutlineVtblRec, *OutlineVtblPtr;

OutlineVtblPtr OutlineGetVtbl(void);
void OutlineInitVtbl(void);
void OutlineDisposeVtbl(void);


struct OutlineAppRec {

	ApplicationRec				superClass;

	ApplicationVtblPtr			vtbl;

	struct GopherRec*					m_GopherDoCmd;
	struct GopherRec*					m_GopherUpdateMenus;
};

typedef struct OutlineAppRec OutlineAppRec, *OutlineAppPtr;

void OutlineAppInit(OutlineAppPtr pOutlineApp, OSType creator);
void OutlineAppDispose(ApplicationPtr pApp);

void OutlineAppDoCmd(OutlineAppPtr pOutlineApp, long cmd);
void OutlineAppUpdateMenus(OutlineAppPtr pOutlineApp);
MenuHandle OutlineAppGetWindowsMenuHandle(ApplicationPtr pApp);
void OutlineAppPreUpdateMenus(ApplicationPtr pApp);
struct DocumentRec* OutlineAppCreateDoc(ApplicationPtr pApp);
struct DocumentRec* OutlineAppCreateDataXferDoc(ApplicationPtr pApp);
void OutlineAppDoNew(ApplicationPtr pApp);
void OutlineAppDoAbout(ApplicationPtr pApp);
#ifndef _MSC_VER
pascal Boolean OutlineAppStandardGetFileFilter(ParmBlkPtr p);
#else
Boolean __pascal OutlineAppStandardGetFileFilter(ParmBlkPtr p);
#endif
Boolean OutlineAppOpenADoc(ApplicationPtr pApp, struct FSSpec* spec, OSType type);


void OleOutlineAppInitInterfaces(void);

struct OleOutlineAppAppImpl {
	struct IAppVtbl*			lpVtbl;
	struct OleOutlineAppRec*	lpOleOutlineApp;
};

typedef struct OleOutlineAppAppImpl OleOutlineAppAppImpl, *OleOutlineAppAppImplPtr;

void OleOutlineAppIAppInit(OleOutlineAppAppImplPtr pOleOutlineAppAppImpl, struct OleOutlineAppRec* pOleOutlineApp);

struct OutlineAppUnknownImpl {
	IUnknownVtbl*				lpVtbl;
	struct OleOutlineAppRec*	lpOleOutlineApp;
	unsigned long				cRef;			// interface specific ref count
};

typedef struct OutlineAppUnknownImpl OutlineAppUnknownImpl, *OutlineAppUnknownImplPtr;

void OleOutlineAppIUnknownInit(OutlineAppUnknownImplPtr pOutlineAppUnknownImpl, struct OleOutlineAppRec* pOleOutlineApp);

// OleOutlineApp::IUnknown methods
STDMETHODIMP IOleOutlineAppUnknownQueryInterface(LPUNKNOWN lpThis, REFIID riid, void* * lplpvObj);
STDMETHODIMP_(unsigned long) IOleOutlineAppUnknownAddRef(LPUNKNOWN lpThis);
STDMETHODIMP_(unsigned long) IOleOutlineAppUnknownRelease(LPUNKNOWN lpThis);

void OleOutlineAppIAppInit(OleOutlineAppAppImplPtr lpThis, struct OleOutlineAppRec* pOleOutlineApp);

#if defined(__PPCC__)
struct AppImpl;
#endif

void IOleOutlineHide(struct AppImpl* lpThis);
void IOleOutlineShow(struct AppImpl* lpThis);
void IOleOutlineWaitContextSwitch(struct AppImpl* lpThis, Boolean comingForward, Boolean eatEvent);
void IOleProcessEvent(struct AppImpl* lpThis, EventRecord* pEvent);
struct OleDocumentRec* IOleOutlineCreateDoc(struct AppImpl* lpThis);
struct OleDocumentRec* IOleGetCurrentOleDoc(struct AppImpl* lpThis);
struct OleDocumentRec* IOleOutlineFindOleDoc(struct AppImpl* lpThis, WindowPtr whichWindow);
ResType IOleOutlineGetClipboardFormat(struct AppImpl* lpThis);
void IOleOutlineGetCmdItem(struct AppImpl* lpThis, long cmd, StringPtr pCmdString);
void IOleOutlineSetCmdItem(struct AppImpl* lpThis, long cmd, StringPtr pCmdString);
Boolean IOleOutlineIsQuitting(struct AppImpl* lpThis);

#if qOleContainerApp && qOleInPlace
HRESULT IOleInPlaceInsertMenus(struct AppImpl* lpThis, OleMBarHandle hOleMBar);
HRESULT IOleInPlaceDoMenu(struct AppImpl* lpThis, EventRecord* pEvent, long mResult);
void IOleInPlaceGetBorder(struct AppImpl* lpThis, Rect * lprectBorder);
HRESULT IOleInPlaceSetBorderSpace(struct AppImpl* lpThis, LPCBORDERWIDTHS pBorder);
#endif


#if qOle

#if qOleServerApp
	#include "appsvr.h"
#elif qOleContainerApp
	#include "appcntr.h"
#endif

#define kMaxNumOfMTS		10
#define kMaxNumOfLinkTypes	3

struct OleApplicationRec {

	IUnknown*						m_pIUnknown;
	struct AppImpl*					m_pIApp;

	struct GopherRec*				m_GopherFilterCmd;

	Boolean							m_fUserInControl;				// does user control life-time of app?

	Boolean							m_fOleInitialized;				// was OLE initialized
	long							m_cDoc;							// document count, for debugging

	struct OleDocumentRec*			m_pClipboardDoc;				// document put in clipboard
	struct OleDocumentRec*			m_pDragSourceDoc;				// dragdrop source document
	
#if qOleClassFactory
	struct OleAppClassFactory*		m_lpClassFactory;				// ptr to allocated ClassFactory instance
	unsigned long					m_dwRegisteredClassFactory;		// value returned by CoRegusterClassObject
#endif

#if qOleMessageFilter
	LPMESSAGEFILTER					m_pIMessageFilter;
#endif

	ResType							m_cfEmbedSource;				// OLE 2.0 clipboard format
	ResType							m_cfEmbeddedObject;
	ResType							m_cfLinkSource;
	ResType							m_cfObjectDescriptor;
	ResType							m_cfLinkSrcDescriptor;

	FORMATETC						m_arrDocGetFmts[kMaxNumOfMTS];	// fmts offered by copy & GetData
	short							m_nDocGetFmts;					// no of fmtetc's for GetData

	OLEUIPASTEENTRY					m_arrPasteEntries[kMaxNumOfMTS];
	short							m_nPasteEntries;

	unsigned int					m_arrLinkTypes[kMaxNumOfLinkTypes];
    short							m_nLinkTypes;

#if qOleServerApp
	OleServerAppRec					server;
#elif qOleContainerApp
    OleContainerAppRec				container;
#endif // qOleContainerApp
};

typedef struct OleApplicationRec OleApplicationRec, *OleApplicationPtr;

void OleAppInit(OleApplicationPtr pOleApp, struct AppImpl* pIApp, IUnknown* pIUnknown);
void OleAppDispose(OleApplicationPtr pOleApp);

void OleAppDoQuit(OleApplicationPtr pOleApp, Boolean askUser, YNCResult defaultResult);

void OleAppDoStartup(OleApplicationPtr pOleApp);

void OleAppFilterCmd(OleApplicationPtr pOleApp, long cmd);

void OleAppDoNew(OleApplicationPtr pOleApp);

Boolean OleAppDoSuspend(OleApplicationPtr pOleApp);
Boolean OleAppDoResume(OleApplicationPtr pOleApp);

void OleAppRegisterClassFactory(OleApplicationPtr pOleApp);
void OleAppRevokeClassFactory(OleApplicationPtr pOleApp);

unsigned long OleAppAddRef(OleApplicationPtr pOleApp);
unsigned long OleAppRelease(OleApplicationPtr pOleApp);
HRESULT OleAppLocalQueryInterface(OleApplicationPtr pOleApp, REFIID riid, void* * lplpvObj);	// query only OleApp for interfaces
HRESULT OleAppQueryInterface(OleApplicationPtr pOleApp, REFIID riid, void* * lplpvObj);

HRESULT OleAppLock(OleApplicationPtr pOleApp, Boolean fLock, Boolean fLastUnlockReleases);

void OleAppDocLock(OleApplicationPtr pOleApp);
void OleAppDocUnlock(OleApplicationPtr pOleApp);

void OleAppAddDocGetFormat(OleApplicationPtr pOleApp, LPFORMATETC pFormatetc);
void OleAppAddPasteEntry(OleApplicationPtr pOleApp, LPOLEUIPASTEENTRY pPasteEntry);
void OleAppAddLinkType(OleApplicationPtr pOleApp, unsigned int LinkType);
void OleAppFlushClipboard(OleApplicationPtr pOleApp);

void OleAppEnableDialog(OleApplicationPtr pOleApp);
void OleAppDisableDialog(OleApplicationPtr pOleApp, Boolean resumeInPlace);

#ifndef _MSC_VER
pascal long MyGrowZoneProc(Size cbNeeded);
#else
long __pascal MyGrowZoneProc(Size cbNeeded);
#endif

typedef struct OleDocumentRec* (*OleAppCreateDocProcPtr)(struct AppImpl* pIApp);
typedef struct OleDocumentRec* (*OleAppGetCurrentOleDocProcPtr)(struct AppImpl* pIApp);
typedef struct OleDocumentRec* (*OleAppFindOleDocProcPtr)(struct AppImpl* pIApp, WindowPtr whichWindow);
typedef void (*OleAppHideProcPtr)(struct AppImpl* pIApp);
typedef void (*OleAppShowProcPtr)(struct AppImpl* pIApp);
typedef void (*OleAppWaitContextSwitchProcPtr)(struct AppImpl* pIApp, Boolean comingForward, Boolean eatEvent);
typedef ResType (*OleAppGetClipboardFormatProcPtr)(struct AppImpl* pIApp);
typedef void (*OleAppGetCmdItemProcPtr)(struct AppImpl* pIApp, long cmd, StringPtr pCmdString);
typedef void (*OleAppSetCmdItemProcPtr)(struct AppImpl* pIApp, long cmd, StringPtr pCmdString);
typedef void (*OleAppProcessEventProcPtr)(struct AppImpl* pIApp, EventRecord* pEvent);
typedef Boolean (*OleAppIsQuittingProcPtr)(struct AppImpl* pIApp);

#if qOleContainerApp && qOleInPlace
typedef HRESULT (*OleAppInPlaceInsertMenusProcPtr)(struct AppImpl* pIApp, OleMBarHandle hOleMBar);
typedef HRESULT (*OleAppInPlaceDoMenuProcPtr)(struct AppImpl* pIApp, EventRecord* pEvent, long mResult);
typedef void (*OleAppInPlaceGetBorderProcPtr)(struct AppImpl* pIApp, Rect * lprectBorder);
typedef HRESULT (*OleAppInPlaceSetBorderSpaceProcPtr)(struct AppImpl* pIApp, LPCBORDERWIDTHS pBorder);
#endif

struct IAppVtbl {
		
	OleAppHideProcPtr				m_HideProcPtr;
	OleAppShowProcPtr				m_ShowProcPtr;

	OleAppWaitContextSwitchProcPtr	m_WaitContextSwitchProcPtr;

	OleAppProcessEventProcPtr		m_ProcessEventProcPtr;

	OleAppCreateDocProcPtr			m_CreateDocProcPtr;
	OleAppGetCurrentOleDocProcPtr	m_GetCurrentOleDocProcPtr;
	OleAppFindOleDocProcPtr			m_FindOleDocProcPtr;
	
	OleAppGetClipboardFormatProcPtr	m_GetClipboardFormatProcPtr;

	OleAppGetCmdItemProcPtr			m_GetCmdItemProcPtr;
	OleAppSetCmdItemProcPtr			m_SetCmdItemProcPtr;

	OleAppIsQuittingProcPtr			m_IsQuittingProcPtr;
	
#if qOleContainerApp && qOleInPlace
	OleAppInPlaceInsertMenusProcPtr	m_InPlaceInsertMenusProcPtr;
	OleAppInPlaceDoMenuProcPtr		m_InPlaceDoMenuProcPtr;
	OleAppInPlaceGetBorderProcPtr	m_InPlaceGetBorderProcPtr;
	OleAppInPlaceSetBorderSpaceProcPtr m_InPlaceSetBorderSpaceProcPtr;
#endif
};

typedef struct IAppVtbl IAppVtbl;

struct AppImpl {
	IAppVtbl*					lpVtbl;
};
       
typedef struct AppImpl AppImpl, *AppImplPtr;

//#endif



#if qOleContainerApp
#define 	CONTAINERDOCFORMAT		"CntrOutl"
#endif

#define		OUTLINEDOCFORMAT		"Outline"

typedef OutlineVtblRec OleOutlineVtblRec, *OleOutlineVtblPtr;

OleOutlineVtblPtr OleOutlineGetVtbl(void);
void OleOutlineInitVtbl(void);
void OleOutlineDisposeVtbl(void);

struct OleOutlineAppRec {

	OutlineAppRec				superClass;

	ApplicationVtblPtr			vtbl;

	unsigned long				m_cDoc;						// total count for documents
	unsigned long				m_cRef;						// total ref count for application

	Boolean						m_fIsQuitting;				// set to true while quitting

	struct GopherRec*			m_GopherDoCmd;
	struct GopherRec*			m_GopherUpdateMenus;

	OutlineAppUnknownImpl		m_IUnknown;
	OleOutlineAppAppImpl		m_IApp;
	
	OleApplicationRec			m_OleApp;
};

typedef struct OleOutlineAppRec OleOutlineAppRec, *OleOutlineAppPtr;

void OleOutlineAppInit(OleOutlineAppPtr pOleOutlineApp, OSType creator);
void OleOutlineAppDispose(ApplicationPtr pApp);
void OleOutlineAppFree(ApplicationPtr pApp);

void OleOutlineAppDoCmd(OleOutlineAppPtr pOleOutlineApp, long cmd);
void OleOutlineAppUpdateMenus(OleOutlineAppPtr pOleOutlineApp);

unsigned long OleOutlineAppAddRef(OleOutlineAppPtr pOleOutlineApp);
unsigned long OleOutlineAppRelease(OleOutlineAppPtr pOleOutlineApp);
HRESULT OleOutlineAppQueryInterface(OleOutlineAppPtr pOleOutlineApp, REFIID riid, void* * lplpvObj);

void OleOutlineAppDocLock(OleOutlineAppPtr pOleOutlineApp);
void OleOutlineAppDocUnlock(OleOutlineAppPtr pOleOutlineApp);

void OleOutlineAppVersionNoCheck(OleOutlineAppPtr pOleOutlineApp, char* pszFormatName, short narrAppVersionNo[]);
void OleOutlineAppGetAppVersionNo(OleOutlineAppPtr pOleOutlineApp, short narrAppVersionNo[]);

struct DocumentRec* OleOutlineAppCreateDoc(ApplicationPtr pApp);
struct DocumentRec* OleOutlineAppCreateDataXferDoc(OleOutlineAppPtr pOleApp);
void OleOutlineAppDoNew(ApplicationPtr pApp);
Boolean OleOutlineAppDoOpen(ApplicationPtr pApp);
void OleOutlineAppDoAbout(ApplicationPtr pApp);

void OleOutlineAppDoStartup(ApplicationPtr pApp);

void OleOutlineAppDoSuspend(ApplicationPtr pApp);
void OleOutlineAppDoResume(ApplicationPtr pApp);

Boolean OleOutlineAppIsReadyToQuit(ApplicationPtr pApp);
void OleOutlineAppDoQuit(ApplicationPtr pApp, Boolean askUser, YNCResult defaultResult);
void OleOutlineAppFlushClipboard(ApplicationPtr pApp);
ResType OleOutlineAppGetClipboardFormat(OleOutlineAppPtr pOleOutlineApp);
void OleOutlineAppGetNextNewFileName(ApplicationPtr pApp, FSSpecPtr pFile);



void OleMessageFilterRegister(struct OleApplicationRec* pOleApp);
void OleMessageFilterRevoke(struct OleApplicationRec* pOleApp);

Boolean OleMessageFilterMessagePending(long userData);

// OLDNAME: OleClassFactoryInterface.h
void OleAppClassFactoryInitInterfaces(void);

struct OleAppClassFactory {
	IClassFactoryVtbl*			m_lpVtbl;
	struct OleApplicationRec*	m_OleApp;
	unsigned long						m_cRef;
	unsigned long						m_cLock;
};

typedef struct OleAppClassFactory OleAppClassFactory, *OleAppclassFactoryPtr;

OleAppclassFactoryPtr OleAppClassFactoryCreate(struct OleApplicationRec* pOleApp);

STDMETHODIMP OleAppIntClassFactoryQueryInterface(LPCLASSFACTORY lpThis, REFIID riid, void* * ppvObj);
STDMETHODIMP_(unsigned long) OleAppIntClassFactoryAddRef(LPCLASSFACTORY lpThis);
STDMETHODIMP_(unsigned long) OleAppIntClassFactoryRelease(LPCLASSFACTORY lpThis);
STDMETHODIMP OleAppIntClassFactoryCreateInstance(LPCLASSFACTORY lpThis, LPUNKNOWN lpUnkOuter, REFIID riid, void* * lplpvObj);
STDMETHODIMP OleAppIntClassFactoryLockServer(LPCLASSFACTORY lpThis, unsigned long fLock);

// OLDNAME: OleMessageFilter.h
#if qOleMessageFilter

void OleMessageFilterRegister(struct OleApplicationRec* pOleApp);
void OleMessageFilterRevoke(struct OleApplicationRec* pOleApp);

Boolean OleMessageFilterMessagePending(long userData);

void OleMessageFilterSetRejectStatus(struct OleApplicationRec* pOleApp, Boolean fReject);
Boolean OleMessageFilterGetRejectStatus(struct OleApplicationRec* pOleApp);

#endif // qOleMessageFilter

#endif // qOle

#endif
