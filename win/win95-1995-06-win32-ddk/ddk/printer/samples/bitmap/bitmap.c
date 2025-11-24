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
#define BUILDDLL
#include "print.h"
#include "gdidefs.inc"
#include "bitmap.h"
#include <memory.h>
#include "mdevice.h"
#include "unidrv.h"

char *rgchModuleName = "BITMAP";

BYTE szBuf[4] = {0xFF, 0xFF, 0xFF, 0xFF};

//*************************************************************
//
//  fnDump
//
//  Purpose: Gets filled in band block from GDI and sends to BlockOut
//           one scan line at a time.
//
//
//  Parameters:
//      LPDV lpdv
//      LPPOINT lpptCursor
//      WORD fMode
//      
//
//  Return: (short FAR PASCAL)
//*************************************************************

short FAR PASCAL fnDump(LPDV lpdv, LPPOINT lpptCursor, WORD fMode)
{
    short     result = 1;
    WORD      iScan, i, WidthBytes, BandHeight;
    WORD      wScanlinesPerSeg, wWAlignBytes, wSegmentInc;
    LPBITMAP  lpbmHdr;
    BOOL      fHuge;
    LPBYTE    lpSrc;
    LPBYTE    lpScanLine;
    LPEXTPDEV lpXPDV;

   // get pointer to our private data stored in UNIDRV's PDEVICE
   lpXPDV = ((LPEXTPDEV)lpdv->lpMd);

    // get ptr to PBITMAP
    lpbmHdr = (LPBITMAP)((LPSTR)lpdv + lpdv->oBruteHdr);

    // initialize some things
    fHuge = lpbmHdr->bmSegmentIndex > 0;
    lpSrc = lpbmHdr->bmBits;
    wWAlignBytes = (lpbmHdr->bmWidth+7)>>3;
    WidthBytes = lpbmHdr->bmWidthBytes;
    BandHeight = lpbmHdr->bmHeight;
    wScanlinesPerSeg = lpbmHdr->bmScanSegment;
    wSegmentInc = lpbmHdr->bmSegmentIndex;

	 // portrait or landscape orientation.
	 for (iScan = 0; ((iScan < BandHeight) && QueryAbort(lpXPDV->hAppDC,0)
         && lpXPDV->hScanBuf);iScan += wScanlinesPerSeg)
	 {
       // get next 64K segment of scans
	    if (iScan)
	    {
          WORD wRemainingScans = BandHeight - iScan;

	       // cross the segment boundary
          lpSrc = (LPBYTE)MAKELONG(0,HIWORD(lpSrc)+wSegmentInc);

          if (wScanlinesPerSeg > wRemainingScans)
             wScanlinesPerSeg = wRemainingScans;
	    }
       // loop through scan lines in 64K segment
       for (i=iScan, lpScanLine=lpSrc; 
            ((i < iScan + wScanlinesPerSeg) && QueryAbort(lpXPDV->hAppDC, 0)
            && lpXPDV->hScanBuf); i++)
       {
	          BlockOut(lpdv, lpScanLine, wWAlignBytes);
             lpScanLine += WidthBytes;
       }
	 }	// end for iScan

    return result;

} //*** fnDump

//*************************************************************
//
//  BlockOut
//
//  Purpose: Copy a scan line to the global scan buffer.
//
//
//  Parameters:
//      LPDV lpdv
//      LPSTR lpBuf
//      WORD len
//      
//
//  Return: (short FAR PASCAL)
//
//
//  Comments:
//*************************************************************

short FAR PASCAL BlockOut(LPDV lpdv, LPSTR lpBuf, WORD len)
{
   WORD wBytes;
   LPEXTPDEV lpXPDV;

   // get pointer to our private data stored in UNIDRV's PDEVICE
   lpXPDV = ((LPEXTPDEV)lpdv->lpMd);

   // convert from WORD aligned to DWORD aligned buffer
   // get DWORD amount of bytes
   wBytes = (WORD)DW_WIDTHBYTES((DWORD)len*8);

   // check to see if need to realloc scan buffer
   if ((lpXPDV->dwTotalScanBytes + wBytes) > lpXPDV->dwScanBufSize)
   {
      HANDLE hTemp;

      lpXPDV->dwScanBufSize += BUF_CHUNK;
      hTemp = GlobalReAlloc(lpXPDV->hScanBuf, lpXPDV->dwScanBufSize, 0);

      // if realloc fails, call ABORTDOC to clean up scan buf
      if (!hTemp)
         Control(lpdv, ABORTDOC, NULL, NULL);
      else
      {
         lpXPDV->hScanBuf  = hTemp;
         lpXPDV->lpScanBuf = (char _huge *)GlobalLock(lpXPDV->hScanBuf);
         lpXPDV->lpScanBuf += lpXPDV->dwTotalScanBytes;
      }
   }

   // if valid scan buf
   if (lpXPDV->hScanBuf)
   {
      // copy scan line to scan bufffer
      _fmemcpy(lpXPDV->lpScanBuf, lpBuf, len);
      lpXPDV->lpScanBuf += len;
      _fmemcpy(lpXPDV->lpScanBuf, (LPSTR)szBuf, wBytes-len);
      lpXPDV->lpScanBuf += wBytes-len;

      // update total scan bytes
      lpXPDV->dwTotalScanBytes += wBytes;
      lpXPDV->dwTotalScans++;
   }
   return wBytes;

} //*** BlockOut


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
//*************************************************************

WORD FAR PASCAL WriteDibHdr(LPEXTPDEV lpXPDV, short function)
{
   BITMAPFILEHEADER bmfh;
   BITMAPINFOHEADER bmi;
   WORD wBytes = 0;
   DWORD aRGBQ[2];

   // compute hdr and color table size
   lpXPDV->wHdrSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (2*sizeof(RGBQUAD));

   // initialize BITMAPFILEHEADER
   bmfh.bfType = ('B'|('M'<<8));
   bmfh.bfSize =  sizeof(BITMAPFILEHEADER) + 
                  sizeof(BITMAPINFOHEADER) +
                  (2*sizeof(RGBQUAD)) +
                  lpXPDV->dwTotalScanBytes ;
   bmfh.bfReserved1 =
   bmfh.bfReserved2 = 0;
   bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (2*sizeof(RGBQUAD));

   // initialize BITMAPINFOHEADER
   bmi.biSize          = sizeof(BITMAPINFOHEADER);
   bmi.biWidth         = (lpXPDV->dwTotalScanBytes/lpXPDV->dwTotalScans) * 8;
   bmi.biHeight        = lpXPDV->dwTotalScans;
   bmi.biPlanes        = 
   bmi.biBitCount      = 1;  
   bmi.biCompression   = BI_RGB;  
   bmi.biSizeImage     =   
   bmi.biXPelsPerMeter =   
   bmi.biYPelsPerMeter =   
   bmi.biClrUsed       =   
   bmi.biClrImportant  = 0;  

   // set up DIB's color table
   aRGBQ[0] = (DWORD)0;              // black
   aRGBQ[1] = (DWORD)0xFFFFFF;       // white

   // write DIB header to disk file
   _llseek(lpXPDV->hDIBFile, 0, 0);

   // write BITMAPFILEHEADER to DIB file
   wBytes += _lwrite(lpXPDV->hDIBFile, (LPSTR)&bmfh, sizeof(BITMAPFILEHEADER));

   // write BITMAPINFOHEADER to DIB file
   wBytes += _lwrite(lpXPDV->hDIBFile, (LPSTR)&bmi, sizeof(BITMAPINFOHEADER));

   // write DIB's color table to DIB file
   wBytes += _lwrite(lpXPDV->hDIBFile, (LPSTR)aRGBQ, 2*sizeof(RGBQUAD));

   return wBytes;

} //*** WriteDibHdr


//*************************************************************
//
//  DumpScans
//
//  Purpose: Dump scan buffer to DIB file
//		
//
//  Parameters:
//      void
//      
//  Return: (void)
//*************************************************************

void DumpScans(LPEXTPDEV lpXPDV)
{
   WORD i, wWidthBytes;

   // seek past header space in DIB file
   _llseek(lpXPDV->hDIBFile, lpXPDV->wHdrSize, 0);

   // compute scan line width
   wWidthBytes = (WORD)(lpXPDV->dwTotalScanBytes / lpXPDV->dwTotalScans);

   // dump scan buffer to file starting from bottom of scan buffer
   // because the DIB's image is stored upside down
   for (i=0; ((i < lpXPDV->dwTotalScans) && QueryAbort(lpXPDV->hAppDC,0)); i++)
   {
      lpXPDV->lpScanBuf -= wWidthBytes;
      _lwrite(lpXPDV->hDIBFile, lpXPDV->lpScanBuf, wWidthBytes);
   }
} //*** DumpScans

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
