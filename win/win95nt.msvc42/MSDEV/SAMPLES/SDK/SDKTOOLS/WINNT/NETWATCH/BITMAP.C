/*
 *  Bitmap.c
 *  
 *  Purpose:
 *      bitmap and listbox support functions for netwatch.exe
 *  
 *  Owner:
 *      MikeSart
 */
#define UNICODE 1

#include <windows.h>
#include <windowsx.h>
#include "netwatch.h"
#include "rcids.h"

/*
 *  function prototypes
 */
void DeleteBitmapLB(void);

/*
 *  globals
 */
DWORD   rgbWindowColor = 0xFF000000;    // variables for the current
DWORD   rgbHiliteColor = 0xFF000000;    // system color settings.
DWORD   rgbWindowText  = 0xFF000000;    // on a WM_SYSCOLORCHANGE
DWORD   rgbHiliteText  = 0xFF000000;    // we check to see if we need
DWORD   rgbGrayText    = 0xFF000000;    // to reload our bitmap.
DWORD   rgbDDWindow    = 0xFF000000;    //
DWORD   rgbDDHilite    = 0xFF000000;    // 0xFF000000 is an invalid RGB

// an array of integers containing the tab stops, in pixels. The tab 
// stops must be sorted in ascending order; back tabs are not allowed. 
int     rgTabs[] = { 2, 20, 38, 56, 74 };

// font style of font to use in listbox
typedef struct
{
    int     lfHeight;
    int     lfWeight;
    BYTE    lfItalic;
    TCHAR   lfFaceName[LF_FACESIZE];
} FONTSTYLE;

FONTSTYLE fontStyle = { 8, FW_NORMAL, 0, TEXT("MS Sans Serif") };

int dxbmpLB, dybmpLB;       // dx and dy of listbox bmps

HDC     hdcMemory = NULL;   // hdc to hold listbox bitmaps (for speed)

HBITMAP hbmpOrigMemBmp = NULL;  // original null bitmap in hdcMemory
HBITMAP hbmpLB = NULL;      // cached listbox bitmaps

HFONT   hfontLB = NULL;     // hfont of LB
HWND    hwndLB = NULL;      // hwnd of LB

/*
 *  DeInitBmps
 *  
 *  Purpose:
 *      cleans up LB hfonts, hdc, and hbmps
 *  
 *  Arguments:
 *  
 *  Returns:
 *      hopefully
 */
VOID
DeInitBmps(VOID)
{
    DeleteBitmapLB();
    if(hdcMemory)
    {
        DeleteDC(hdcMemory);
        hdcMemory = NULL;
    }

    if(hfontLB)
    {
        SetWindowFont(hwndLB, GetStockObject(SYSTEM_FONT), FALSE);
        DeleteObject(hfontLB);
        hfontLB = NULL;
    }
}

/*
 *  SetLBFont
 *  
 *  Purpose:
 *      creates a font from the global fontStyle
 *      sets global hfontLB to new font and WM_SETFONTs
 *      the hwndLB to the new font
 *  
 *  Arguments:
 *  
 *  Returns:
 *      yep
 */
VOID
SetLBFont(VOID)
{
    LOGFONT lf;

    lf.lfHeight = fontStyle.lfHeight;
    lf.lfWidth = 0;
    lf.lfEscapement = 0;
    lf.lfOrientation = 0;
    lf.lfWeight = fontStyle.lfWeight;
    lf.lfItalic = fontStyle.lfItalic;
    lf.lfUnderline = 0;
    lf.lfStrikeOut = 0;
    lf.lfCharSet = ANSI_CHARSET;
    lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    lf.lfQuality = DEFAULT_QUALITY;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
    lstrcpy(lf.lfFaceName, fontStyle.lfFaceName);

    hfontLB = CreateFontIndirect(&lf);
    if(hfontLB)
        SetWindowFont(hwndLB, hfontLB, FALSE);        
}

/*
 *  InitBmps
 *  
 *  Purpose:
 *      inits listbox globals, creates listbox
 *  
 *  Arguments:
 *      HWND    main hwnd of app (parent of LB)
 *  
 *  Returns:
 *      TRUE - success; FALSE - failed
 */
BOOL
InitBmps(HWND hwnd)
{
    HDC     hdcScreen;
    HBITMAP hbmpTemp;

    hdcScreen = GetDC(NULL);
    if(!hdcScreen)
        goto CantInit;
    hdcMemory = CreateCompatibleDC(hdcScreen);
    if(!hdcMemory)
        goto ReleaseScreenDC;

    hbmpTemp = CreateCompatibleBitmap(hdcMemory, 1, 1);
    if(!hbmpTemp)
        goto ReleaseMemDC;
    hbmpOrigMemBmp = SelectObject(hdcMemory, hbmpTemp); // get hbmp of NULL
    if(!hbmpOrigMemBmp)                                 // bmp for hdcMemory
        goto ReleaseMemDC;                              // for when we delete
    SelectObject(hdcMemory, hbmpOrigMemBmp);            // it later in life
    DeleteObject(hbmpTemp);
    ReleaseDC(NULL, hdcScreen);

    SetRGBValues();     // set the global RGB values
    LoadBitmapLB();     // load the bmps into hdcMemory

    hwndLB = CreateWindow(TEXT("listbox"), NULL,
        WS_CHILD | WS_VISIBLE | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT |
        WS_VSCROLL | LBS_WANTKEYBOARDINPUT | LBS_HASSTRINGS |
        LBS_OWNERDRAWFIXED,
        0, 0, 0, 0, hwnd, (HMENU)IDD_lstSHARES, ghInst, NULL);

    SetLBFont();    // set the font of our listbox
    return TRUE;

/* Error recovery exits */
ReleaseMemDC:
    DeleteDC(hdcMemory);
    hdcMemory = NULL;

ReleaseScreenDC:
    ReleaseDC(NULL, hdcScreen);

CantInit:
    return FALSE;
}

/*
 *  SetRGBValues
 *  
 *  Purpose:
 *      To set various system colors in static variables.  Called at
 *      init time and when system colors change.
 *  
 *  Arguments:
 *  
 *  Returns:
 *      Yes
 */
VOID
SetRGBValues(VOID)
{
    rgbWindowColor = GetSysColor(COLOR_WINDOW);
    rgbHiliteColor = GetSysColor(COLOR_HIGHLIGHT);
    rgbWindowText  = GetSysColor(COLOR_WINDOWTEXT);
    rgbHiliteText  = GetSysColor(COLOR_HIGHLIGHTTEXT);
    rgbGrayText    = GetSysColor(COLOR_GRAYTEXT);
}

/*
 *  MeasureItem
 *  
 *  Purpose:
 *      called from msg WM_MEASUREITEM: returns max dy of listbox items
 *  
 *  Arguments:
 *      HWND        hwnd of main window
 *      pmis        measureitemstruct from WM_MEASUREITEM call
 *  
 *  Returns:
 *      uh huh
 */
VOID
MeasureItem(HANDLE hwnd, LPMEASUREITEMSTRUCT pmis)
{
    HDC        hDC = GetDC(hwnd);
    HANDLE     hFont = hfontLB;
    TEXTMETRIC TM;

    if(!hFont)
        hFont = GetStockObject(SYSTEM_FONT);
    hFont = SelectObject(hDC, hFont);
    GetTextMetrics(hDC, &TM);
    SelectObject(hDC, hFont);
    ReleaseDC(hwnd, hDC);

    // set the height to be max of (dyfont or dybitmap)
    pmis->itemHeight = max(dybmpLB, TM.tmHeight);
}

/*
 *  OutTextFormat
 *  
 *  Purpose:
 *      to parse the string in the listbox and draw it accordingly:
 *      first char == chBOLD: line is bold
 *      first char == chUNDERLINE: line is underlined (can follow chBOLD)
 *      char == chTAB: go to next column in rgTabs
 *      '/001#': bitblt that numbered bitmap.
 *      otherwise, outtext the line
 *  
 *  Arguments:
 *      pDI     from DrawItem from WM_DRAWITEM msg
 *  
 *  Returns:
 *      yessir
 */
VOID
OutTextFormat(LPDRAWITEMSTRUCT pDI)
{
    static  TCHAR szItem[60];
    TCHAR   *pch;
    INT     nT;
    INT     nTab = 0;           // current tab we is on
    HFONT   hfDef = NULL;
    HFONT   hfOld = NULL;       // bold or underlined font
    TCHAR   *pchBuff = NULL;

    nT = (ListBox_GetTextLen(pDI->hwndItem, pDI->itemID) + 1) * sizeof(TCHAR);
    if(nT == LB_ERR)
        return;

    // if our quick buffer is big enough
    if(nT > (sizeof(szItem) / sizeof(TCHAR)))
    {
        pchBuff = GlobalAllocPtr(GHND, nT);
        if(!pchBuff)
            return;
        pch = pchBuff;
    }
    else
    {
        pch = szItem;
    }

    //Get the text string for this item
    ListBox_GetText(pDI->hwndItem, pDI->itemID, pch);

    // erase background
    ExtTextOut(pDI->hDC, 0, 0, ETO_OPAQUE, &pDI->rcItem, NULL, 0, NULL);

    // underline or bold this line?  Only check first & second char
    if(*pch == chBOLD || *pch == chUNDERLINE)
    {
        LOGFONT     lf;

        hfOld = GetWindowFont(pDI->hwndItem);
        if(!hfOld)
            hfOld = GetStockObject(SYSTEM_FONT);
        GetObject(hfOld, sizeof(lf), &lf);

        if(*pch == chBOLD)
        {
            lf.lfWeight = FW_BOLD;
            pch++;
        }
        if(*pch == chUNDERLINE)
        {
            lf.lfUnderline = TRUE;
            pch++;
        }

        hfDef = CreateFontIndirect(&lf);
        if(hfDef)
            SelectObject(pDI->hDC, hfDef);
    }

    // selected or nonselected bmps?
    nT = (ODS_SELECTED & pDI->itemState) ? (BMWIDTH * NUMBMPS) : 0;

    // parse the string
    for(; *pch; pch++)
    {
        TCHAR   *pchT;
        RECT    rc;

        if(*pch == chBITMAP)     // do we have a bitmap?
        {
            ++pch;
            // draw the bitmap
            BitBlt(pDI->hDC, pDI->rcItem.left + rgTabs[nTab],
                pDI->rcItem.top, BMWIDTH, BMHEIGHT, hdcMemory,
                nT + (int)(*pch - TEXT('0')) * BMWIDTH, 0, SRCCOPY);
            continue;
        }

        if(*pch == chTAB)    // move to next tabstop?
        {
            nTab++;
            continue;
        }

        pchT = pch;     // find end of the column of text
        while(*pchT && (*pchT != chTAB))
            pchT++;

        // set rect to drawtext in
        SetRect(&rc, pDI->rcItem.left + rgTabs[nTab], pDI->rcItem.top, 
            pDI->rcItem.right, pDI->rcItem.bottom);

        // draw the text
        ExtTextOut(pDI->hDC, rc.left, rc.top + 1, ETO_OPAQUE | ETO_CLIPPED,
            &rc, pch, pchT - pch, NULL);
        pch = pchT - 1; // move to end of this column
    }

    if(hfDef)   // delete underline or bold font if we created it
    {
        SelectObject(pDI->hDC, hfOld);
        DeleteObject(hfDef);
    }

    GlobalFreeNullPtr(pchBuff);
}

/*
 *  DrawItem
 *
 *  Purpose:
 *      Handles WM_DRAWITEM for both drive and directory listboxes.
 *
 *  Parameters:
 *      pDI     LPDRAWITEMSTRUCT passed from the WM_DRAWITEM message.
 *
 *  Return Value:
 *      ya
 */
VOID
DrawItem(LPDRAWITEMSTRUCT pDI)
{
    COLORREF    crText, crBack;

    if((int)pDI->itemID < 0)
        return;

    if((ODA_DRAWENTIRE | ODA_SELECT) & pDI->itemAction)
    {
        if(pDI->itemState & ODS_SELECTED)
        {
            // Select the appropriate text colors
            crText = SetTextColor(pDI->hDC, rgbHiliteText);
            crBack = SetBkColor(pDI->hDC, rgbHiliteColor);
        }

        // parse and spit out bmps and text
        OutTextFormat(pDI);

        // Restore original colors if we changed them above.
        if(pDI->itemState & ODS_SELECTED)
        {
            SetTextColor(pDI->hDC, crText);
            SetBkColor(pDI->hDC,   crBack);
        }
    }

    if((ODA_FOCUS & pDI->itemAction) || (ODS_FOCUS & pDI->itemState))
        DrawFocusRect(pDI->hDC, &pDI->rcItem);
}

/*
 *  RgbInvertRgb
 *  
 *  Purpose:
 *      To reverse the byte order of the RGB value (for file format
 *  
 *  Arguments:
 *  
 *  Returns:
 *      New color value (RGB to BGR)
 */
#define RgbInvertRgb(_rgbOld) \
    (LONG)RGB(GetBValue(_rgbOld), GetGValue(_rgbOld), GetRValue(_rgbOld))

/*
 *  LoadAlterBitmap (mostly stolen from commdlg)
 *  
 *  Purpose:
 *      Loads the IDB_LB bitmap and gives all the pixels that are
 *      RGBREPLACE a new color.
 *
 *  Assumption:
 *      This function will work on one bitmap during it's lifetime.
 *      (Due to the fact that it finds RGBREPLACE once and then
 *      operates on that offset whenever called again because under NT,
 *      it appears that the bitmap is cached, so the second time you go
 *      looking for RGBREPLACE, it won't be found.) You could load the
 *      resource, copy it, then modify the copy as a workaround. But I
 *      chose the cheap way out as I will only ever modify one bmp.
 *  
 *  Arguments:
 *      rgbInstead  rgb value to replace defined RGBREPLACE with
 *  
 *  Returns:
 *      NULL - failed or hbmp of new modified bitmap
 */
HBITMAP
LoadAlterBitmap(DWORD rgbInstead)
{
    static WORD         qlng = 0;   // offset into color table
    HANDLE              hbmp = NULL;
    LPBITMAPINFOHEADER  qbihInfo;
    HDC                 hdcScreen;
    HANDLE              hresLoad;
    HANDLE              hres;
    LPBYTE              qbBits;
    DWORD               rgbReplace;

    // load our listbox bmps resource
    hresLoad = FindResource(ghInst, MAKEINTRESOURCE(IDB_LB), RT_BITMAP);
    if(hresLoad == NULL)
        return NULL;
    hres = LoadResource(ghInst, hresLoad);
    if(hres == NULL)
        return NULL;

    rgbReplace = RgbInvertRgb(RGBREPLACE);
    rgbInstead = RgbInvertRgb(rgbInstead);
    qbihInfo = (LPBITMAPINFOHEADER)LockResource(hres);

    // if we haven't found the color offset yet, find it
    if(!qlng)
    {
        for(qlng = (WORD)qbihInfo->biSize; ; qlng += sizeof(DWORD))
        {
            if(*((LPBYTE)(qbihInfo) + qlng) == rgbReplace)
                break;
        }
    }

    // replace that color value with our new one
    *(DWORD *)((LPBYTE)(qbihInfo) + qlng) = (DWORD)rgbInstead;

    // Skip over the header structure
    qbBits = (LPBYTE)(qbihInfo + 1);

    // Skip the color table entries, if any
    qbBits += (1 << (qbihInfo->biBitCount)) * sizeof(RGBQUAD);

    // Create a color bitmap compatible with the display device
    hdcScreen = GetDC(NULL);
    if(hdcScreen != NULL)
    {
        hbmp = CreateDIBitmap(hdcScreen, qbihInfo, (LONG)CBM_INIT,
            qbBits, (LPBITMAPINFO) qbihInfo, DIB_RGB_COLORS);
        ReleaseDC(NULL, hdcScreen);
    }

    UnlockResource(hres);
    FreeResource(hres);

    return hbmp;
}

/*
 *  DeleteBitmapLB
 *  
 *  Purpose:
 *      Get rid of hbmpLB, if it exists
 *  
 *  Arguments:
 *  
 *  Returns:
 *      Si senor
 */
VOID 
DeleteBitmapLB(VOID)
{
    if(hbmpOrigMemBmp)
    {
        SelectObject(hdcMemory, hbmpOrigMemBmp);
        if(hbmpLB != NULL)
        {
            DeleteObject(hbmpLB);
            hbmpLB = NULL;
        }
    }
}

/*
 *  LoadBitmapLB (mostly stolen from commdlg)
 *  
 *  Purpose:
 *      Creates the listbox bitmap. If an appropriate bitmap
 *      already exists, it just returns immediately.  Otherwise, it
 *      loads the bitmap and creates a larger bitmap with both regular
 *      and highlight colors.
 *
 *  Arguments:
 *  
 *  Returns:
 *      TRUE - success; FALSE - failure
 */
BOOL 
LoadBitmapLB(VOID)
{
    BITMAP  bmp;
    HANDLE  hbmp, hbmpOrig;
    HDC     hdcTemp;
    BOOL    bWorked = FALSE;

    // check for existing bitmap and validity
    if( (hbmpLB != NULL) &&
        (rgbWindowColor == rgbDDWindow) &&
        (rgbHiliteColor == rgbDDHilite))
    {
        if(SelectObject(hdcMemory, hbmpLB))
            return TRUE;
    }

    DeleteBitmapLB();

    rgbDDWindow = rgbWindowColor;
    rgbDDHilite = rgbHiliteColor;

    if(!(hdcTemp = CreateCompatibleDC(hdcMemory)))
        goto LoadExit;

    if(!(hbmp = LoadAlterBitmap(rgbWindowColor)))
        goto DeleteTempDC;

    GetObject(hbmp, sizeof(BITMAP), (LPBYTE) &bmp);
    dybmpLB = bmp.bmHeight;
    dxbmpLB = bmp.bmWidth;

    hbmpOrig = SelectObject(hdcTemp, hbmp);

    hbmpLB = CreateDiscardableBitmap(hdcTemp, dxbmpLB*2, dybmpLB);
    if(!hbmpLB)
        goto DeleteTempBmp;

    if(!SelectObject(hdcMemory, hbmpLB))
    {
        DeleteBitmapLB();
        goto DeleteTempBmp;
    }

    BitBlt(hdcMemory, 0, 0, dxbmpLB, dybmpLB,   // copy unhighlited bmps
           hdcTemp, 0, 0, SRCCOPY);             // into hdcMemory
    SelectObject(hdcTemp, hbmpOrig);

    DeleteObject(hbmp);

    if(!(hbmp = LoadAlterBitmap(rgbHiliteColor)))
        goto DeleteTempDC;

    hbmpOrig = SelectObject(hdcTemp, hbmp);
    BitBlt(hdcMemory, dxbmpLB, 0, dxbmpLB, dybmpLB, // copy highlited bmps
        hdcTemp, 0, 0, SRCCOPY);                    // into hdcMemory
    SelectObject(hdcTemp, hbmpOrig);

    bWorked = TRUE;

DeleteTempBmp:
    DeleteObject(hbmp);
DeleteTempDC:
    DeleteDC(hdcTemp);
LoadExit:
    return bWorked;
}

void
BlitIcon(HDC hdc, LONG x, LONG y, int nBitmap)
{
    if(hdcMemory)
    {
        BitBlt(hdc, x, y, BMWIDTH, BMHEIGHT, hdcMemory,
            BMWIDTH * nBitmap, 0, SRCCOPY);
    }
}
