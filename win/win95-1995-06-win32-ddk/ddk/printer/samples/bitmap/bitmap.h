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

// BITMAP.H

#define MAX_LEN         128
#define BUF_CHUNK       32768
#define CB_LANDSCAPE	   0x0001	
#define IDE_FILENAME    100

#define DW_WIDTHBYTES(bits) (((bits)+31)/32*4)

#define LPDV_DEFINED

// documented part of UNIDRV.DLL's PDEVICE
typedef struct
    {
    short  iType;
    short  oBruteHdr;
    HANDLE hMd;
    LPSTR  lpMd;
    } PDEVICE, FAR * LPDV;

// private data for DUMP callback.
typedef struct
    {
      char       szDIBFile[MAX_LEN];
      HFILE      hDIBFile;
      OFSTRUCT   of;
      DWORD      dwScanBufSize;
      DWORD      dwTotalScanBytes;
      DWORD      dwTotalScans;
      WORD       wWidthBytes;
      HANDLE     hScanBuf;
      char _huge *lpScanBuf;
      WORD       wHdrSize;
      HDC        hAppDC;
      int        iPageNum;

    } EXTPDEV, FAR *LPEXTPDEV;

typedef struct tagBITMAPFILEHEADER
{
    UINT    bfType;
    DWORD   bfSize;
    UINT    bfReserved1;
    UINT    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILEHEADER;
typedef BITMAPFILEHEADER*      PBITMAPFILEHEADER;
typedef BITMAPFILEHEADER FAR* LPBITMAPFILEHEADER;

WORD FAR PASCAL WriteDibHdr(LPEXTPDEV, short);
void NEAR PASCAL DumpScans(LPEXTPDEV);
short FAR PASCAL BlockOut(LPDV, LPSTR, WORD);
short FAR PASCAL fnDump(LPDV, LPPOINT, WORD);
BOOL FAR PASCAL FileDlgProc(HWND, WORD, WPARAM, LPARAM);
short WINAPI Control(LPDV, WORD, LPSTR, LPSTR);
