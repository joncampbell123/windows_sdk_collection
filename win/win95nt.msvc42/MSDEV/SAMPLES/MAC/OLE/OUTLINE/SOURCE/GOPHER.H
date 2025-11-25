/*****************************************************************************\
*                                                                             *
*    Gopher.h                                                                 *
*                                                                             *
*    OLE Version 2.0 Sample Code                                              *
*                                                                             *
*    Copyright (c) 1992-1994, Microsoft Corp. All rights reserved.            *
*                                                                             *
\*****************************************************************************/

#ifndef __GOPHER_H__
#define __GOPHER_H__

#ifdef __PPCC__
#include "Types.h"
#endif

extern struct GopherRec*	gGopher;

#define kUpdateMenusType	'MENU'
#define kFilterCmdType		'FILT'
#define kDoCmdType			'CMD '
#define kAdjustCusrorType	'CURS'

typedef void (*UpdateMenusProcPtr)(void* p);
typedef void (*FilterCmdProcPtr)(void* p, long cmd);
typedef void (*DoCmdProcPtr)(void* p, long cmd);
typedef void (*AdjustCursorProcPtr)(void *p);

typedef OSType GopherType;

struct GopherRec {

	GopherType			m_Type;

	struct GopherRec*	m_NextGopher;
	struct GopherRec*	m_PreviousGopher;

	void*				m_UserData;

	union {
		UpdateMenusProcPtr		m_UpdateMenusProcPtr;
		FilterCmdProcPtr		m_FilterCmdProcPtr;
		DoCmdProcPtr			m_DoCmdProcPtr;
		AdjustCursorProcPtr		m_AdjustCursorProcPtr;
	} m_Proc;
};

typedef struct GopherRec GopherRec, *GopherPtr;

void UpdateMenus(void);
void DoCmd(long cmd);
void PostCmd(long cmd);
#define AdjustCursor()				GopherAdjustCursor(gGopher)

#define InheritUpdateMenus(x)		GopherUpdateMenus((x)->m_NextGopher)
#define InheritFilterCmd(x, cmd)	GopherFilterCmd((x)->m_NextGopher, (cmd))
#define InheritDoCmd(x, cmd)		GopherDoCmd((x)->m_NextGopher, (cmd))
#define InheritAdjustCursor(x)		GopherAdjustCursor((x)->m_NextGopher)

void GopherUpdateMenus(GopherPtr theGopher);
void GopherFilterCmd(GopherPtr theGopher, long cmd);
void GopherDoCmd(GopherPtr theGopher, long cmd);
void GopherAdjustCursor(GopherPtr theGopher);

GopherPtr GopherNewUpdateMenus(UpdateMenusProcPtr pUpdateMenusProcPtr, void* pUserData);
GopherPtr GopherNewFilterCmd(DoCmdProcPtr pDoCmdProcPtr, void* pUserData);
GopherPtr GopherNewDoCmd(DoCmdProcPtr pDoCmdProcPtr, void* pUserData);
GopherPtr GopherNewAdjustCursor(AdjustCursorProcPtr pAdjustCursorProcPtr, void* pUserData);
void GopherDispose(GopherPtr theGopher);

void GopherAdd(GopherPtr theGopher);
void GopherRemove(GopherPtr theGopher);

#endif
