/*
 * CGIZBAR.CPP
 * Sample Code Class Libraries
 *
 * Implementation of the CGizmoBar class
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include <windows.h>
#include "classlib.h"


/*
 * CGizmoBar::CGizmoBar
 * CGizmoBar::~CGizmoBar
 *
 * Constructor Parameters:
 *  hInst           HINSTANCE of the module we're loaded in.
 */

CGizmoBar::CGizmoBar(HINSTANCE hInst)
    : CWindow(hInst)
    {
    return;
    }


CGizmoBar::~CGizmoBar(void)
    {
    return;
    }





/*
 * CGizmoBar::FInit
 *
 * Purpose:
 *  Initializes a GizmoBar object by creating the control that it
 *  owns.
 *
 * Parameters:
 *  hWndParent      HWND of the parent window.  The GizmoBar is
 *                  created up from the bottom of this window,
 *                  spanning the entire width of the window.
 *  uID             UINT id of the control.
 *  cy              UINT height to create the control
 *
 * Return Value:
 *  BOOL            TRUE if the function succeeded, FALSE otherwise.
 */

BOOL CGizmoBar::FInit(HWND hWndParent, UINT uID, UINT cy)
    {
    RECT            rc;

    /*
     * Note that the class is already registered since we live in a
     * DLL and that DLL will be loaded if anyone is using this class
     * library.
     */

    GetClientRect(hWndParent, &rc);
    m_cyBar=cy;

    m_hWnd=CreateWindow(CLASS_GIZMOBAR, TEXT("Wooley")
        , WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, rc.left, rc.top
        , rc.right-rc.left, m_cyBar, hWndParent, (HMENU)uID, m_hInst
        , 0L);

    return (NULL!=m_hWnd);
    }







/*
 * CGizmoBar::OnSize
 *
 * Purpose:
 *  Handles parent resizing.  The owner of this window is responsible
 *  to call this function when it wants the GizmoBar to resize.  The
 *  GizmoBar will automatically occupy a strip of the appropriate
 *  height along the top of the window.
 *
 * Parameters:
 *  hWndParent      HWND of the parent window to which we're resizing
 *
 * Return Value:
 *  None
 */

void CGizmoBar::OnSize(HWND hWndParent)
    {
    RECT        rc;

    GetClientRect(hWndParent, &rc);

    SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.right-rc.left
        , m_cyBar, SWP_NOZORDER);

    return;
    }






/*
 * CGizmoBar::FontSet
 *
 * Purpose:
 *  Changes the font in the StatStrip.
 *
 * Parameters:
 *  hFont           HFONT of the font to use in the control.
 *  fRedraw         BOOL indicating if the control is to repaint
 *
 * Return Value:
 *  None
 */

void CGizmoBar::FontSet(HFONT hFont, BOOL fRedraw)
    {
    SendMessage((UINT)m_hWnd, WM_SETFONT, (WPARAM)hFont, fRedraw);
    return;
    }





/*
 * CGizmoBar::FontGet
 *
 * Purpose:
 *  Retrieves the handle of the current font used in the control.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HFONT           Handle to the current font.
 */

HFONT CGizmoBar::FontGet(void)
    {
    return (HFONT)(UINT)SendMessage((UINT)m_hWnd, WM_GETFONT, 0, 0L);
    }






/*
 * CGizmoBar::Enable
 *
 * Purpose:
 *  Enables or disables the StatStrip window, graying the text if
 *  the control is disabled.
 *
 * Parameters:
 *  fEnable         BOOL specifying to enable (TRUE) or disable
 *
 * Return Value:
 *  None
 */

void CGizmoBar::Enable(BOOL fEnable)
    {
    EnableWindow(m_hWnd, fEnable);
    return;
    }






/*
 * CGizmoBar::HwndAssociateSet
 *
 * Purpose:
 *  Changes the associate window of a GizmoBar.
 *
 * Parameters:
 *  hWndNew         HWND of new associate.
 *
 * Return Value:
 *  HWND            Handle of previous associate.
 */

HWND CGizmoBar::HwndAssociateSet(HWND hWndNew)
    {
    return GBHwndAssociateSet(m_hWnd, hWndNew);
    }





/*
 * CGizmoBar::HwndAssociateGet
 *
 * Purpose:
 *  Retrieves the associate window of a GizmoBar
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HWND            Handle of current associate.
 */

HWND CGizmoBar::HwndAssociateGet(void)
    {
    return GBHwndAssociateGet(m_hWnd);
    }







/*
 * CGizmoBar::Add
 *
 * Purpose:
 *  Creates a new gizmo on the GizmoBar.  Subsequent operations
 *  should be done using the identifier, uID, for this gizmo.
 *
 * Parameters:
 *  iType           UINT type of the gizmo to create.
 *  iGizmo          UINT position (zero-based) for the gizmo.
 *  uID             UINT identifier for WM_COMMAND from this gizmo.
 *  dx, dy          UINT dimensions of the gizmo.
 *  pszText         LPTSTR initial text for edit, list, combo, text
 *  hBitmap         HBITMAP for gizmos of the button types (COMMAND
 *                  or ATTRIBUTE) specifies a source bitmap from
 *                  which the button image is taken.
 *  iImage          UINT index into hBitmap for the button image
 *  uState          UINT initial state of the gizmo.
 *
 * Return Value:
 *  BOOL            TRUE if creation succeeded, FALSE otherwise.
 */

BOOL CGizmoBar::Add(UINT iType, UINT iGizmo, UINT uID, UINT dx
    , UINT dy, LPTSTR pszText, HBITMAP hBmp, UINT iImage, UINT uState)
    {
    return GBGizmoAdd(m_hWnd, iType, iGizmo, uID, dx, dy
        , pszText, hBmp, iImage, uState);
    }







/*
 * CGizmoBar::Remove
 *
 * Purpose:
 *  Removes an existing gizmo from the GizmoBar.
 *
 * Parameters:
 *  uID             UINT identifier for this gizmo.
 *
 * Return Value:
 *  BOOL            TRUE if deletion succeeded, FALSE otherwise.
 */

BOOL CGizmoBar::Remove(UINT uID)
    {
    return GBGizmoRemove(m_hWnd, uID);
    }






/*
 * CGizmoBar::SendMessage
 *
 * Purpose:
 *  Implements the equivalent of SendMessage to a gizmo in the
 *  GizmoBar.  Separators, command buttons, and attribute buttons
 *  do not accept messages.
 *
 * Parameters:
 *  uID             UINT identifier of the gizmo to affect.
 *  iMsg            UINT message to send.
 *  wParam          WPARAM of the message.
 *  lParam          LPARAM of the message.
 *
 * Return Value:
 *  LONG            Return value from the message.  0L if the
 *                  gizmo does not accept messages.
 */

LONG CGizmoBar::SendMessage(UINT uID, UINT iMsg, WPARAM wParam
    , LPARAM lParam)
    {
    return GBGizmoSendMessage(m_hWnd, uID, iMsg, wParam, lParam);
    }







/*
 * CGizmoBar::Show
 *
 * Purpose:
 *  Shows or hides a control, adjusting the positions of all others
 *  to make room for or reuse the space for this control.
 *
 * Parameters:
 *  uID             UINT identifier of the gizmo to affect.
 *  fShow           BOOL TRUE to show the gizmo, FALSE to hide it.
 *
 * Return Value:
 *  BOOL            TRUE if the function was successful, FALSE
 *                  otherwise.
 */

BOOL CGizmoBar::Show(UINT uID, BOOL fShow)
    {
    return GBGizmoShow(m_hWnd, uID, fShow);
    }






/*
 * CGizmoBar::Enable
 *
 * Purpose:
 *  Enables or disables a control on the GizmoBar.
 *
 * Parameters:
 *  uID             UINT identifier of the gizmo to affect.
 *  fEnable         BOOL TRUE to enable the gizmo, FALSE otherwise.
 *
 * Return Value:
 *  BOOL            TRUE if the gizmo was previously disabled, FALSE
 *                  otherwise.
 */

BOOL CGizmoBar::Enable(UINT uID, BOOL fEnable)
    {
    return GBGizmoEnable(m_hWnd, uID, fEnable);
    }





/*
 * CGizmoBar::Check
 *
 * Purpose:
 *  Checks or unchecks an attribute button in the GizmoBar.  If the
 *  gizmo is part of a group of mutually exclusive attributes, then
 *  other gizmos are unchecked when this one is checked.  If this is
 *  the only one checked in these circumstances, this function is
 *  a NOP.
 *
 * Parameters:
 *  uID             UINT identifier of the gizmo to affect.
 *  fCheck          BOOL TRUE to check this gizmo, FALSE to uncheck.
 *
 * Return Value:
 *  BOOL            TRUE if the change took place.  FALSE otherwise.
 */

BOOL CGizmoBar::Check(UINT uID, BOOL fCheck)
    {
    return GBGizmoCheck(m_hWnd, uID, fCheck);
    }





/*
 * CGizmoBar::FocusSet
 *
 * Purpose:
 *  Sets the focus to a partuclar gizmo in the gizmo if that gizmo
 *  can accept the focus.  Separators, attribute buttons, text,
 *  and command buttons cannot have the focus.
 *
 * Parameters:
 *  uID             UINT identifier of the gizmo to affect.
 *
 * Return Value:
 *  BOOL            TRUE if the focus was set.  FALSE otherwise,
 *                  such as when uID identifies a control that
 *                  cannot have focus.
 *
 */

UINT CGizmoBar::FocusSet(UINT uID)
    {
    return GBGizmoFocusSet(m_hWnd, uID);
    }








/*
 * CGizmoBar::Exist
 *
 * Purpose:
 *  Determines if a gizmo of a given identifier exists.
 *
 * Parameters:
 *  uID             UINT identifier to verify.
 *
 * Return Value:
 *  BOOL            TRUE if the gizmo exists, FALSE otherwise.
 */

BOOL CGizmoBar::Exist(UINT uID)
    {
    return GBGizmoExist(m_hWnd, uID);
    }





/*
 * CGizmoBar::TypeGet
 *
 * Purpose:
 *  Returns the type of the gizmo specified by the given identifer.
 *
 * Parameters:
 *  uID             UINT identifier to find.
 *
 * Return Value:
 *  int             A GIZMOTYPE_* value if the function is
 *                  successful, otherwise -1.
 */

int CGizmoBar::TypeGet(UINT uID)
    {
    return GBGizmoTypeGet(m_hWnd, uID);
    }








/*
 * CGizmoBar::DataSet
 * CGizmoBar::DataGet
 *
 * Purpose:
 *  Sets or retrieves an extra DWORD value associated with the given
 *  gizmo.  Applications can store any information here they please.
 *
 * Parameters:
 *  uID             UINT identifier of the gizmo.
 *  dwData          (Set only) DWORD data to store with the gizmo.
 *
 * Return Value:
 *  DWORD           Set:  Previous value
 *                  Get:  Current value
 */

DWORD CGizmoBar::DataSet(UINT uID, DWORD dwData)
    {
    return GBGizmoDataSet(m_hWnd, uID, dwData);
    }


DWORD CGizmoBar::DataGet(UINT uID)
    {
    return GBGizmoDataGet(m_hWnd, uID);
    }





/*
 * CGizmoBar::NotifySet
 * CGizmoBar::NotifyGet
 *
 * Purpose:
 *  Sets or retrieves the notify status of a gizmo.  If notify is
 *  FALSE, the no WM_COMMAND messages are sent from the GizmoBar
 *  to the parent window when this gizmo is used.
 *
 * Parameters:
 *  uID             UINT identifier of the gizmo.
 *  fNotify         (Set only) BOOL new notify status to set.
 *
 * Return Value:
 *  BOOL            Set:  Previous value of the notify flag.
 *                  Get:  Current value of the notify flag.
 */

BOOL CGizmoBar::NotifySet(UINT uID, BOOL fNotify)
    {
    return GBGizmoNotifySet(m_hWnd, uID, fNotify);
    }


BOOL CGizmoBar::NotifyGet(UINT uID)
    {
    return GBGizmoNotifyGet(m_hWnd, uID);
    }







/*
 * CGizmoBar::TextSet
 * CGizmoBar::TextGet
 *
 * Purpose:
 *  Retrieves or sets text in a GizmoBar gizmo.  Separators, command
 *  buttons, and attribute buttons are not affected by this call.
 *
 * Parameters:
 *  uID             UINT identifying the gizmo.
 *  psz             LPTSTR pointing to a buffer to receive the text.
 *  cch             (Get only) UINT maximum number of chars to copy
 *                  to psz.
 *
 * Return Value:
 *  int             Number of characters copied to psz.
 */

void CGizmoBar::TextSet(UINT uID, LPTSTR psz)
    {
    GBGizmoTextSet(m_hWnd, uID, psz);
    return;
    }


int CGizmoBar::TextGet(UINT uID, LPTSTR psz, UINT cch)
    {
    return GBGizmoTextGet(m_hWnd, uID, psz, cch);
    }







/*
 * CGizmoBar::IntSet
 * CGizmoBar::IntGet
 *
 * Purpose:
 *  Retrieves or sets an integer in a GizmoBar gizmo.  Separators,
 *  command buttons, and attribute buttons are not affected by this
 *  call.
 *
 * Parameters:
 *  uID             UINT identifying the gizmo.
 *
 *  Get Parameters:
 *  pfTrans         BOOL * in which the success of the function is
 *                  returned.
 *  fSigned         BOOL TRUE to indicate if the value is signed.
 *
 *  Set Parameters:
 *  u               UINT value to set in the gizmo.
 *  fSigned         BOOL TRUE to indicate if the value is signed.
 *
 * Return Value:
 *  UINT            Integer translation of the gizmo's text.
 */

void CGizmoBar::IntSet(UINT uID, int i, BOOL fSigned)
    {
    GBGizmoIntSet(m_hWnd, uID, i, fSigned);
    return;
    }


UINT CGizmoBar::IntGet(UINT uID, BOOL *pfTrans, BOOL fSigned)
    {
    return GBGizmoIntGet(m_hWnd, uID, pfTrans, fSigned);
    }
