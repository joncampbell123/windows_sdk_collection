/**********************************************************************/
/*                                                                    */
/*      DATA.C                                                        */
/*                                                                    */
/*      Copyright (c) 1995-1996  Microsoft Corporation                     */
/*                                                                    */
/**********************************************************************/

#include "windows.h"
#include "imm.h"

HANDLE hInst;
HWND hWndMain;
HWND hWndCompStr;
HWND hWndToolBar;
HWND hWndStatus;
HWND hWndCandList;

LOGFONT lf;
HFONT hFont = 0;

int nStatusHeight = 0;
int nToolBarHeight = 0;
BOOL fShowCand = FALSE;
DWORD fdwProperty = 0;

DWORD  dwCompStrLen      = 0;
DWORD  dwCompAttrLen     = 0;
DWORD  dwCompClsLen      = 0;
DWORD  dwCompReadStrLen  = 0;
DWORD  dwCompReadAttrLen = 0;
DWORD  dwCompReadClsLen  = 0;
DWORD  dwResultStrLen      = 0;
DWORD  dwResultClsLen      = 0;
DWORD  dwResultReadStrLen  = 0;
DWORD  dwResultReadClsLen  = 0;

char   szCompStr[512];
BYTE   bCompAttr[512];
DWORD  dwCompCls[128];
char   szCompReadStr[512];
BYTE   bCompReadAttr[512];
DWORD  dwCompReadCls[128];
char   szResultStr[512];
DWORD  dwResultCls[128];
char   szResultReadStr[512];
DWORD  dwResultReadCls[128];
char   szPaintResult[512];
char   szPaintResultRead[512];

LPCANDIDATELIST lpCandList = NULL;

POINT ptImeUIPos;

