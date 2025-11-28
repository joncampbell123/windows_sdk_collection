//-----------------------------------------------------------------------------
// File: WinMain.cpp
//
// Desc: Windows management for DirectInput sample
//
//
// Copyright (c) 1998 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include <windowsx.h>
#include "JoyFFeed.h"

#define FEEDBACK_WINDOW_X       20
#define FEEDBACK_WINDOW_Y       60
#define FEEDBACK_WINDOW_WIDTH   200

//-----------------------------------------------------------------------------
// Function prototypes 
//-----------------------------------------------------------------------------
LRESULT CALLBACK MainDialogProc( HWND, UINT, WPARAM, LPARAM );
void OnPaint( HWND hWnd );
void OnMouseMove( HWND hWnd, int x, int y, UINT keyFlags );
void OnLeftButtonDown( HWND hWnd, int x, int y, UINT keyFlags );
void OnLeftButtonUp( HWND hWnd, int x, int y, UINT keyFlags );
int CoordToForce( int x );

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
DWORD g_dwLastEffectSet; // time of the previous force feedback effect set




//-----------------------------------------------------------------------------
// Function: WinMain(HANDLE, HANDLE, LPSTR, int)
//
// Description: 
//     Entry point for the application.  Since we use a simple dialog for 
//     user interaction we don't need to pump messages.
//
//-----------------------------------------------------------------------------
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, 
                      LPSTR lpCmdLine, int nCmdShow )
{
    // DirectInputCreate needs the instance handle
    g_hInst = hInstance;

    // Display the main dialog box.
    DialogBox( hInstance, 
            MAKEINTRESOURCE(IDD_JOY_FEEDBACK), 
            NULL, 
            (DLGPROC) MainDialogProc );

    FreeDirectInput();

    return TRUE;
}




//-----------------------------------------------------------------------------
// Function: MainDialogProc
//
// Description: 
//      Handles dialog messages
//
//-----------------------------------------------------------------------------
LRESULT CALLBACK MainDialogProc( HWND hDlg, UINT message, WPARAM wParam, 
                                 LPARAM lParam )
{
    HRESULT hr;

    switch (message) 
    {
        case WM_INITDIALOG:
            hr = InitDirectInput( hDlg );
            if ( FAILED(hr) )
            {
                MessageBox( NULL, 
                    "Error Initializing DirectInput", 
                    "DirectInput Sample", 
                    MB_ICONERROR | MB_OK );

                EndDialog( hDlg, 0 );
            }

            // init the time of the last force feedback effect
            g_dwLastEffectSet = timeGetTime();

            return TRUE;
            break;

        case WM_MOUSEMOVE:
            OnMouseMove( hDlg, LOWORD(lParam), HIWORD(lParam), wParam );
            break;

        case WM_LBUTTONDOWN:
            OnLeftButtonDown( hDlg, LOWORD(lParam), HIWORD(lParam), wParam );
            break;

        case WM_LBUTTONUP:
            OnLeftButtonUp( hDlg, LOWORD(lParam), HIWORD(lParam), wParam );
            break;

        case WM_PAINT:
            OnPaint( hDlg );
            break;

        case WM_ACTIVATE:   // sent when window changes active state
            if ( WA_INACTIVE == wParam )
            {
                g_bActive = FALSE;
            }
            else
            {
                g_bActive = TRUE;
            }

            // Set exclusive mode access to the mouse based on active state
            SetAcquire( hDlg );

            return TRUE;
            break;

        case WM_COMMAND:
            {
                switch ( LOWORD(wParam) )
                {
                case IDC_CLOSE:
                    PostQuitMessage( 0 );
                    break;
                }

                return TRUE;
            }
            break;


        case WM_CLOSE:
            EndDialog( hDlg, TRUE ); 

            return TRUE;
            break;
    }

    return FALSE;
}




//-----------------------------------------------------------------------------
// Function: OnPaint
//
// Description: 
//      Handles the WM_PAINT window message
//
//-----------------------------------------------------------------------------
void OnPaint( HWND hWnd )
{
    PAINTSTRUCT ps;
    HDC         hDC;
    HPEN        hpenOld;
    HPEN        hpenBlack;
    HBRUSH      hbrOld;
    HBRUSH      hbrBlack;
    int         x;
    int         y;
    
    hDC = BeginPaint( hWnd, &ps );

    if (NULL != hDC) 
    {
        // everything is scaled to the size of the window.
        hpenBlack = GetStockPen( BLACK_PEN );
        hpenOld = SelectPen( hDC, hpenBlack );

        // draw force feedback bounding rect
        MoveToEx( hDC, FEEDBACK_WINDOW_X, FEEDBACK_WINDOW_Y, NULL );

        LineTo( hDC, FEEDBACK_WINDOW_X, 
                     FEEDBACK_WINDOW_Y + FEEDBACK_WINDOW_WIDTH );

        LineTo( hDC, FEEDBACK_WINDOW_X + FEEDBACK_WINDOW_WIDTH, 
                     FEEDBACK_WINDOW_Y + FEEDBACK_WINDOW_WIDTH );

        LineTo( hDC, FEEDBACK_WINDOW_X + FEEDBACK_WINDOW_WIDTH, 
                     FEEDBACK_WINDOW_Y );

        LineTo( hDC, FEEDBACK_WINDOW_X, 
                     FEEDBACK_WINDOW_Y );

        // calculate center of feedback window for center marker
        x = FEEDBACK_WINDOW_X + FEEDBACK_WINDOW_WIDTH / 2;
        y = FEEDBACK_WINDOW_Y + FEEDBACK_WINDOW_WIDTH / 2;

        // draw center marker
        MoveToEx( hDC, x, y - 10, NULL );
        LineTo( hDC, x, y + 10 + 1 );
        MoveToEx( hDC, x - 10, y, NULL );
        LineTo( hDC, x + 10 + 1, y );

        hbrBlack = GetStockBrush( BLACK_BRUSH );
        hbrOld = SelectBrush( hDC, hbrBlack );

        x = MulDiv( FEEDBACK_WINDOW_WIDTH,
                    g_nXForce + DI_FFNOMINALMAX, 
                    2 * DI_FFNOMINALMAX );

        y = MulDiv( FEEDBACK_WINDOW_WIDTH, 
                    g_nYForce + DI_FFNOMINALMAX, 
                    2 * DI_FFNOMINALMAX );

        x += FEEDBACK_WINDOW_X;
        y += FEEDBACK_WINDOW_Y;

        Ellipse( hDC, x-5, y-5, x+6, y+6 );

        SelectBrush( hDC, hbrOld );
        SelectPen( hDC, hpenOld );

        EndPaint( hWnd, &ps );
    }
}





//-----------------------------------------------------------------------------
// Function: OnMouseMove
//
// Description: 
//      If the mouse button is down, then change the direction of
//      the force to match the new location.
//
//-----------------------------------------------------------------------------
void OnMouseMove( HWND hWnd, int x, int y, UINT keyFlags )
{
    DWORD dwCurrentTime;

    if ( keyFlags & MK_LBUTTON ) 
    {
        dwCurrentTime = timeGetTime();
        
        if ( dwCurrentTime - g_dwLastEffectSet < 100 )
        {
            // Don't allow setting effect more often than
            // 100ms since every time an effect is set, the
            // joystick will jerk.
            //
            // Note: This is not neccessary, and is specific to this sample
            return;
        }

        g_dwLastEffectSet = dwCurrentTime;

        x -= FEEDBACK_WINDOW_X;
        y -= FEEDBACK_WINDOW_Y;

        g_nXForce = CoordToForce( x );
        g_nYForce = CoordToForce( y );

        InvalidateRect( hWnd, 0, TRUE );
        UpdateWindow( hWnd );

        SetJoyForcesXY();
    }
}




//-----------------------------------------------------------------------------
// Function: OnLeftButtonDown
//
// Description: 
//      Capture the mouse so we can follow it, and start updating the
//      force information.
//
//-----------------------------------------------------------------------------
void OnLeftButtonDown( HWND hWnd, int x, int y, UINT keyFlags )
{
    SetCapture( hWnd );
    OnMouseMove( hWnd, x, y, MK_LBUTTON );
}




//-----------------------------------------------------------------------------
// Function: OnLeftButtonUp
//
// Description: 
//      Stop capturing the mouse when the button goes up.
//
//-----------------------------------------------------------------------------
void OnLeftButtonUp( HWND hWnd, int x, int y, UINT keyFlags )
{
    ReleaseCapture();
}




//-----------------------------------------------------------------------------
// Function: CoordToForce
//
// Description: 
//     Convert a coordinate 0 <= nCoord <= FEEDBACK_WINDOW_WIDTH 
//      to a force value in the range -DI_FFNOMINALMAX to +DI_FFNOMINALMAX.
//
//-----------------------------------------------------------------------------
int CoordToForce( int nCoord )
{
    int nForce = MulDiv( nCoord, 
                         2 * DI_FFNOMINALMAX, 
                         FEEDBACK_WINDOW_WIDTH 
                       ) - DI_FFNOMINALMAX;

    // keep force within bounds
    if ( nForce < -DI_FFNOMINALMAX ) 
    {
        nForce = -DI_FFNOMINALMAX;
    }

    if ( nForce > +DI_FFNOMINALMAX ) 
    {
        nForce = +DI_FFNOMINALMAX;
    }

    return nForce;
}

