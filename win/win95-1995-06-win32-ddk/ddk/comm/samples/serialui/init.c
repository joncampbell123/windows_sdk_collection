/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

//---------------------------------------------------------------------------
//
// File: init.c
//
//  This file contains the library entry points 
//
// Usage and assumptions used in this DLL.
//
//  1) Message crackers are used.  See windowsx.h and windowsx.txt.
//
//  2) Many functions are considered "member functions" of a 
//     particular class.  Because this is not C++, the function
//     names follow a special naming convention: "Class_Name".
//     In addition, it is common practice that the first 
//     argument for these type of functions is a "this" pointer
//     to the particular object.
//
//---------------------------------------------------------------------------

#include "suiprv.h"         // common headers

#include "res.h"

#pragma data_seg(DATASEG_PERINSTANCE)

HINSTANCE g_hinst = 0;

#pragma data_seg()

BOOL CALLBACK LibMain(HINSTANCE hinst, UINT wDS, DWORD unused)
    {
    g_hinst = hinst;

    return TRUE;
    }

BOOL CALLBACK WEP(BOOL fSystemExit)
    {
    return TRUE;
    }

