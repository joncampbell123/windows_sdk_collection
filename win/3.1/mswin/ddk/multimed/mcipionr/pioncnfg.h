/****************************************************************************
 *
 *   pioncnfg.h
 *
 *   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
 *
 ***************************************************************************/

#define IDD_PIONCNFG   1

#define P_COM1       100
#define P_COM2       101
#define P_COM3       102
#define P_COM4       103
#define P_SELECT     104

int PASCAL FAR pionConfig(HWND hwndParent, LPDRVCONFIGINFO lpInfo);
