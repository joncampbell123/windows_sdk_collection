/*
 * ICONBOX.H
 *
 * Structures and definitions for the IconBox control.
 *
 * Copyright (c)1992 Microsoft Corporation, All Right Reserved
 */


#ifndef _ICONBOX_H_
#define _ICONBOX_H_

//Function prototypes
BOOL            FIconBoxInitialize(HINSTANCE, HINSTANCE, LPSTR);
void            IconBoxUninitialize(void);
LONG CALLBACK EXPORT IconBoxWndProc(HWND, UINT, WPARAM, LPARAM);


//Window extra bytes contain the bitmap index we deal with currently.
#define CBICONBOXWNDEXTRA               (sizeof(HGLOBAL)+sizeof(BOOL))
#define IBWW_HIMAGE                     0
#define IBWW_FLABEL                     (sizeof(HGLOBAL))

#ifdef WIN32
#define GETHIMAGE(h)                    (HGLOBAL)GetWindowLong(h, IBWW_HIMAGE)
#define SETHIMAGE(h, hmf)               (HGLOBAL)SetWindowLong(h, IBWW_HIMAGE, hmf)
#define GETFLABEL(h)                    (BOOL)GetWindowLong(h, IBWW_FLABEL)
#define SETFLABEL(h, b)                 (BOOL)SetWindowLong(h, IBWW_FLABEL, b)
#else
#define GETHIMAGE(h)                    (HGLOBAL)GetWindowWord(h, IBWW_HIMAGE)
#define SETHIMAGE(h, hmf)               (HGLOBAL)SetWindowWord(h, IBWW_HIMAGE, hmf)
#define GETFLABEL(h)                    (BOOL)GetWindowWord(h, IBWW_FLABEL)
#define SETFLABEL(h, b)                 (BOOL)SetWindowWord(h, IBWW_FLABEL, b)
#endif

//Control messages
#define IBXM_IMAGESET                   (WM_USER+0)
#define IBXM_IMAGEGET                   (WM_USER+1)
#define IBXM_IMAGEFREE                  (WM_USER+2)
#define IBXM_LABELENABLE                (WM_USER+3)


#endif //_ICONBOX_H_
