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

//-----------------------------------------------------------------------------
// This files contains the module name for this mini driver.  Each mini driver
// must have a unique module name.  The module name is used to obtain the
// module handle of this Mini Driver.  The module handle is used by the
// generic library to load in tables from the Mini Driver.
//-----------------------------------------------------------------------------

#define PRINTDRIVER
#include "print.h"
#include "gdidefs.inc"
#include "cbitmap.h"
#include <memory.h>

char *rgchModuleName = "CBITMAP";

//*************************************************************
//
//  fnDump
//
//  Purpose:  
//           
//
//  Parameters:
//      LPDV lpdv
//      LPPOINT lpptCursor
//      WORD fMode
//      
//
//  Return: (short FAR PASCAL)
//
//*************************************************************

short FAR PASCAL fnDump(LPDV lpdv, LPPOINT lpptCursor, WORD fMode)
{
    short     result = 1;
    LPBITMAP  lpbmHdr;
    LPEXTPDEV lpXPDV;
    BYTE huge *hpBits;
    DWORD     dwTotalBandBytes;
    int       iNumWrites, i;

   // get pointer to our private data stored in UNIDRV's PDEVICE
   lpXPDV = ((LPEXTPDEV)lpdv->lpMd);

   // get ptr to UNIDRV's band PBITMAP
   lpbmHdr = (LPBITMAP)((LPSTR)lpdv + lpdv->oBruteHdr);

   // save bitmap settings once for each page, they are used to fill
   // in the DIB header.
   if (!lpXPDV->bBmpProps)
   {
      // Band bitmap is split horizontal
      lpXPDV->sHeight = 0;
      lpXPDV->sWidth = lpbmHdr->bmWidth;
      lpXPDV->sWidthBytes = lpbmHdr->bmWidthBytes;
      lpXPDV->cBPP = lpbmHdr->bmBitsPixel;
      lpXPDV->cPlanes = lpbmHdr->bmPlanes;

      lpXPDV->bBmpProps = TRUE;
   }

   // get ptr to band bitmap bits
   hpBits = (BYTE huge *)lpbmHdr->bmBits;

   // update total bytes so far for page
   lpXPDV->dwTotalBytes += (DWORD)lpbmHdr->bmWidthBytes * lpbmHdr->bmHeight;

   // current band buffer size
   dwTotalBandBytes = (DWORD)lpbmHdr->bmWidthBytes * lpbmHdr->bmHeight;

   iNumWrites = (int)(dwTotalBandBytes/MAX_WRITE);

   for (i=1; i <= iNumWrites; i++)
      {
       _lwrite(lpXPDV->hDIBFile, hpBits, (UINT)MAX_WRITE);
      hpBits += MAX_WRITE;
      }
    _lwrite(lpXPDV->hDIBFile, (LPSTR)hpBits,(UINT)(dwTotalBandBytes%MAX_WRITE));

   // In the case of multiple bands, update the page dimensions
   lpXPDV->sHeight += lpbmHdr->bmHeight;
      
    return result;

} //*** fnDump

//*************************************************************
//
//  WriteDibHdr
//
//  Purpose: Write DIB header to disk
//              
//
//
//  Parameters:
//      LPDV lpdv
//      short function
//      
//
//  Return: (WORD FAR PASCAL)
//
//*************************************************************

WORD FAR PASCAL WriteDibHdr(LPEXTPDEV lpXPDV, short function)
{
   BITMAPFILEHEADER bmfh;
   BITMAPINFOHEADER bmi;
   WORD wBytes = 0;

   // compute DIB header size
   lpXPDV->wHdrSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

   // move file ptr past DIB hdr
   if (function == STARTDOC)
   {
      _llseek(lpXPDV->hDIBFile, lpXPDV->wHdrSize, 0);
      return 0;
   }

   // initialize BITMAPFILEHEADER
   bmfh.bfType = ('B'|('M'<<8));
   bmfh.bfSize = lpXPDV->wHdrSize + lpXPDV->dwTotalBytes;
   bmfh.bfReserved1 =
   bmfh.bfReserved2 = 0;
   bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

   // initialize BITMAPINFOHEADER
   bmi.biSize          = sizeof(BITMAPINFOHEADER);
   bmi.biWidth         = lpXPDV->sWidth;
   bmi.biHeight        = lpXPDV->sHeight;
   bmi.biPlanes        = lpXPDV->cPlanes;
   bmi.biBitCount      = lpXPDV->cBPP;
   bmi.biCompression   = BI_RGB;  
   bmi.biSizeImage     = 0;  
   bmi.biXPelsPerMeter = 0;  
   bmi.biYPelsPerMeter = 0;  
   bmi.biClrUsed       = 0;  
   bmi.biClrImportant  = 0;  

   // write DIB header to disk file
   _llseek(lpXPDV->hDIBFile, 0, 0);

   // write BITMAPFILEHEADER to DIB file
   wBytes += _lwrite(lpXPDV->hDIBFile, (LPSTR)&bmfh, sizeof(BITMAPFILEHEADER));

   // write BITMAPINFOHEADER to DIB file
   wBytes += _lwrite(lpXPDV->hDIBFile, (LPSTR)&bmi, sizeof(BITMAPINFOHEADER));

   return wBytes;

} //*** WriteDibHdr


//*************************************************************
//
//  FileDlgProc
//
//  Purpose: To bring up the Print File dialog
//
//
//  Parameters:
//      HWND hwnd
//      WORD message
//      WPARAM wParam
//      LPARAM lParam
//      
//
//  Return: (BOOL FAR PASCAL)
//
//*************************************************************

BOOL FAR PASCAL FileDlgProc(HWND hwnd, WORD message, WPARAM wParam, LPARAM lParam)
{
   static LPEXTPDEV lpXPDV;

    switch (message)
      {
	 case WM_INITDIALOG:
	    // get pointer to our private data stored in UNIDRV's PDEVICE
	    lpXPDV = ((LPEXTPDEV)lParam);
	    break;

	 case WM_COMMAND:
		 switch ((WORD)wParam)
		 {
		    case IDCANCEL:
		       EndDialog(hwnd, 0);
		  return FALSE;

		    case IDOK:
		       GetDlgItemText(hwnd, IDE_FILENAME, (LPSTR)lpXPDV->szDIBFile, MAX_LEN);
		       EndDialog(hwnd,1);
		       break;

	       default:
		  return FALSE;
		 }
		 break;

	 default:
		 return FALSE;
	      }
      return TRUE;

} //*** FileDlgProc
