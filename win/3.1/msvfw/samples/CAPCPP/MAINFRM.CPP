/****************************************************************************
 *
 *   captevw.cpp: implementation of the CMainFrame class
 * 
 *   Microsoft Video for Windows Capture Class Sample Program
 *
 ***************************************************************************/
/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
 * 
 **************************************************************************/

#include "stdafx.h"
#include "windows.h"
#include "windowsx.h"
#include "mmsystem.h"

#include <vfw.h>

#include "captest.h"
#include "mainfrm.h"
#include "captedoc.h"
#include "captevw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
         //{{AFX_MSG_MAP(CMainFrame)
         ON_WM_CREATE()
         ON_COMMAND(ID_WINDOW_TILE_HORZ, OnWindowTileHorz)
    ON_WM_PALETTECHANGED()
    ON_WM_QUERYENDSESSION()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// arrays of IDs used to initialize control bars

// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
         // same order as in the bitmap 'toolbar.bmp'
         ID_FILE_NEW,
         ID_FILE_OPEN,
         ID_FILE_SAVE,
                  ID_SEPARATOR,
         ID_EDIT_CUT,
         ID_EDIT_COPY,
         ID_EDIT_PASTE,
                  ID_SEPARATOR,
         ID_FILE_PRINT,
         ID_APP_ABOUT,
};

static UINT BASED_CODE indicators[] =
{
         ID_SEPARATOR,                       // status line indicator
         ID_INDICATOR_CAPS,
         ID_INDICATOR_NUM,
         ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
         // TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
         if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
                  return -1;

         if (!m_wndToolBar.Create(this) ||
                  !m_wndToolBar.LoadBitmap(IDR_MAINFRAME) ||
                  !m_wndToolBar.SetButtons(buttons,
                    sizeof(buttons)/sizeof(UINT)))
         {
                  TRACE("Failed to create toolbar\n");
                  return -1;                 // fail to create
         }

         if (!m_wndStatusBar.Create(this) ||
                  !m_wndStatusBar.SetIndicators(indicators,
                    sizeof(indicators)/sizeof(UINT)))
         {
                  TRACE("Failed to create status bar\n");
                  return -1;                 // fail to create
         }

         return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
         CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
         CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnWindowTileHorz()
{
         // TODO: Add your command handler code here
         MDITile (MDITILE_VERTICAL);
}

void CMainFrame::StatusCallback (int nID, LPCSTR lpszStatusText)
{
    m_wndStatusBar.SetPaneText( 0 /* nIndex */,  lpszStatusText, TRUE );
    m_wndStatusBar.UpdateWindow(); 
}

void CMainFrame::OnPaletteChanged(CWnd* pFocusWnd)
{
    //SendMessageToDescendants( WM_PALETTECHANGED, 0, 0, TRUE);
}




BOOL CMainFrame::OnQueryEndSession()
{
    if (!CMDIFrameWnd::OnQueryEndSession())
        return FALSE;
    
    CMDIChildWnd* pMDIChildWnd = MDIGetActive();
    if (pMDIChildWnd == NULL)
        return TRUE; // no active MDI child frame
         
    CView* pView = pMDIChildWnd->GetActiveView();
    ASSERT(pView != NULL);

    // Ask the active MDI child if its OK to exit...
    return (BOOL) pView->SendMessage(WM_QUERYENDSESSION, 0, 0);
}
