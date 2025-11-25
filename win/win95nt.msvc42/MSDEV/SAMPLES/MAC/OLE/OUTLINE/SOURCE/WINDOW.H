#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "App.h"
#include "Const.h"

#define kMinimumWindowHeight		50
#define kMinimumWindowWidth			100

#define WinVtblDef						\
	kDisposeProcPtr;					\
	kFreeProcPtr;						\
	kDoActivateProcPtr;					\
	kDoUpdateProcPtr;					\
	kDoGrowProcPtr;						\
	kDoGrowWindowProcPtr;				\
	kDoZoomProcPtr;						\
	kDoZoomWindowProcPtr;				\
	kGetDragBoundsProcPtr;				\
	kDoDragProcPtr;						\
	kMoveWindowProcPtr;					\
	kShiftWindowProcPtr;				\
	kDoContentProcPtr;					\
	kSetWindowTitleProcPtr;				\
	kGetWindowProcPtr;					\
	kShowProcPtr;						\
	kHideProcPtr;						\
	kIsVisibleProcPtr;					\
	kBringToFrontProcPtr;				\
	kDoIdleProcPtr;

#define kDisposeProcPtr					void (*m_DisposeProcPtr)(pThis)
#define kFreeProcPtr					void (*m_FreeProcPtr)(pThis)
#define kDoActivateProcPtr				void (*m_DoActivateProcPtr)(pThis, Boolean becomingActive)
#define kDoUpdateProcPtr				void (*m_DoUpdateProcPtr)(pThis)
#define kDoGrowProcPtr					long (*m_DoGrowProcPtr)(pThis, EventRecord* theEvent)
#define kDoGrowWindowProcPtr			long (*m_DoGrowWindowProcPtr)(pThis, EventRecord* theEvent)
#define kDoZoomProcPtr					void (*m_DoZoomProcPtr)(pThis, short partCode)
#define kDoZoomWindowProcPtr			void (*m_DoZoomWindowProcPtr)(pThis, short partCode)
#define kGetDragBoundsProcPtr			void (*m_GetDragBoundsProcPtr)(pThis, Rect* rBounds)
#define kDoDragProcPtr					void (*m_DoDragProcPtr)(pThis, EventRecord* pEvent, Boolean front)
#define kMoveWindowProcPtr				void (*m_MoveWindowProcPtr)(pThis, short h, short v, Boolean front)
#define kShiftWindowProcPtr				void (*m_ShiftWindowProcPtr)(pThis, short hShift, short vShift)
#define kDoContentProcPtr				void (*m_DoContentProcPtr)(pThis, EventRecord* theEvent)
#define kSetWindowTitleProcPtr			void (*m_SetWindowTitleProcPtr)(pThis, StringPtr pTitle)
#define kGetWindowProcPtr				WindowPtr (*m_GetWindowProcPtr)(pThis)
#define kShowProcPtr					void (*m_ShowProcPtr)(pThis)
#define kHideProcPtr					void (*m_HideProcPtr)(pThis)
#define kIsVisibleProcPtr				Boolean (*m_IsVisibleProcPtr)(pThis)
#define kBringToFrontProcPtr			void (*m_BringToFrontProcPtr)(pThis)
#define kDoIdleProcPtr					void (*m_DoIdleProcPtr)(pThis)

#undef pThis
#define pThis struct WinRec* pWin

struct WinVtblRec {
	WinVtblDef
};

typedef struct WinVtblRec WinVtblRec, *WinVtblPtr;

WinVtblPtr WinGetVtbl(void);
void WinInitVtbl(void);
void WinDisposeVtbl(void);

#define WinDataDef													\
	WindowPtr							m_WindowPtr;				\
	Rect								m_GrowSizeRect;				\
	Boolean								m_fFloating;

struct WinRec {
	WinVtblPtr							vtbl;
	WinDataDef
};

typedef struct WinRec WinRec, *WinPtr;

void WinInit(WinPtr pWin, short resID, Boolean floating);
void WinDispose(WinPtr pWin);
void WinFree(WinPtr pWin);
void WinDoActivate(WinPtr pWin, Boolean becomingActive);
void WinDoUpdate(WinPtr pWin);
long WinDoGrow(WinPtr pWin, EventRecord* theEvent);
long WinDoGrowWindow(WinPtr pWin, EventRecord* theEvent);
void WinDoZoom(WinPtr pWin, short partCode);
void WinDoZoomWindow(WinPtr pWin, short partCode);
void WinGetDragBounds(WinPtr pWin, Rect* rBounds);
void WinDoDrag(WinPtr pWin, EventRecord* theEvent, Boolean front);
void WinMoveWindow(WinPtr pWin, short h, short v, Boolean front);
void WinShiftWindow(WinPtr pWin, short hShift, short vShift);
void WinDoContent(WinPtr pWin, EventRecord* theEvent);
void WinSetWindowTitle(WinPtr pWin, StringPtr pTitle);
WindowPtr WinGetWindow(WinPtr pWin);
void WinShow(WinPtr pWin);
void WinHide(WinPtr pWin);
Boolean WinIsVisible(WinPtr pWin);
void WinBringToFront(WinPtr pWin);
void WinDoIdle(WinPtr pWin);

#endif
