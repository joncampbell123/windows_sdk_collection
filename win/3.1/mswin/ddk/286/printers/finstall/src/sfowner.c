/**[f******************************************************************
* sfowner.c -
*
* Copyright (C) 1988,1989 Aldus Corporation
* Copyright (C) 1989-1990 Microsoft Corporation.
* Copyright (C) 1990,1991 Hewlett-Packard Company.
*     All rights reserved.
*     Company confidential.
*
**f]*****************************************************************/
  
/*
 * $Header: 
 */

/*
 * $Log:
 */

/******************************   sfowner.c   ******************************/
//
// The functions in this module are to support owner-draw listboxes in
// the main dialog box for the font installer.
//
// Two of the functions support special WM_ messages relating to owner-draw
// controls, and are called from the message dispatch code in SFINSTAL.C
//
// The remainder of the functions are for loading bitmap resources
// for the listboxes, and for disposing of them when the dialog is
// distroyed.
//
// The listboxes should have the LBS_OWNERDRAWFIXED and LBS_HASSTRINGS
// styles set.
//
/***************************************************************************/
//
// History
//
// 09 jul 91    rk(hp) 'Re-'create FreeListboxBitmaps() to delete hBMMDisk and
//              hBMCart from memory. 
//
// 08 jun 89    peterbe     Use COLOR_HIGHLIGHT and COLOR_HIGHLIGHTTEXT
//              for highlighted items now.
//
// 15 may 89    peterbe     Added a couple of pixels to iIndentDisk and
//              IndentCart to move the bitmaps over a little.
//
// 04 may 89    peterbe     If focus changes, DrawItem() returns FALSE
//              to cause dialog proc to call DefWindowProc().
//
// 25 apr 89    peterbe     Chose bitmap on font height w/o ext. leading.
//              Changed all font height numbers.
//
// 24 apr 89    peterbe     Using 'nocrap.h' to compile in less memory.
//              Added support of BIG bitmaps.
//
// 29 mar 89    peterbe     Align strings so they appear the same whether
//              a '*' appears or not.
// 28 mar 89    peterbe     Implementing color background/foreground
//              setting in listbox selections.
//              Removed FreeListboxBitmaps().
// 27 mar 89    peterbe     Adding code for loading and using bitmaps.
//              commented out for now
// 24 mar 89    peterbe     Created.
//
/***************************************************************************/
  
  
#include "nocrap.h"
#undef  NOGDI
#undef  NOBITMAP
#undef  NOBRUSH
#undef  NOWINMESSAGES
#undef  NOCOLOR
#undef  NORASTEROPS
#undef  NOTEXTMETRIC
#undef  NOCTLMGR
#include <windows.h>
#include "strings.h"
#include "bitmaps.h"
  
extern HANDLE hLibInst;
  
// statics for listbox bitmaps etc.
static HBITMAP hBMDisk = NULL;      // handle for Disk bitmap
static HBITMAP hBMCart = NULL;      // handle for Cartridge bitmap
static int iIndent = 8;         // text indentation
int iHeight = 12;        // height of listbox item
static int iAsterWidth = 0;     // width of "* " string
static int iIndentDisk = 0;     // indentation of Disk bitmaps
static int iIndentCart = 0;     // indentation of Cartrige bitmaps
static int iDiskHeight = 8;     // height of Disk bitmap
static int iDiskWidth;          // width of Disk bitmap
static int iCartHeight = 8;     // height of Cartridge bitmap
static int iCartWidth;          // width of Cartridge bitmap
static int iBMLeading = 0;      // leading for bitmap
  
// string procedure declarations..
  
LPSTR       far PASCAL lstrcpy( LPSTR, LPSTR );
int       far PASCAL lstrlen(LPSTR);
  
// SFOWNER.C -- FillMeasureItem()
  
// This function handles the WM_MEASUREITEM message.
// In this program, it is called each time one of the listboxes
// in the main dialog is created, since these are LBS_OWNERDRAWFIXED
// listboxes.
// All it needs to do here is to fill in the height of a listbox
// item in the SF_LB_LEFT and SF_LB_RIGHT listboxes.
  
void FAR PASCAL FillMeasureItem(lpMI)
LPMEASUREITEMSTRUCT lpMI;
{
  
    // We only know about listboxes right now.
    if (lpMI->CtlType == ODT_LISTBOX)
    {
        lpMI->itemHeight = iHeight;
    }
  
} // FillMeasureItem()
  
// SFOWNER.C -- DrawItem()
  
// This function handles the WM_DRAWITEM message, for entries in
// one of the 2 listboxes in the main dialog box.
  
BOOL FAR PASCAL DrawItem(hDlg, lpItem)
HWND hDlg;              // handle of dialog
LPDRAWITEMSTRUCT lpItem;        // structure containing other data...
{
    HDC     hDC;            // handle of control DC
    HDC     hMemoryDC;      // compatible DC.
    BYTE    buf[80];        // buffer for string.
    LPSTR   lpStr;          // pointer to string
    HBITMAP hBitmap, hOldBitmap;    // bitmaps
    int     iIndentBitmap;
    int     iWidthBitmap;
    int     iHeightBitmap;
    int     iIndentExtra;
    HBRUSH  hBr;            // handle of background brush
    HBRUSH  hBrOld;         // handle of old background brush
    DWORD   rgbBackground;      // background color
    DWORD   rgbText;        // text color
  
    // Is this the kind of item we deal with in this function, and
    // do we perform this action here?
  
    if  ((ODT_LISTBOX == lpItem->CtlType) &&        // listboxes only!
        ((ODA_DRAWENTIRE | ODA_SELECT) &       // must draw item, or
        lpItem->itemAction) &&      // change selection!
        ((SF_LB_LEFT == lpItem->CtlID) ||      // these 2 listboxes!
        (SF_LB_RIGHT == lpItem->CtlID) )
        )
    {
        hDC = lpItem->hDC;
        if ( (int) SendDlgItemMessage(
            hDlg,           // dialog handle
            lpItem->CtlID,      // id of listbox control
            LB_GETTEXT,     // get text for listbox item
            lpItem->itemID,     // no. of this item in listbox.
            (long)(LPSTR) buf)  // copy string to buf.
            >= 0 )
        {
            // we got the string OK. Do a fixup if it's 0-length.
            // We then display it and any prefixed bitmap.
            if (buf[0] == 0)
            {
                buf[0] = '_';
                buf[1] = 0;
            }
  
            // we'll display any '*' as part of the string..
            // but other characters are used to select a bitmap.
            // We also adjust the indentation...
            // NOTE: The format of the strings we're dealing with
            // is determined by the buildDescrStr() function
            // in SFUTILS.C.
            lpStr = buf;
            iIndentExtra = 0;
            switch (buf[0])
            {
                case '_':       // underscore ("no fonts" string)
                    lpStr++;    // skip '_', and also, cancel the
                    iIndentExtra = - iIndent;   // indentation
                    break;
                case 0x20:      // space (temp. soft font)
                case 0xA9:      // copyright (cart)(don't show)
                    lpStr += 2;         // skip 2 spaces
                    iIndentExtra = iAsterWidth; // adj. indentation
                    break;
                case '*':       // asterix
                    break;      // do nothing -- display it!
            }
  
            // Determine the background and foreground colors,
            // clear the whole rectangle, and set the text colors.
  
            rgbBackground = GetSysColor(
            (ODS_SELECTED & (lpItem->itemState)) ?
            COLOR_HIGHLIGHT : COLOR_WINDOW);
  
            hBr = CreateSolidBrush(rgbBackground);
            hBrOld = (HBRUSH)SelectObject( hDC, (HANDLE)hBr );
            FillRect( hDC, (LPRECT)&lpItem->rcItem, hBr);
            SelectObject( hDC, (HANDLE)hBrOld );
            DeleteObject( (HANDLE)hBr );
  
            rgbText = GetSysColor(
            (ODS_SELECTED & (lpItem->itemState)) ?
            COLOR_HIGHLIGHTTEXT: COLOR_WINDOWTEXT);
  
            SetTextColor(hDC, rgbText);
            SetBkColor(hDC, rgbBackground);
  
            // Display the string, to the right of where the
            // bitmap will be.
  
            TextOut(hDC,
            iIndent + iIndentExtra,
            (lpItem->rcItem).top,
            lpStr, lstrlen(lpStr));
  
            // select graphics on the basis of the start of the string
  
            switch (buf[0])
            {
                case '_':           // 'no fonts' string ..
                    hBitmap = (HBITMAP) NULL;
                    break;          // do nothing
  
                case 0xa9:          // Cartridge font
                    hBitmap = hBMCart;
                    iIndentBitmap = iIndentCart;
                    iWidthBitmap = iCartWidth;
                    iHeightBitmap = iCartHeight;
                    break;
  
                case '*':           // perm. soft
                case 'S':           // temp. soft
                case 32:            // temp. soft
                default:            // must be soft font
                    hBitmap = hBMDisk;
                    iIndentBitmap = iIndentDisk;
                    iWidthBitmap = iDiskWidth;
                    iHeightBitmap = iDiskHeight;
            }
  
            // Display the bitmap..
  
            if (hBitmap)
            {
                // we're given hDC -- USER gets and releases that.
                hMemoryDC = CreateCompatibleDC(hDC);
                hOldBitmap = SelectObject(hMemoryDC, hBitmap);
                BitBlt( hDC,
                iIndentBitmap,
                (lpItem->rcItem).top +
                iBMLeading, // down a little
                iWidthBitmap, iHeightBitmap,
                hMemoryDC, 0, 0, SRCCOPY);
  
                SelectObject(hMemoryDC, hOldBitmap);
                DeleteDC(hMemoryDC);
            }
  
        }
    }
  
    // return FALSE if the focus changes and this is a listbox...
    return ((ODT_LISTBOX != lpItem->CtlType) ||
    !(ODA_FOCUS & (lpItem->itemAction)));
  
} // DrawItem()
  
// SFOWNER.C -- GetListboxBitmaps()
  
void FAR PASCAL GetListboxBitmaps(hDlg)
HWND hDlg;
{
    HANDLE hDC;
    int iSel;
    int iDisk;
    int iCart;
    int iDW;
    BITMAP bmDisk;      // BITMAP structure for Disk
    BITMAP bmCart;      // BITMAP structure for Cartridge
    TEXTMETRIC TM;      // structure for font information.
  
    // Determine the height of the system font, and external leading.
    // From this, determine the listbox height.
    // Also, get width of "*" string.
  
    hDC = GetDC(hDlg);
  
    // get text height
    GetTextMetrics(hDC, (LPTEXTMETRIC) &TM);
    iHeight = TM.tmHeight + TM.tmExternalLeading;
  
    // determine size of icon to use on the basis of tmHeight
    iSel = TM.tmHeight;
  
    // get width of "* " string
    iAsterWidth = (int)(WORD) GetTextExtent(hDC, (LPSTR)"* ", 2);
  
    ReleaseDC(hDlg, hDC);
  
  
    // Adjust iSel in case it doesn't match a 'standard' value..
  
    switch(iSel)
    {
        case 24:
        case 20:
        case 16:
        case 12:
        case 8:
            break;
        default:
        {
            // the numbers below are the heights of the DISK
            // bitmaps...
            if (iSel > 24)
                iSel = 24;      // BIG
            else if (iSel > 20)
                iSel = 20;      // 8514
            else if (iSel > 16)
                iSel = 16;      // VGA
            else if (iSel > 12)
                iSel = 12;      // EGA
            else
                iSel = 8;       // CGA
        }
    }
  
  
    // select which bitmaps we're going to load on the basis of the
    // system font
  
    switch(iSel)
    {
        case 24:            // BIG
            iDisk = BM_DISKBIG;
            iCart = BM_CARTBIG;
            iBMLeading = 4;
            break;
  
        case 20:            // 8514
            iDisk = BM_DISK8514;
            iCart = BM_CART8514;
            iBMLeading = 3;
            break;
  
        case 16:            // VGA resolution
            iDisk = BM_DISKVGA;
            iCart = BM_CARTVGA;
            iBMLeading = 2;
            break;
  
        case 12:            // EGA resolution
            iDisk = BM_DISKEGA;
            iCart = BM_CARTEGA;
            iBMLeading = 1;
            break;
            // Actually, CGA case ...
        default:
            iDisk = BM_DISKEGA;     // .. change these to CGA bitmaps
            iCart = BM_CARTEGA;
    }
  
    // init. some things.
  
    iIndent = 15;
    iDiskWidth = iCartWidth = 0;
  
    // Load the bitmaps, and get their sizes.
  
    if (hBMDisk = LoadBitmap(hLibInst, (LPSTR) MAKEINTRESOURCE(iDisk)))
    {
        GetObject(hBMDisk, sizeof (bmDisk), (LPSTR) &bmDisk);
        iDiskWidth = bmDisk.bmWidth;
        iDiskHeight = bmDisk.bmHeight;
        iIndent = iDiskWidth + 10;
        // This shouldn't happen with standard font sizes..
        if (iDiskHeight >= iHeight)
            iHeight = iDiskHeight + 1;
    }
  
    if (hBMCart = LoadBitmap(hLibInst, (LPSTR) MAKEINTRESOURCE(iCart)))
    {
  
        GetObject(hBMCart, sizeof (bmCart), (LPSTR) &bmCart);
        iCartWidth = bmCart.bmWidth;
        iCartHeight = bmCart.bmHeight;
        if (iCartWidth > iCartWidth)
            iIndent = iCartWidth + 10;
        if (iCartHeight >= iHeight)
            iHeight = iCartHeight + 1;
    }
  
  
    // this calculation centers the bitmaps with respect to each other.
  
    iIndentDisk = iIndentCart = 2;
    iDW = (iCartWidth - iDiskWidth) / 2;
    if(iDW > 0)
        iIndentDisk = iDW + 2;
    else if (iDW < 0)
        iIndentCart = iDW + 2;
  
    // Adjust text indentation.. in case listbox is byte-aligned!
  
  
} // GetListboxBitmaps()
  

// SFOWNER.C -- FreeListboxBitmaps()
  
void FAR PASCAL FreeListboxBitmaps(void)
{
    /* Delete bitmaps from memory. Note: DrawItem in sfowner.c selects bitmap
       object into memory context, then BitBlts to hDC, then selects default
       bitmap back into the memory context. So it is safe to delete these 
       objects, since they are not selected into a device context.         */
    
    if (hBMDisk)
         DeleteObject(hBMDisk);

    if (hBMCart)
         DeleteObject(hBMCart);

} // FreeListboxBitmaps()
