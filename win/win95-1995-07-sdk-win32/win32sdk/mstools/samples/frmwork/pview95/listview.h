// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
// PURPOSE:
//    Contains declarations for list view creation and manipulation functions.


//-------------------------------------------------------------------------
// Manifest constants

#define ID_LISTVIEW     201
#define CPROCESSCOLUMNS 6
#define CTHREADCOLUMNS  3
#define C_COLUMNS       3

#define EXETYPE_32BIT   32      // Executable type either 16 or 32 bits
#define EXETYPE_16BIT   16


//-------------------------------------------------------------------------
// Type definitions

typedef struct tagPINFO
{
    char  szModName[256];
    DWORD pid;
    DWORD dwPriorityClass;   // Stores priority class of process
    LONG  pcPriClassBase;    // Stores base priority of process
    DWORD cntThreads;
    UINT  uAppType; 
    char  szFullPath[_MAX_PATH];
} PINFO, *PPINFO;

typedef struct tagTINFO
{
    DWORD tid;
    DWORD pidOwner;
    LONG  tpBasePri;
    LONG  tpDeltaPri;
} TINFO, *PTINFO;


typedef struct tagSORTINFO 
{
    int idFrom;
    int iSubItem;
} SORTINFO, *PSORTINFO;


typedef struct tagAPPCOLUMNDEF
{
    WORD idString;
    WORD cxColumn;
} APPCOLUMNDEF;


//-------------------------------------------------------------------------
// Function prototypes.

HWND CreateListView(HWND, DWORD dwStyle, APPCOLUMNDEF acd[], UINT, UINT);
BOOL WINAPI ListView_InitColumns(HWND, APPCOLUMNDEF acd[], UINT);
BOOL AddProcessItem(HWND, PINFO);
BOOL AddThreadItem(HWND, TINFO);
LRESULT ListView_OnGetDispInfo(int idFrom, LV_DISPINFO *plvdi);
LRESULT ListView_OnClick(int idFrom, NM_LISTVIEW *pnmlv);
LRESULT ListView_OnDeleteItem(int idFrom, NM_LISTVIEW *pnmlv);
LRESULT ListView_OnColumnClick(int idFrom, NM_LISTVIEW *pnmlv);
int CALLBACK ListView_CompareProc(LPARAM lParam1, LPARAM lParam2, 
                                  LPARAM lParamSort);
DWORD ListView_GetSelectedPid(HWND hwndLV, LPSTR lpszPath, int cch);
LRESULT CALLBACK LV_Subclass(HWND, UINT, WPARAM, LPARAM);

extern APPCOLUMNDEF acdProcess[];
extern APPCOLUMNDEF acdThread[];
extern FARPROC g_lpfnThreadListView;
