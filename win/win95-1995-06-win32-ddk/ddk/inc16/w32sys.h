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

/*************************************************************************\
* w32sys.h
*
* Win32S i/f
*
\*************************************************************************/

#ifndef APIENTRY
#define APIENTRY _far _pascal _loadds
#endif

HANDLE APIENTRY GetPEResourceTable(WORD hFile);
HANDLE APIENTRY LoadPEResource(HANDLE hFile, LPSTR lpResTable, LPSTR lpId, LPSTR lpType);
WORD   APIENTRY GetW32SysVersion(VOID);
BOOL   APIENTRY GetPEExeInfo(LPSTR lpFileName, LPSTR lpBuff, WORD cbBuff, WORD iInfo);
WORD   APIENTRY ExecPE(LPSTR  lpPath, LPSTR lpCmd, WORD nCmdShow);
BOOL   APIENTRY IsPEFormat(LPSTR lpFileName, WORD hFile);

/*
 * Constants for GetPEExeInfo iInfo parameter
 */
#define GPEI_MODNAME     1
#define GPEI_DESCRIPTION 2
