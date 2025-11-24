// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//		are changed infrequently
//
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 1995  Microsoft Corporation.  All Rights Reserved.
// 

// don't let windows.h include MMSYSTEM.H; let afxv_nt.h declare HTASK
#define WIN32_LEAN_AND_MEAN	
#include <afxole.h>	// MFC core and OLE and standard components
#include <afxext.h>	// MFC extensions (including VB)

#include <mmsystem.h>
#include <vfw.h>

// inc32\avicap.h casts to LPINFOCHUNK, which doesn't exist, 
// instead of LPCAPINFOCHUNK
#undef capFileSetInfoChunk
#define capFileSetInfoChunk(hwnd, lpInfoChunk)     ((BOOL)AVICapSM(hwnd, WM_CAP_FILE_SET_INFOCHUNK, (WPARAM)0, (LPARAM)(LPCAPINFOCHUNK)(lpInfoChunk)))
