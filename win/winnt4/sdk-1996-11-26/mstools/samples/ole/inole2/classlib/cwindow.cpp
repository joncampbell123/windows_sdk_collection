/*
 * CWINDOW.CPP
 * Sample Code Class Libraries
 *
 * Implementation of a simple CWindow class.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
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
 * CWindow::CWindow
 * CWindow::~CWindow
 *
 * Constructor Parameters:
 *  hInst           HINSTANCE of the task owning us.
 */

CWindow::CWindow(HINSTANCE hInst)
    {
    m_hInst=hInst;
    m_hWnd =NULL;
    return;
    }


CWindow::~CWindow(void)
    {
    if (IsWindow(m_hWnd))
        DestroyWindow(m_hWnd);

    return;
    }






/*
 * CWindow::Window
 *
 * Purpose:
 *  Returns the window handle associated with this object.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HWND            Window handle for this object
 */

HWND CWindow::Window(void)
    {
    return m_hWnd;
    }
