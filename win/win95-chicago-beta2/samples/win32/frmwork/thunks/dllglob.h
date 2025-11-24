// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE:   dllglob.h
//
//  PURPOSE:
//    Contains declarations for all globally scoped names in the DLL.
//

#ifndef INC_DLLGLOB_H
#define INC_DLLGLOB_H


//-------------------------------------------------------------------------
// Constant definitions

#define DLLEXPORT __declspec(dllexport)


//-------------------------------------------------------------------------
// Exported function declarations

DLLEXPORT BOOL WINAPI DLL32Call(HWND hDlg);

// Control dialog id's. 

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
#define IDD_LPVOID32    217

#define IDD_HWND16      220
#define IDD_CHAR16      221
#define IDD_INT16       222
#define IDD_USHORT16    223
#define IDD_LONG16      224
#define IDD_ULONG16     225
#define IDD_LPSTR16     226
#define IDD_LPVOID16    227

//-------------------------------------------------------------------------
// Product identifier string defines

#define SZAPPNAME     "DLL32"
#define SZDESCRIPTION "Win32 DLL32 Skeleton"
#define SZVERSION     "Version 4.0"


//-------------------------------------------------------------------------
// Version string definitions

#define SZRCOMPANYNAME "CompanyName"
#define SZRDESCRIPTION "FileDescription"
#define SZRVERSION     "FileVersion"
#define SZRAPPNAME     "InternalName"
#define SZRCOPYRIGHT   "LegalCopyright"
#define SZRTRADEMARK   "LegalTrademarks"
#define SZRPRODNAME    "ProductName"
#define SZRPRODVER     "ProuctVersion"


#endif // INC_DLLGLOB_H
