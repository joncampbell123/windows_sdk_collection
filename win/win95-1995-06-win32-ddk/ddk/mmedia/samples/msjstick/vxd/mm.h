/****************************************************************************
 *   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *   PURPOSE.
 *
 *   Copyright (c) 1994 - 1995	Microsoft Corporation.	All Rights Reserved.
 *
 *  File:       mm.h
 *  Content:	inclusion of mmsystem.h and mmddk.h (and definitions to allow
 *		this to work)
 *
 ***************************************************************************/

#ifndef __MM_INCLUDED__
#define __MM_INCLUDED__
/*
 * define all types and macros necessary to include mmsystem and mmddk
 */
#ifndef DECLARE_HANDLE
    #define DECLARE_HANDLE(name) typedef HANDLE name
#endif
#define WINAPI
#define CALLBACK
#define far
#define near
#define FAR
#define NEAR
#define DECLSPEC_IMPORT

typedef long		LRESULT;
typedef long		LPARAM;
typedef long		LONG;
typedef void 		*LPVOID,*PVOID;
typedef PVOID 		HANDLE;
typedef int		BOOL;
typedef const char	*LPCWSTR, *PCWSTR;
typedef const char	*LPCSTR, *PCSTR;
typedef char		*LPSTR;
typedef char		*LPWSTR;
typedef char		WCHAR;
typedef DWORD		*LPDWORD;
typedef WORD		*LPWORD;
typedef BYTE		*LPBYTE;
DECLARE_HANDLE(HINSTANCE);
DECLARE_HANDLE(HWND);
DECLARE_HANDLE(HTASK);
DECLARE_HANDLE(HDC);
typedef HINSTANCE HMODULE;
typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT, *PRECT, NEAR *NPRECT, FAR *LPRECT;


#ifndef WINVER
#define	WINVER	0x0400
#endif
#include <mmsystem.h>
#include <mmddk.h>

#endif
