// -----------------------------------------------------------------
// File name:  DLL16.H
//
// This header file contains the function prototypes for functions
// exported by the DLL named DLL16.
//
// Development Team:  Dan Ruder
//
// Written by Microsoft Product Support Services, Windows Developer Support.
//
// Copyright (c) 1991-1995 Microsoft Corporation.  All rights reserved.
// -----------------------------------------------------------------


//
// Global Variable
//
extern HANDLE ghDLLInst;            // Global to store DLL instance handle


#define IDD_HWND        200
#define IDD_CHAR        201
#define IDD_INT         202
#define IDD_SHORT       203
#define IDD_LONG        204
#define IDD_ULONG       205
#define IDD_CHARSTAR    206
#define IDD_VOIDSTAR    207

#define IDD_HWND32      210
#define IDD_CHAR32      211
#define IDD_INT32       212
#define IDD_USHORT32    213
#define IDD_LONG32      214
#define IDD_ULONG32     215
#define IDD_LPSTR32     216
#define IDD_VOIDSTAR32  217

#define IDD_HWND16      220
#define IDD_CHAR16      221
#define IDD_INT16       222
#define IDD_USHORT16    223
#define IDD_LONG16      224
#define IDD_ULONG16     225
#define IDD_LPSTR16     226
#define IDD_VOIDSTAR16  227

//
// Function Prototypes
//

typedef unsigned long ULONG;

int FAR PASCAL __export DLL16Call(HWND  hDlg,
                                  char  bThunk,
                                  int   nThunk,
                                  UINT  usThunk,
                                  long  lThunk,
                                  ULONG uLong,
                                  LPSTR lpstrThunk,
                                  void *lpvoidThunk);
