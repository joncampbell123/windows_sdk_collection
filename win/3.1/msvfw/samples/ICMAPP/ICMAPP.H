/*
| icmapp.h - header file for icmapp
|
|
*/
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

#define ABOUTBOX    1

/* Menu Items */

#define MENU_ABOUT          100
#define MENU_EXIT           101
#define MENU_NEW            102
#define MENU_CLOSE          103
#define MENU_OPEN           105
#define MENU_SAVE           106
#define MENU_COPY           107
#define MENU_PASTE          108
#define MENU_CLOSEALL       109

#define MENU_COMPRESS       201
#define MENU_DECOMPRESS     202

// zoom factor is (wParam - MENU_ZOOM) / ZOOM
// zoom factor == 0 means stretch to window

#define ZOOM                2
#define MENU_ZOOM           4000
#define MENU_ZOOMW          4000        // zoom = 0 (stretch to window)
#define MENU_ZOOM12         4001        // zoom = 1/2
#define MENU_ZOOM1          4002        // zoom = 1
#define MENU_ZOOM2          4004        // zoom = 2

#ifdef DEBUG
    extern void FAR CDECL dprintf(LPSTR, ...);
    #define DPF dprintf
#else
    #define DPF / ## /
#endif
