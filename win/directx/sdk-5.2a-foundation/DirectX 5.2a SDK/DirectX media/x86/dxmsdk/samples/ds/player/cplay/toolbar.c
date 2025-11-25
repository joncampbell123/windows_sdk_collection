//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1997  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

// In the C++ versions of Player the MFC class CToolbar handles all of the
// toolbar implementation, including creating a 'disabled' bitmap from the
// toolbar bitmap. In this app, the pure C version, of Player we insist on
// a seperate disabled bitmap being supplied by the programmer. This
// drastically reduces the amount of code required to maintain the toolbar.
// The buttons in this app are controls with the owner draw attribute set.

#include "stdwin.h"
#include "media.h"
#include "resource.h"

// Constants for the bitmap
const int nButtonImageWidth = 32;
const int nButtonImageHeight = 32;

// Constants for the toolbar implementation
const int nButtonBorder = 8;
const int nSeperatorGap = 6;
const int nToolbarBorderWidth = 5;
const int nToolbarBorderHeight = 3;

// The window for each button
struct{
    HWND hwndPlayButton;
    HWND hwndPauseButton;
    HWND hwndStopButton;
} toolbar;


//
// CalcRequiredSize
//
void CalcRequiredSize( SIZE *pSize )
{
    // Calculate the area required for this toolbar

    // size for 3 buttons, 2 borders and a seperator
    // ...but we'll add on a some extra seperators for good measure
    pSize->cx = (nButtonImageWidth + nButtonBorder) * 3
                    + nToolbarBorderWidth * 2 + nSeperatorGap*5;

    // size for 1 button and 2 borders
    pSize->cy = nButtonImageHeight + nButtonBorder
                    + nToolbarBorderHeight * 2;

} // CalcRequiredSize


//
// UpdateToolbar
//
// Maintains the enabled/disabled state of the buttons - we should be
// called periodically and/or whenever there is a change of graph state
//
void UpdateToolbar()
{
    EnableWindow( toolbar.hwndPlayButton, CanPlay() );
    EnableWindow( toolbar.hwndPauseButton, CanPause() );
    EnableWindow( toolbar.hwndStopButton, CanStop() );

} // UpdateToolbar


//
// InitToolbar
//
// Create the controls for the buttons
//
BOOL InitToolbar( HINSTANCE hInstance, HWND hwnd )
{
    int x;      // Position of the next button

    x = nToolbarBorderWidth;

    // The 'Play' button
    toolbar.hwndPlayButton = CreateWindow( "BUTTON",
                                           NULL,
                                           BS_OWNERDRAW | WS_VISIBLE | WS_CHILD,
                                           x,
                                           nToolbarBorderHeight,
                                           nButtonImageWidth + nButtonBorder,
                                           nButtonImageHeight + nButtonBorder,
                                           hwnd,
                                           (HMENU) ID_MEDIA_PLAY,
                                           hInstance,
                                           NULL);

    x += nButtonImageWidth + nButtonBorder;

    // The 'Pause' button
    toolbar.hwndPauseButton = CreateWindow( "BUTTON",
                                            NULL,
                                            BS_OWNERDRAW | WS_VISIBLE | WS_CHILD,
                                            x,
                                            nToolbarBorderHeight,
                                            nButtonImageWidth + nButtonBorder,
                                            nButtonImageHeight + nButtonBorder,
                                            hwnd,
                                            (HMENU) ID_MEDIA_PAUSE,
                                            hInstance,
                                            NULL);

    x += nButtonImageWidth + nButtonBorder + nSeperatorGap;

    // The 'Stop' button
    toolbar.hwndStopButton = CreateWindow( "BUTTON",
                                           NULL,
                                           BS_OWNERDRAW | WS_VISIBLE | WS_CHILD,
                                           x,
                                           nToolbarBorderHeight,
                                           nButtonImageWidth + nButtonBorder,
                                           nButtonImageHeight + nButtonBorder,
                                           hwnd,
                                           (HMENU) ID_MEDIA_STOP,
                                           hInstance,
                                           NULL);

    // We don't call UpdateToolbar to set the button states as
    // the multimedia variables may not have been initialized yet
    return TRUE;

} // InitToolbar


//
// DrawRect
//
// Draws a filled rectangle using the given stock object.
// The rectangle starts at left,top and includes right,bottom
//
void DrawRect( HDC hDC, int left, int top, int right, int bottom, UINT nStockObject )
{
    RECT rect;

    rect.left = left;
    rect.top = top;
    rect.bottom = bottom+1;
    rect.right = right+1;

    FillRect( hDC, &rect, GetStockObject( nStockObject ) );

} // DrawRect


//
// DrawButton
//
// Called by the main window whenever a button needs to be redrawn
//
void DrawButton( HINSTANCE hInstance, DRAWITEMSTRUCT FAR * lpDrawItem )
{
    HDC hSourceDC = CreateCompatibleDC( NULL );
    HGDIOBJ hgdiOldBitmap;
    int nIndex;
    UINT nUpperBrush, nLowerBrush;

    int lFrame = lpDrawItem->rcItem.left;
    int tFrame = lpDrawItem->rcItem.top;
    int rFrame = lpDrawItem->rcItem.right -1;
    int bFrame = lpDrawItem->rcItem.bottom - 1;

    // Draw a black, rounded frame around the bottom top and bottom lines
    DrawRect( lpDrawItem->hDC, lFrame+1, tFrame, rFrame-1, tFrame, BLACK_BRUSH );
    DrawRect( lpDrawItem->hDC, lFrame+1, bFrame, rFrame-1, bFrame, BLACK_BRUSH );

    // left and right lines
    DrawRect( lpDrawItem->hDC, lFrame, tFrame+1, lFrame, bFrame-1, BLACK_BRUSH );
    DrawRect( lpDrawItem->hDC, rFrame, tFrame+1, rFrame, bFrame-1, BLACK_BRUSH );

    // Adjust the pointers to point to the inside of the rectangle
    lFrame++; rFrame--;
    tFrame++; bFrame--;

    // The left and top will be highlighted and the right and bottom will
    // be in shadow if the button is raised (ie, not selected).
    // Otherwise the left and top will be in shadow and the right and bottom
    // will be their normal light gray

    if( lpDrawItem->itemState & ODS_SELECTED ){
        nUpperBrush = GRAY_BRUSH;
        nLowerBrush = LTGRAY_BRUSH;
    } else {
        nUpperBrush = WHITE_BRUSH;
        nLowerBrush = GRAY_BRUSH;
    }

    // Draw top & left border highlight/shadow
    DrawRect( lpDrawItem->hDC, lFrame, tFrame, rFrame, tFrame, nUpperBrush );
    DrawRect( lpDrawItem->hDC, lFrame, tFrame, lFrame, bFrame, nUpperBrush );

    // Draw bottom & right border highlight/shadow
    DrawRect( lpDrawItem->hDC, rFrame, tFrame+1, rFrame, bFrame, nLowerBrush );
    DrawRect( lpDrawItem->hDC, lFrame+1, bFrame, rFrame, bFrame, nLowerBrush );

    // Load the IDR_DISABLED_MAINFRAME bitmap into our source hDC if the
    // button is disabled otherwise load the IDR_MAINFRAME bitmap
    if( lpDrawItem->itemState & ODS_DISABLED )
        hgdiOldBitmap = SelectObject( hSourceDC,
            (HGDIOBJ) LoadBitmap( hInstance, MAKEINTRESOURCE( IDR_DISABLED_MAINFRAME ) ));
    else
        hgdiOldBitmap =
        hgdiOldBitmap = SelectObject( hSourceDC,
            (HGDIOBJ) LoadBitmap( hInstance, MAKEINTRESOURCE( IDR_MAINFRAME ) ));

    // Decide which button to blit to the display
    switch( lpDrawItem->CtlID ){
        case ID_MEDIA_PLAY:
            nIndex = 0;
            break;

        case ID_MEDIA_PAUSE:
            nIndex = 1;
            break;

        case ID_MEDIA_STOP:
            nIndex = 2;
            break;
    }

    // ..and blit it
    BitBlt( lpDrawItem->hDC,
            lpDrawItem->rcItem.left + nButtonBorder/2,
            lpDrawItem->rcItem.top  + nButtonBorder/2,
            nButtonImageWidth,
            nButtonImageHeight,
            hSourceDC,
            nIndex * nButtonImageWidth,
            0,
            SRCCOPY
          );

    // Restore the original bitmap
    SelectObject( hSourceDC, hgdiOldBitmap );

} // DrawButton

