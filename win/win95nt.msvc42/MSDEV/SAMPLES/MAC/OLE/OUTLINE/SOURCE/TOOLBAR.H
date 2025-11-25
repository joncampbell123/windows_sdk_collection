#ifndef __TOOLBAR_H__
#define __TOOLBAR_H__

#include "Window.h"

typedef enum {
	TOOLBAR_TOP      = 0,
	TOOLBAR_LEFT     = 1,
	TOOLBAR_BOTTOM   = 2,
	TOOLBAR_RIGHT    = 3,
	TOOLBAR_FLOATING = 4,
	TOOLBAR_DISABLED = 5
} TOOLBAR_STATE;

#define ToolbarVtblDef					\
	kEnableProcPtr;						\
	kGetStateProcPtr;					\
	kSetStateProcPtr;					\
	kSpaceNeededProcPtr;

#define kEnableProcPtr					void (*m_EnableProcPtr)(pThis, Boolean fEnable)
#define kGetStateProcPtr				TOOLBAR_STATE (*m_GetStateProcPtr)(pThis)
#define kSetStateProcPtr				void (*m_SetStateProcPtr)(pThis, TOOLBAR_STATE state)
#define kSpaceNeededProcPtr				void (*m_SpaceNeededProcPtr)(pThis, Rect* prectSpace)

#undef pThis
#define pThis struct ToolbarRec* pToolbar

struct ToolbarVtblRec {
	WinVtblDef
	ToolbarVtblDef
};

typedef struct ToolbarVtblRec ToolbarVtblRec, *ToolbarVtblPtr;

ToolbarVtblPtr ToolbarGetVtbl(void);
void ToolbarInitVtbl(void);
void ToolbarDisposeVtbl(void);

#define ToolbarDataDefInternal										\
	PicHandle							m_hToolsPict;				\
	Point								m_ptPosition;				\
	Boolean								m_fEnabled;					\
	Boolean								m_fVisible;					\
	BORDERWIDTHS						m_BorderWidths;				\
	TOOLBAR_STATE						m_State;

#define ToolbarDataDefAndBase										\
	WinVtblPtr							baseVtbl;					\
	ToolbarDataDefInternal

#define ToolbarDataDef												\
	unsigned long						m_ToolbarFiller;			\
	ToolbarDataDefInternal

struct ToolbarRec {
	ToolbarVtblPtr						vtbl;
	WinDataDef
	ToolbarDataDefAndBase
};

typedef struct ToolbarRec ToolbarRec, *ToolbarPtr;

void ToolbarInit(ToolbarPtr pToolbar, short resID);
void ToolbarDispose(ToolbarPtr pToolbar);

void ToolbarDoUpdate(ToolbarPtr pToolbar);
void ToolbarShow(ToolbarPtr pToolbar);
void ToolbarHide(ToolbarPtr pToolbar);
void ToolbarMoveWindow(ToolbarPtr pToolbar, short h, short v, Boolean front);

void ToolbarEnable(ToolbarPtr pToolbar, Boolean fEnable);
TOOLBAR_STATE ToolbarGetState(ToolbarPtr pToolbar);
void ToolbarSetState(ToolbarPtr pToolbar, TOOLBAR_STATE state);
void ToolbarSpaceNeeded(ToolbarPtr pToolbar, Rect* prectSpace);

#endif
