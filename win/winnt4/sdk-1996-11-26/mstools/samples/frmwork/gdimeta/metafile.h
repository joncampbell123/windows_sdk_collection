// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993 - 1996  Microsoft Corporation.  All Rights Reserved.
//
// PURPOSE:
//    Contains declarations for metafile support routines in metafile.c
//
//-------------------------------------------------------------------------

#define HIMETRICINCH  2540    // number of hi-metric units (.01mm) per inch

//
// Aldus Placeable Metafile (APM) definitions
//

#define APM_SIGNATURE 0x9AC6CDD7

#ifndef RC_INVOKED       // RC can't handle #pragmas
#pragma pack(2)
typedef struct tagRECTS
{
    short   left;
    short   top;
    short   right;
    short   bottom;
} RECTS, *PRECTS;

typedef struct tagAPMFILEHEADER
{
    DWORD   key;
    WORD    hmf;
    RECTS   bbox;
    WORD    inch;
    DWORD   reserved;
    WORD    checksum;
} APMFILEHEADER, *PAPMFILEHEADER;
#pragma pack()
#endif  /* !RC_INVOKED */

//
// MetaFile function prototypes
//

UINT         PictureWidth(HENHMETAFILE);
UINT         PictureHeight(HENHMETAFILE);
HENHMETAFILE LoadMetaFile(LPCSTR);
WORD         APMChecksum(PAPMFILEHEADER);
HENHMETAFILE LoadPlaceableMetaFile(PAPMFILEHEADER);
BOOL         SavePlaceableMetaFile(HENHMETAFILE, LPCSTR);
HPALETTE     CreateMetaPalette(HENHMETAFILE);
HDC          GetDCFromEMF(HWND, HENHMETAFILE, HPALETTE);
