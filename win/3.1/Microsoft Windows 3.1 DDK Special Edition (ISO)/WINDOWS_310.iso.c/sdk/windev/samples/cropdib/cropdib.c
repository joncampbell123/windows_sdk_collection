/* Microsoft Multimedia Systems Sample Code Library
 *
 * CropDIB.c is a Windows with Multimedia application that crops a
 * series of DIBs. It uses the DIB driver; although this is the only 
 * multimedia component that it demonstrates, the DIB.C module contains
 * many useful functions for working with DIBs and palettes.
 *
 * This application crops and "brighten" (add a specified increment to
 * the palette entries) a series of DIBs. 
 *
 * If an input "pattern" is specified, it opens a series of input files; 
 * otherwise, it opens only the specified filename. From each input file,
 * it can read one or a series of DIBs.
 *
 * If an output "pattern" is specified, it writes the DIBs to a series 
 * of output files; otherwise, it writes all the DIBs to the single
 * specified filename.
 *
 * Demonstrates:
 *         Use of DIB Driver
 *         Direct manipulation of DIB pixel data
 *         Direct manipulation of DIB palette data
 *         Cacheing of Driver to avoid re-load
 *
 * (C) Copyright Microsoft Corp. 1991.  All rights reserved.
 *
 * You have a royalty-free right to use, modify, reproduce and 
 * distribute the Sample Files (and/or any modified version) in 
 * any way you find useful, provided that you agree that 
 * Microsoft has no warranty obligations or liability for any 
 * Sample Application Files which are modified.
 *
 */

#include <windows.h>
#include <stdlib.h>
#include "cropdib.h"
#include "dib.h"


/* Globals */
char    gszAppName[] = "CropDIB";       // For title bar etc.
HANDLE  ghInst;                         // Program instance handle


/* Wait cursor */
static HCURSOR  ghcurSave;              // Previous cursor
int      giCurSave = 0;                 // Use count variable for StartWait()
#define StartWait()     ((giCurSave++ == 0)     \
    ? (ghcurSave = SetCursor(LoadCursor(NULL, IDC_WAIT))) : 0)
#define EndWait()       ((--giCurSave == 0)     \
    ? SetCursor(ghcurSave) : 0)


/* prototypes */
int  ErrMsg (HWND hwnd, int idsfmt, ...);
BOOL NEAR PASCAL DoCrop(HWND hwnd);
GLOBALHANDLE NEAR PASCAL CropDIB(GLOBALHANDLE hdibOld,
    int xCrop, int yCrop, int wCrop, int hCrop);
void NEAR PASCAL BrightenDIB(GLOBALHANDLE hdib, int iBrightenBy);
BOOL NEAR PASCAL IsPattern(LPSTR sz);
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpszCmdLine, int iCmdShow);
BOOL FAR PASCAL AboutDlgProc(HWND hwnd, unsigned wMsg,WORD wParam, LONG lParam);
BOOL FAR PASCAL CropDIBDlgProc(HWND hwnd, unsigned wMsg,WORD wParam, LONG lParam);


/* DoCrop(hwnd)
 *
 * Perform cropping operation, using information in controls in
 * dialog box <hwnd>.
 */
BOOL NEAR PASCAL DoCrop(
    HWND hwnd)                      // Dialog-box window handle
{
    BOOL  fOK = FALSE;
    char  achInFilePat[_MAX_PATH];  // Input file or pattern
    char  achOutFilePat[_MAX_PATH]; // Output file or pattern
    char  achInFile[_MAX_PATH];     // Actual input file name
    char  achOutFile[_MAX_PATH];    // Actual output file name
    HFILE   fhIn = HFILE_ERROR;                // Input file handle (or -1)
    HFILE   fhOut = HFILE_ERROR;               // Output file handle (or -1)

    int   xCrop, yCrop;             // Crop rectangle location
    int   wCrop, hCrop;             // Crop rectangle size
    int   iBrightenBy;              // Brighten by how much?

    int   iInFrameNo;               // Input frame number
    int   iOutFrameNo;              // Output frame number
    GLOBALHANDLE hdib = NULL;       // Memory DIB (or NULL)
    BOOL  fFirstDIBRead;            // Make file has at least 1 DIB

    LPSTR szCopyCrop;               // "Copying" or "Cropping"
    char  ach[_MAX_PATH + 50];      // Status message buffer

    /* Read parameters from dialog box fields.
     */
    achInFilePat[0] == 0;
    GetDlgItemText(hwnd, ID_INPUTFILEEDIT, 
        achInFilePat, sizeof(achInFilePat));

    achOutFilePat[0] == 0;
    GetDlgItemText(hwnd, ID_OUTPUTFILEEDIT,
        achOutFilePat, sizeof(achInFilePat));

    xCrop = GetDlgItemInt(hwnd, ID_XEDIT, NULL, TRUE);
    yCrop = GetDlgItemInt(hwnd, ID_YEDIT, NULL, TRUE);
    wCrop = GetDlgItemInt(hwnd, ID_WEDIT, NULL, TRUE);
    hCrop = GetDlgItemInt(hwnd, ID_HEDIT, NULL, TRUE);

    iBrightenBy = GetDlgItemInt(hwnd, ID_BRIGHTEDIT, NULL, TRUE);

    szCopyCrop = ((wCrop == 0) && (hCrop == 0)) ? "Copying" : "Cropping";

    /* Open input and output files (if no pattern specified).
     */
    if(!IsPattern(achInFilePat))
    {
        fhIn = _lopen((LPSTR)achInFilePat, OF_READ);
        if(fhIn < 0)
        {
            Error1(hwnd, IDS_CANTOPENFILE, (LPSTR)achInFilePat);
            goto DoCrop_EXIT;
        }
        lstrcpy(achInFile, achInFilePat);
    }

    if(!IsPattern(achOutFilePat))
    {
        fhOut = _lcreat((LPSTR)achOutFilePat, NULL);
        if(fhOut < 0)
        {
            Error1(hwnd, IDS_CANTOPENFILE, (LPSTR)achOutFilePat);
            goto DoCrop_EXIT;
        }
        lstrcpy(achOutFile, achOutFilePat);
    }

    /* For each input file, process all the DIBs in the input file
     */
    iInFrameNo = iOutFrameNo = 0;
    while(TRUE)
    {
        if(IsPattern(achInFilePat))
        {
            wsprintf(achInFile, achInFilePat, iInFrameNo++);
            fhIn = _lopen((LPSTR)achInFile, READ);
            if(fhIn < 0)
            {
                if(iInFrameNo == 1)
                {
                    iOutFrameNo++;  // If first open fails, continue; pattern
                    continue;       // may be 1-relative.
                }                
                else
                {
                    break;
                }
            }
        }

        /* For each DIB in input file <fhIn>, write the DIBs into one or
         * more output files.
         */
        fFirstDIBRead = TRUE;
        while(TRUE)
        {
            /* Open a DIB.
             */
            wsprintf(ach, "%s from %s", (LPSTR)szCopyCrop,
                (LPSTR)achInFile);
            SetDlgItemText(hwnd, ID_STATUSTEXT, ach);
            
            if((hdib = OpenDIB(fhIn)) == NULL)
            {
                if(fFirstDIBRead)
                {
                    Error1(hwnd, IDS_ERRORREADING, (LPSTR)achInFile);
                    goto DoCrop_EXIT;
                }
                else
                    break;
            }
            fFirstDIBRead = FALSE;

            /* Brighten the DIB.
             */
            if(iBrightenBy != 0)
                BrightenDIB(hdib, iBrightenBy);

            /* Crop the DIB.
             */
            if((wCrop != 0) || (hCrop != 0))
            {
                GLOBALHANDLE    hdibNew;

                /* Preserve hdib in case CropDib fails.
                 */
                hdibNew = CropDIB(hdib, xCrop, yCrop, wCrop, hCrop);
                if(hdibNew == NULL)
                    goto DoCrop_EXIT;
                hdib = hdibNew;
            }

            /* If there's an output file pattern, open the
             * next output file; otherwise we append the DIB to the
             * already open output file.
             */
            if(IsPattern(achOutFilePat))
            {
                wsprintf(achOutFile, achOutFilePat,
                    iOutFrameNo++);
                fhOut = _lcreat((LPSTR)achOutFile, NULL);
                if(fhOut < 0)
                {
                    Error1(hwnd, IDS_CANTOPENFILE,
                        (LPSTR)achOutFile);
                    goto DoCrop_EXIT;
                }
            }

            /* Write the DIB to <fhOut>.
             */
            wsprintf(ach, "%s from %s to %s", (LPSTR)szCopyCrop,
                (LPSTR)achInFile, (LPSTR)achOutFile);
            SetDlgItemText(hwnd, ID_STATUSTEXT, ach);

            if(!WriteDIB(fhOut, hdib))
            {
                Error1(hwnd, IDS_ERRORREADING, (LPSTR)achInFile);
                goto DoCrop_EXIT;
            }

            GlobalFree(hdib), hdib = NULL;

            /* Close the output file (if specified from a pattern).
             */
            if(IsPattern(achOutFilePat))
            {
                _lclose(fhOut);
                fhOut = HFILE_ERROR;
            }
        }

        if(IsPattern(achInFilePat))
        {
            _lclose(fhIn);
            fhIn = HFILE_ERROR;
        }
        else
            break;
    }

    fOK = TRUE;

DoCrop_EXIT:

    if(fhIn >= 0)
    {
        _lclose(fhIn);
        fhIn = HFILE_ERROR;
    }

    if(fhOut >= 0)
    {
        _lclose(fhOut);
        fhOut = HFILE_ERROR;
    }

    if(hdib != NULL)
    {
        GlobalFree(hdib);
        hdib = NULL;
    }
    return fOK;
}


/* hdibNew = CropDIB(hdibOld, xCrop, yCrop, wCrop, hCrop)
 *
 * Crops the specified rectangle from memory DIB <hdibOld> and returns
 * a new memory DIB containing the cropped portion of the original DIB.
 * If successful, it frees the original DIB (hdibOld).
 *
 * On failure, the function displays a message and returns NULL. <hdibOld> 
 * is not freed.
 */
GLOBALHANDLE NEAR PASCAL CropDIB(
    GLOBALHANDLE hdibOld,                   // DIB to crop
    int xCrop,
    int yCrop,                              // Crop origin
    int wCrop,
    int hCrop)                              // Crop extents
{
    BOOL               fOK =     FALSE;
    GLOBALHANDLE       hdibNew = NULL;
    LPBITMAPINFOHEADER lpbiOld = NULL;
    BITMAPINFOHEADER   bihOld;
    BYTE huge *        pBitsOld;

    LPBITMAPINFOHEADER lpbiNew = NULL;
    HDC                hdcNew =  NULL;

    /* Create a DIB to hold the cropped portion of the <hdibOld> 
     */
    hdibNew = CreateDib(hdibOld, wCrop, hCrop);
    if(hdibNew == NULL)
    {
        Error(NULL, IDS_OUTOFMEMORY);
        goto CropDIB_EXIT;
    }

    /* Lock both DIBs 
     */
    lpbiOld = (LPBITMAPINFOHEADER)GlobalLock(hdibOld);
    lpbiNew = (LPBITMAPINFOHEADER)GlobalLock(hdibNew);

    pBitsOld = (LPSTR)lpbiOld + (WORD)lpbiOld->biSize + PaletteSize(lpbiOld);

    /* Get a DC onto the new DIB.
     */
    hdcNew = CreateDC("DIB", NULL, NULL, (LPSTR)lpbiNew);
    if(hdcNew == NULL)
    {
        if((lpbiOld->biBitCount == 24) || (lpbiOld->biCompression != BI_RGB))
        {
            Error(NULL, IDS_FORMATNOTSUPPORTED);
        }
        else
        {
            Error(NULL, IDS_CANTLOADDIBDRV);
        }
        goto CropDIB_EXIT;
    }

    /* Copy from the old DIB to the new DIB. Get the BITMAPINFOHEADER
     * values and calculate the Y-origin of the crop area (the bitmap
     * origin is at the lower-left corner).
     */
    DibInfo(hdibOld, &bihOld);              // Get BITMAPINFOHEADER

    yCrop = (int)bihOld.biHeight - (yCrop + hCrop);

    StretchDIBits(  hdcNew,                 // Destination DC
                    0, 0,                   // Destination origin
                    wCrop, hCrop,           // Destination width & height
                    xCrop, yCrop,           // Source origin
                    wCrop, hCrop,           // Source width & height
                    pBitsOld,               // Bitmap bits
                    (LPBITMAPINFO)lpbiOld,  // Bitmap header
                    DIB_RGB_COLORS,         // Meaning of palette entries
                    SRCCOPY);               // ROP
    
    fOK = TRUE;

CropDIB_EXIT:

    if(hdcNew != NULL)
        DeleteDC(hdcNew);
    if(lpbiOld != NULL)
        GlobalUnlock(hdibOld);
    if(lpbiNew != NULL)
        GlobalUnlock(hdibNew);

    if(fOK)
    {
        /* Successful -- delete <hdibOld> and return <hdibNew> 
         */
        GlobalFree(hdibOld);
        return hdibNew;
    }
    else
        return NULL;
}


/* BrightenDIB(hdib, iBrightenBy)
 *
 * Brighten <hdib> by adding <iBrightenBy> to each component of its palette.
 */
void NEAR PASCAL BrightenDIB(
    GLOBALHANDLE    hdib,                   // DIB to brighten
    int      iBrightenBy )                  // How much brightness to add
{
    LPBITMAPINFOHEADER lpbi;
    RGBQUAD FAR *prgb;
    int cPalEntries;
    int r, g, b;


    /* Lock the DIB.
     */
    if((lpbi = (LPBITMAPINFOHEADER)GlobalLock(hdib)) == NULL)
        return;

    prgb = (RGBQUAD FAR *)((LPBYTE)lpbi + lpbi->biSize);

    cPalEntries = (int)lpbi->biClrUsed;

    while(cPalEntries > 0)
    {
        r = (int)prgb->rgbRed   + iBrightenBy;
        g = (int)prgb->rgbGreen + iBrightenBy;
        b = (int)prgb->rgbBlue  + iBrightenBy;

        prgb->rgbRed =   (BYTE)max(0, min(r, 255));
        prgb->rgbGreen = (BYTE)max(0, min(g, 255));
        prgb->rgbBlue =  (BYTE)max(0, min(b, 255));

        prgb++;
        cPalEntries--;
    }
    GlobalUnlock(hdib);
}


/* fPattern = IsPattern(sz)
 *
 * Return TRUE if <sz> is a numbered file pattern, e.g. "frm%02.dib".
 */
BOOL NEAR PASCAL IsPattern(
    LPSTR sz)
{
    while(*sz != 0)
    {
        if(*sz++ == '%')
            return TRUE;
    }

    return FALSE;
}


/* WinMain(hInst, hPrev, lpszCmdLine, cmdShow)
 * 
 * The main procedure for the App.
 */
int PASCAL WinMain(
    HINSTANCE  hInst,		   // Instance handle of current instance
    HINSTANCE  hPrev,		   // Instance handle of previous instance
    LPSTR   lpszCmdLine,        // Null-terminated command line
    int     iCmdShow )          // How window should be initially displayed
{
    FARPROC  fpfn;

    /* Save instance handle for dialog boxes */
    ghInst = hInst;


    /* Display CropDIB dialog box */

    fpfn = MakeProcInstance((FARPROC)CropDIBDlgProc, ghInst);
    DialogBox(ghInst, MAKEINTRESOURCE(CROPDIBBOX), NULL, (DLGPROC)fpfn);
    FreeProcInstance(fpfn);

    return TRUE;
}


/* AboutDlgProc(hwnd, wMsg, wParam, lParam)
 *
 * This function handles messages belonging to the "About" dialog box.
 * The only message that it looks for is WM_COMMAND, indicating the user
 * has pressed the "OK" button.  When this happens, it takes down
 * the dialog box.
 */
BOOL FAR PASCAL AboutDlgProc(
    HWND     hwnd,              // Window handle of "about" dialog box
    unsigned wMsg,              // Message number
    WORD     wParam,            // Message-dependent parameter
    LONG     lParam )           // Message-dependent parameter
{
    switch (wMsg)
    {
        case WM_INITDIALOG:
            return TRUE;

        case WM_COMMAND:
            if(wParam == IDOK)
                EndDialog(hwnd, TRUE);
            break;
    }
    return FALSE;
}


/* CropDIBDlgProc(hwnd, wMsg, wParam, lParam)
 *
 * This function handles messages belonging to the "CropDIB" dialog box.
 */
BOOL FAR PASCAL CropDIBDlgProc(
    HWND     hwnd,              // Window handle of "about" dialog box
    unsigned wMsg,              // Message number
    WORD     wParam,            // Message-dependent parameter
    LONG     lParam )           // Message-dependent parameter
{
    FARPROC  fpfn;
    HMENU    hmenuSystem;       // Handle to system menu
    HDC      hdcCache;

    switch (wMsg)
    {
        case WM_INITDIALOG:

            /* append "About" menu item to system menu */
            hmenuSystem = GetSystemMenu(hwnd, FALSE);
            AppendMenu(hmenuSystem, MF_SEPARATOR, 0, NULL);
            AppendMenu(hmenuSystem, MF_STRING, IDM_ABOUT,
                "&About CropDIB...");
            return TRUE;

        case WM_SYSCOMMAND:
            switch (wParam)
            {
                case IDM_ABOUT:

                    /* request to display "About" dialog box */
                    fpfn = MakeProcInstance((FARPROC) AboutDlgProc, ghInst);
                    DialogBox(ghInst, MAKEINTRESOURCE(ABOUTBOX),
                                hwnd, (DLGPROC)fpfn);
                    FreeProcInstance(fpfn);

                    break;
            }
            break;

        case WM_COMMAND:

            switch (wParam)
            {
                case IDOK:              // "Begin"

                    StartWait();

                    /* Cache the DIB driver here so we don't need to reload
                     * it every time we do a crop.
                     */
                    hdcCache = CreateDC("DIB", NULL, NULL, NULL);
                    if(hdcCache == NULL)
                    {
                        Error(hwnd, IDS_CANTLOADDIBDRV);
                        EndWait();
                        EndDialog(hwnd, FALSE);
                        break;
                    }
                    DoCrop(hwnd);

                    /* Clear the status text control */
                    SetDlgItemText(hwnd, ID_STATUSTEXT, "");

                    /* Get rid of the cache */
                    DeleteDC(hdcCache);

                    EndWait();
                    break;

                case IDCANCEL:          // "Done"

                    EndDialog(hwnd, TRUE);
                    break;
            }
            break;
    }
    return FALSE;
}

/* ErrMsg(hWnd, iszfmt, ...)
 *
 * Displays a message box with the formatted string supplied in <iszfmt>.
 */
int ErrMsg(HWND hwnd, int iszfmt, ...)
{
    char    ach[128];
    char    sz[80];
    
    LoadString(ghInst,iszfmt,sz,sizeof(sz));

    wvsprintf (ach, sz, (LPSTR)(&iszfmt + 1));     // Format the string 
    MessageBox(hwnd, ach, NULL, MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL);
    return FALSE;
}
