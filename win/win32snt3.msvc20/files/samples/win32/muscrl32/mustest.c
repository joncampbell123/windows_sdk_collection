/*
 * MUSCROLL.C
 *
 * Contains the main window procedure of the MicroScroll control
 * that handles mouse logic, and Windows messages.
 *
 * Version 1.1, October 1991, Kraig Brockschmidt
 *
 * Win32 & Win32 control format port, April 1994, Tarn Faulkner
 */

#include <windows.h>
#include "muscrl32.h"
#include "mustest.h"


//These are globals for convenience only.
HWND        hVertEdit;
HWND        hHorzEdit;
HWND        hVertMS;
HWND        hHorzMS;


/*
 * WinMain
 *
 * Purpose:
 *  Main entry point of application.   Should register the app class
 *  if a previous instance has not done so and do any other one-time
 *  initializations.
 *
 * Parameters:
 *  Standard
 *
 * Return Value:
 *  int              Value to return to Windows--termination code.
 *
 */

int PASCAL WinMain (HANDLE hInstance, HANDLE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
    {
    WNDCLASS    wc;
    HWND        hWnd;
    MSG         msg;
    TEXTMETRIC  tm;
    HDC         hDC;
    LONG        cxFont;
    LONG        cyFont;


    if (!hPrevInstance)
        {
        wc.style          = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc    = MusTestWndProc;
        wc.cbClsExtra     = 0;
        wc.cbWndExtra     = 0;
        wc.hInstance      = hInstance;
        wc.hIcon          = LoadIcon(hInstance, "MusTestIcon");
        wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
        wc.lpszMenuName   = "MusTestMenu";
        wc.lpszClassName  = "MusTest";


        if (!RegisterClass(&wc))
            return FALSE;
        }


    //Create and show main window.
    hWnd=CreateWindow("MusTest", "MicroScrolls",
                      WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW,
                      35, 35, 250, 110,
                      NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    //Get text dimensions on which to base control sizes.
    hDC=GetDC(hWnd);
    GetTextMetrics(hDC, &tm);
    cxFont=tm.tmAveCharWidth;
    cyFont=tm.tmHeight;
    ReleaseDC(hWnd, hDC);

    //Create a 'spin' button (edit control with vertical MicroScroll
    CreateWindow("static", "Number 1-99:",
                 WS_VISIBLE | WS_CHILD | SS_LEFT,
                 2,   2+(cyFont/4), 16*cxFont, cyFont,
		 hWnd, (HMENU)ID_NULL, hInstance, 0L);

    hVertEdit=CreateWindow("edit", "50",
                 WS_VISIBLE | WS_CHILD | ES_LEFT | WS_BORDER,
                 2+16*cxFont, 2, 4*cxFont, (cyFont*3)/2,
                 hWnd, (HMENU)ID_VERTEDIT, hInstance, 0L);

    SendMessage(hVertEdit, EM_LIMITTEXT, 2, 0L);

    /*
     * Width is forced odd with 1+(2*cxFont) so the small triangle
     * looks perfectly centered.  Simple aesthetics.
     *
     * Parent window is default associate.
     */
    hVertMS=CreateWindow("MicroScroll32", "1,99,50",
                 WS_VISIBLE | WS_CHILD | MSS_SPIN,
                 2+20*cxFont, 2, 1+(2*cxFont), (cyFont*3)/2,
                 hWnd, (HMENU)ID_VERTSCROLL, hInstance, 0L);

    //Create a horizonal edit control with a horizontal MicroScroll
    hHorzEdit=CreateWindow("edit", "Horizontal Edit Control",
                 WS_VISIBLE | WS_CHILD | ES_LEFT | WS_BORDER,
                 2, 2*cyFont, 30*cxFont, (cyFont*3)/2,
                 hWnd, (HMENU)ID_VERTEDIT, hInstance, 0L);

    //Parent window is default associate.
    hHorzMS=CreateWindow("MicroScroll32", "1,99,50",
                 WS_VISIBLE | WS_CHILD | MSS_HORIZONTAL,
                 2+30*cxFont, 2*cyFont, 3*cxFont, (cyFont*3)/2,
                 hWnd, (HMENU)ID_HORZSCROLL, hInstance, 0L);

    //Change the horizontal MicroScroll colors to a blue base.
    MSCrColorSet(hHorzMS, MSCOLOR_FACE,      RGB(0,   0,   255));
    MSCrColorSet(hHorzMS, MSCOLOR_ARROW,     RGB(255, 255, 255));
    MSCrColorSet(hHorzMS, MSCOLOR_SHADOW,    RGB(0,   0,   128));
    MSCrColorSet(hHorzMS, MSCOLOR_HIGHLIGHT, RGB(0,   255, 255));

    while (GetMessage(&msg, NULL, 0, 0))
        {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        }

    return msg.wParam;
    }



/*
 * MusTestWndProc
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

LRESULT CALLBACK MusTestWndProc(HWND hWnd, UINT iMessage,
			       WPARAM wParam, LPARAM lParam)
    {
    INT         nPos;
    char        szNum[32];
    INT         nNum;
    INT         nMax;
    INT         nMin;
    BOOL        bTranslated;


    switch (iMessage)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_COMMAND:
            switch (wParam)
                {
                case IDM_EXIT:
                    PostMessage(hWnd, WM_CLOSE, 0, 0L);
                    break;
                }
            break;

        case WM_HSCROLL:
            //Affect horizontal scrolling in the edit control.
            SetFocus(hHorzEdit);

            if ((HWND)lParam==hHorzMS)
                {
                SendMessage(hHorzEdit, EM_GETSEL, (WPARAM)&nPos, (LPARAM)0);

                if (LOWORD(wParam)==SB_LINEUP)
                    nPos=(--nPos < 0) ? 0 : nPos;
                else
                    nPos=(++nPos > 32767) ? 32767 : nPos;

                SendMessage(hHorzEdit, EM_SETSEL, (WPARAM)nPos, (LPARAM)nPos);
                }
            break;


        case WM_VSCROLL:
            //For vertical scrolling we implement a spin button.

            //Ignore what comes from MSM_WCURRENTPOSSET
            if (LOWORD(wParam)==SB_THUMBTRACK)
                break;

            SetFocus(hVertEdit);

            if ((HWND)lParam==hVertMS)
                {
                // Note that like any WM_VSCROLL we can only get a 16-bit value here
                nPos=(INT)HIWORD(wParam);

                /*
                 * The code between here and the wsprintf case where
                 * we change the edit control's text is entirely to
                 * support the case where the use might have typed a
                 * different value into the edit control.
                 */

                SendMessage(hVertMS, MSM_DWRANGEGET, (WPARAM)&nMin, (LPARAM)&nMax);

                //Get the number in the control.
                nNum=GetDlgItemInt(hWnd, ID_VERTEDIT, &bTranslated, TRUE);

                /*
                 * Check if we got a valid value from the control. Otherwise
                 * use the current position.
                 */
                if (bTranslated && nNum >= 1)
                    {
                    /*
                     * If we are decrementing the value and nNum-1==nPos,
                     * then we don't need to modify anything.  Otherwise,
                     * nPos must become nNum-1 if that is >= minimum.
                     */
                    if (SB_LINEDOWN==LOWORD(wParam) && nNum-1!=nPos)
                        {
                        if (nNum > nMin)
                            nPos=nNum-1;
                        }

                    if (SB_LINEUP==LOWORD(wParam) && nNum+1!=nPos)
                        {
                        if (nNum < nMax)
                            nPos=nNum+1;
                        }
                    }


                //Only change the control if we have to.
                if (nPos!=nNum)
                    {
                    //Convert the value and put it in the control.
                    wsprintf(szNum, "%d", nPos);
                    SetDlgItemText(hWnd, ID_VERTEDIT, szNum);
                    }

                //Update the current position if it changed.
                if (nPos!=(INT)HIWORD(wParam))
                    SendMessage(hVertMS, MSM_WCURRENTPOSSET, (WPARAM)nNum, (LPARAM)0);

                /*
                 * We always want to do this in case the user typed something but we
                 * could not scroll (like they typed in the max), in which case the
                 * selection went away.
                 */
                SendMessage(hVertEdit, EM_SETSEL, 0, 32767);
                }
            break;


        default:
            return (DefWindowProc(hWnd, iMessage, wParam, lParam));
        }

    return 0L;
    }
