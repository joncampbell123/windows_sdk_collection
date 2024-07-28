/*
 * MUSCROLL.C
 *
 * Contains the main window procedure of the MicroScroll control
 * that handles mouse logic, and Windows messages.
 *
 * Version 1.1, October 1991, Kraig Brockschmidt
 */

#include <windows.h>
#include "muscroll.h"
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
    WORD        cxFont;
    WORD        cyFont;


    if (!hPrevInstance)
        {
        wc.style          = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc    = MusTestWndProc;
        wc.cbClsExtra     = 0;
        wc.cbWndExtra     = 0;
        wc.hInstance      = hInstance;
        wc.hIcon          = LoadIcon(hInstance, "MusTestIcon");
        wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground  = COLOR_WINDOW + 1;
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
                 hWnd, ID_VERTEDIT, hInstance, 0L);

    SendMessage(hVertEdit, EM_LIMITTEXT, 2, 0L);

    /*
     * Width is forced odd with 1+(2*cxFont) so the small triangle
     * looks perfectly centered.  Simple aesthetics.
     *
     * Parent window is default associate.
     */
    hVertMS=CreateWindow("microscroll", "1,99,50",
                 WS_VISIBLE | WS_CHILD | MSS_SPIN,
                 2+20*cxFont, 2, 1+(2*cxFont), (cyFont*3)/2,
                 hWnd, ID_VERTSCROLL, hInstance, 0L);


    //Create a horizonal edit control with a horizontal MicroScroll
    hHorzEdit=CreateWindow("edit", "Horizontal Edit Control",
                 WS_VISIBLE | WS_CHILD | ES_LEFT | WS_BORDER,
                 2, 2*cyFont, 30*cxFont, (cyFont*3)/2,
                 hWnd, ID_VERTEDIT, hInstance, 0L);

    //Parent window is default associate.
    hHorzMS=CreateWindow("microscroll", "1,99,50",
                 WS_VISIBLE | WS_CHILD | MSS_HORIZONTAL,
                 2+30*cxFont, 2*cyFont, 3*cxFont, (cyFont*3)/2,
                 hWnd, ID_HORZSCROLL, hInstance, 0L);

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

long FAR PASCAL MusTestWndProc(HWND hWnd, UINT iMessage,
			       WPARAM wParam, LPARAM lParam)
    {
    LONG        lPos;
    short       wPos;
    char        szNum[5];
    DWORD       dwRange;
    short       wNum;
    short       wMax;
    short       wMin;
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

            if (HIWORD(lParam)==hHorzMS)
                {
                lPos=SendMessage(hHorzEdit, EM_GETSEL, 0, 0L);
                wPos=LOWORD(lPos);

                if (wParam==SB_LINEUP)
                    wPos=(--wPos < 0) ? 0 : wPos;
                else
                    wPos=(++wPos > 32767) ? 32767 : wPos;

                lPos=MAKELONG(wPos, wPos);
                SendMessage(hHorzEdit, EM_SETSEL, 0, lPos);
                }
            break;


        case WM_VSCROLL:
            //For vertical scrolling we implement a spin button.

            //Ignore what comes from MSM_WCURRENTPOSSET
            if (wParam==SB_THUMBTRACK)
                break;

            SetFocus(hVertEdit);

            if (HIWORD(lParam)==hVertMS)
                {
                wPos=LOWORD(lParam);

                /*
                 * The code between here and the wsprintf case where
                 * we change the edit control's text is entirely to
                 * support the case where the use might have typed a
                 * different value into the edit control.
                 */

                dwRange=SendMessage(hVertMS, MSM_DWRANGEGET, 0, 0L);
                wMax=HIWORD(dwRange);
                wMin=LOWORD(dwRange);

                //Get the number in the control.
                wNum=GetDlgItemInt(hWnd, ID_VERTEDIT, &bTranslated, TRUE);


                /*
                 * Check if we got a valid value from the control. Otherwise
                 * use the current position.
                 */
                if (bTranslated && wNum >= 1)
                    {
                    /*
                     * If we are decrementing the value and wNum-1==wPos,
                     * then we don't need to modify anything.  Otherwise,
                     * wPos must become wNum-1 if that is >= minimum.
                     */
                    if (SB_LINEDOWN==wParam && wNum-1!=wPos)
                        {
                        if (wNum > wMin)
                            wPos=wNum-1;
                        }

                    if (SB_LINEUP==wParam && wNum+1!=wPos)
                        {
                        if (wNum < wMax)
                            wPos=wNum+1;
                        }
                    }


                //Only change the control if we have to.
                if (wPos!=wNum)
                    {
                    //Convert the value and put it in the control.
                    wsprintf(szNum, "%d", wPos);
                    SetDlgItemText(hWnd, ID_VERTEDIT, szNum);
                    }

                //Update the current position if it changed.
                if (wPos!=(short)LOWORD(lParam))
                    SendMessage(hVertMS, MSM_WCURRENTPOSSET, wNum, 0L);


                /*
                 * We always want to do this in case the user typed something but we
                 * could not scroll (like they typed in the max), in which case the
                 * selection went away.
                 */
                SendMessage(hVertEdit, EM_SETSEL, 0, MAKELONG(0, 32767));
                }
            break;


        default:
            return (DefWindowProc(hWnd, iMessage, wParam, lParam));
        }

    return 0L;
    }
