/*
 *
 *  Window ListHScroll App
 *
 *  From Microsoft OnLine WinSDK Support
 *
 */

#include <windows.h>
#include "listhorz.h"


/*
 * WinMain
 *
 * Purpose:
 *  Main entry point of application.   Should register the app class
 *  if a previous instance has not done so and do any other one-time
 *  initializations.
 *
 * Parameters:
 *  See Windows SDK Guide to Programming, page 2-3
 *
 * Return Value:
 *  Value to return to Windows--termination code.
 *
 */

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
    {
    WNDCLASS    wndClass;
    HWND        hWnd;
    MSG         msg;



    if (!hPrevInstance)
        {
        wndClass.style          = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpfnWndProc    = ListHScrollWndProc;
        wndClass.cbClsExtra     = 0;
        wndClass.cbWndExtra     = DLGWINDOWEXTRA;
        wndClass.hInstance      = hInstance;
        wndClass.hIcon          = LoadIcon(hInstance, "ListHScrollIcon");
        wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground  = COLOR_WINDOW + 1;
        wndClass.lpszMenuName   = NULL;
        wndClass.lpszClassName  = "ListHScroll";


        if (!RegisterClass(&wndClass))
            return FALSE;
        }


    hWnd=CreateDialog(hInstance, "ListHScroll", 0, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    while (GetMessage(&msg, NULL, 0,0 ))
        {
        if (!IsDialogMessage (hWnd, &msg))
            {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            }
        }



    return msg.wParam;
    }





/*
 * ListHScrollWndProc
 *
 * Purpose:
 *  Window class procedure.  Standard callback.
 *
 * Parameters:
 *  The standard.  See Section 2.4 Windows SDK Guide to Programming,
 *  page 2-4.
 *
 * Return Value:
 *  See Parameters, above.
 *
 */


long FAR PASCAL ListHScrollWndProc(HWND hWnd, UINT iMessage,
			    WPARAM wParam, LPARAM lParam)
    {
    static HFONT    hFont;
    LOGFONT         lf;
    static BOOL     fFirstPaint;
    HWND            hList;
    HWND            hEdit;
    char            *pch;
    WORD            cb;
    WORD            iSel;
    HANDLE          hMem;

    switch (iMessage)
        {
        case WM_CREATE:
            lf.lfEscapement    = 0 ;
            lf.lfOrientation   = 0 ;
            lf.lfOutPrecision  = OUT_DEFAULT_PRECIS ;
            lf.lfClipPrecision = CLIP_DEFAULT_PRECIS ;
            lf.lfHeight        = 14;
            lf.lfWidth         = 5;
            lf.lfWeight        = 0;

            lf.lfItalic        = 0;
            lf.lfUnderline     = 0;
            lf.lfStrikeOut     = 0;

            lf.lfPitchAndFamily=FF_SWISS;

            hFont=CreateFontIndirect(&lf);
            fFirstPaint=TRUE;
            break;

        case WM_PAINT:
            if (fFirstPaint)
                {
                fFirstPaint=FALSE;
                if (!FInitListboxExtents(GetDlgItem(hWnd, ID_LISTBOX)))
                    {
                    iSel=MessageBox(hWnd,
                            "No memory--scrolling will not work right, continue?",
                            "Memory Error", MB_YESNO);

                    if (iSel==IDNO)
                        {
                        PostMessage(hWnd, WM_CLOSE, 0, 0L);
                        return 0L;
                        }
                    }

                SendDlgItemMessage(hWnd, ID_LISTBOX, WM_SETFONT, hFont, 1L);
                }

            return (DefWindowProc(hWnd, iMessage, wParam, lParam));


        case WM_DESTROY:
            FFreeListboxExtents(GetDlgItem(hWnd, ID_LISTBOX));
            PostQuitMessage(0);
            DeleteObject(hFont);
            break;

        case WM_COMMAND:
            hList=GetDlgItem(hWnd, ID_LISTBOX);

            switch (wParam)
                {
                case ID_ADD:
                    hEdit=GetDlgItem(hWnd, ID_STRINGEDIT);

                    /*
                     * Allocate a buffer to copy the edit control text.
                     * This must be done because EM_GETHANDLE does not
                     * work with single-line controls.
                     */
		    cb=(WORD)(1+SendMessage(hEdit, EM_LINELENGTH, 0, 0L));

                    hMem=LocalAlloc(LMEM_MOVEABLE | LMEM_ZEROINIT, cb+1);
                    pch=(char *)LocalLock(hMem);

                    //Get the edit control text.
                    SendMessage(hEdit, WM_GETTEXT, cb, (LONG)(LPSTR)pch);

                    //Update the scroll extent if necessary then add string.
                    WAddExtentEntry(hList, pch);
                    SendMessage(hList, LB_ADDSTRING, 0, (LONG)(LPSTR)pch);

                    LocalUnlock(hMem);
                    LocalFree(hMem);
                    break;


                case ID_DELETE:
                    //Get the current selection index.
		    iSel=(WORD)SendMessage(hList, LB_GETCURSEL, 0, 0L);

                    if (iSel!=LB_ERR)
                        {
                        //Change the extent if necessary, then delete the string.
                        WRemoveExtentEntry(hList, iSel);
                        SendMessage(hList, LB_DELETESTRING, iSel, 0L);
                        }
                    break;

                }

        default:
            return (DefWindowProc(hWnd, iMessage, wParam, lParam));
        }

    return 0L;
    }
