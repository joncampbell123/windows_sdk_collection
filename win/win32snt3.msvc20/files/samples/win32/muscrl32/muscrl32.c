/*******************************************************************************
*
*  MUSCRL32.C
*
*  A Win32 custom control DLL implementing a MicroScroll32 spin control.
*
*  Version 1.1, October 1991, Kraig Brockschmidt
*
*  Win32 & Win32 control format port, April 1994, Tarn Faulkner
*
*  PURPOSE:     Provides an example of a Windows NT custom control DLL
*               and library, with exported functions, which may be used by
*               other applications (MUTEST.EXE) and the SDK dialog editor.
*
*               This file contains the main skeleton of the implementation,
*               and the code for supporting editing of control styles from
*               resource editors such as the SDK dialog editor.
*
*
*  FUNCTIONS:   DllMain()            - Registers MicroScroll32 window class 
*                                       when a process loads this DLL.
*               MuScrl32WndProc()    - Window procedure for spincube
*                                      control.
*
*               **Dialog editor related routines**
*
*               CustomControlInfoA() - Called by DLGEDIT to initialize
*                                      a CCINFO structure(s).
*               MuScrl32SizeToText() - Called by DLGEDIT if user requests
*                                      that control be sized to fit text.
*               MuScrl32Style()      - Brings up dialog box which allows
*                                      user to modify control style.
*               MuScrl32DlgProc()    - Procedure for control style dialog.
*               DWFormStyleFlags()   - Set style flags from dialog
*               FRangePositionCheck  - Check dialog range text field
*
\******************************************************************************/

#include <windows.h>
#include "mscrdll.h"

//
// Some global vars for this module
//

HANDLE    ghMod;   // DLL's module handle
LPCCSTYLE gpccs;   // global pointer to a CCSTYLE structure

// Registering any control specific style flags

CCSTYLEFLAGA aMuScrl32StyleFlags[] = { { MSS_VERTICAL,      0, "MSS_VERTICAL"    },
                                       { MSS_HORIZONTAL,    0, "MSS_HORIZONTAL" },
                                       { MSS_NOPEGSCROLL,   0, "MSS_NOPEGSCROLL" },
                                       { MSS_TEXTHASRANGE,  0, "MSS_TEXTHASRANGE" },
                                       { MSS_INVERTRANGE,   0, "MSS_INVERTRANGE" } };

/******************************************************************************\
*
*  FUNCTION:    DllMain
*
*  INPUTS:      hDLL       - DLL module handle
*               dwReason   - reason being called (e.g. process attaching)
*               lpReserved - reserved
*
*  RETURNS:     TRUE if initialization passed, or
*               FALSE if initialization failed.
*
*  COMMENTS:    On DLL_PROCESS_ATTACH registers the MUSCRL32CLASS
*
*               DLL initialization serialization is guaranteed within a
*               process (if multiple threads then DLL entry points are
*               serialized), but is not guaranteed across processes.
*
*               When synchronization objects are created, it is necesaary
*               to check the return code of GetLastError even if the create
*               call succeeded. If the object existed, ERROR_ALREADY_EXISTED
*               will be returned.
*
*               Note that CRT initialization is already done for us by Visual
*               C++ whose _DllMainCRTStartup() calls DLLMain().
*
\******************************************************************************/

BOOL WINAPI DllMain (HANDLE hDLL, DWORD dwReason, LPVOID lpReserved)
{
    
  // CRT initialization already done by _DllMainCRTStartup 
  // which call this.  See LINK /ENTRY for details

  WNDCLASS wc;
  ghMod = hDLL;

  switch (dwReason)
  {
    case DLL_PROCESS_ATTACH:
    {
      wc.style         = CS_DBLCLKS | CS_GLOBALCLASS
                         | CS_VREDRAW | CS_HREDRAW;
      wc.lpfnWndProc   = (WNDPROC) MuScrl32WndProc;
      wc.cbClsExtra    = 0;
      wc.cbWndExtra    = MUSCRL32_EXTRA;
      wc.hInstance     = hDLL;
      wc.hIcon         = NULL;
      wc.hCursor       = LoadCursor (NULL, IDC_ARROW);
      wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
      wc.lpszMenuName  = (LPSTR) NULL;
      wc.lpszClassName = (LPSTR) MUSCRL32CLASS;

      if (!RegisterClass (&wc))
      {
        MessageBox (NULL,
                    (LPCTSTR) "DllMain(): RegisterClass() failed",
                    (LPCTSTR) "Err! - MUSCRL32.DLL",
                    MB_OK | MB_ICONEXCLAMATION);

        return FALSE;
      }

      break;
    }

    default:

      break;
  }
  return TRUE;
}

/******************************************************************************\
*
*  FUNCTION:    CustomControlInfoA
*
*  INPUTS:      acci - pointer to an array of CCINFOA structures
*
*  RETURNS:     Number of controls supported by this DLL
*
*  COMMENTS:    See CUSTCNTL.H for more info
*
\******************************************************************************/

UINT CALLBACK CustomControlInfoA (LPCCINFOA acci)
{
  //
  // Dlgedit is querying the number of controls this DLL supports, so return 2.
  //   Then we'll get called again with a valid "acci"
  //

  if (!acci)
    return 2;

  //
  // Fill in the constant values.
  //

  acci[0].flOptions         = 0;
  acci[0].cxDefault         = 11;      // default width  (dialog units)
  acci[0].cyDefault         = 20;      // default height (dialog units)
  acci[0].flStyleDefault    = WS_CHILD | WS_VISIBLE | MSS_VERTICAL;
  acci[0].flExtStyleDefault = 0;
  acci[0].flCtrlTypeMask    = 0;
  acci[0].cStyleFlags       = NUM_MUSCRL32_STYLES;
  acci[0].aStyleFlags       = aMuScrl32StyleFlags;
  acci[0].lpfnStyle         = MuScrl32Style;
  acci[0].lpfnSizeToText    = MuScrl32SizeToText;
  acci[0].dwReserved1       = 0;
  acci[0].dwReserved2       = 0;

  //
  // Copy the strings
  //
  // NOTE: MAKE SURE THE STRINGS COPIED DO NOT EXCEED THE LENGTH OF
  //       THE BUFFERS IN THE CCINFO STRUCTURE!  See CUSCNTL.H
  //

  lstrcpy (acci[0].szClass, MUSCRL32CLASS);
  lstrcpy (acci[0].szDesc,  MUSCRL32DESCRIPTION);
  lstrcpy (acci[0].szTextDefault, MUSCRL32DEFAULTTEXT);

  // Register a second type defaulting to horizontal

  acci[1].flOptions         = 0;
  acci[1].cxDefault         = 20;      // default width  (dialog units)
  acci[1].cyDefault         = 15;      // default height (dialog units)
  acci[1].flStyleDefault    = WS_CHILD | WS_VISIBLE | MSS_HORIZONTAL;
  acci[1].flExtStyleDefault = 0;
  acci[1].flCtrlTypeMask    = 0;
  acci[1].cStyleFlags       = NUM_MUSCRL32_STYLES;
  acci[1].aStyleFlags       = aMuScrl32StyleFlags;
  acci[1].lpfnStyle         = MuScrl32Style;
  acci[1].lpfnSizeToText    = MuScrl32SizeToText;
  acci[1].dwReserved1       = 0;
  acci[1].dwReserved2       = 0;

  //
  // Copy the strings
  //
  // NOTE: MAKE SURE THE STRINGS COPIED DO NOT EXCEED THE LENGTH OF
  //       THE BUFFERS IN THE CCINFO STRUCTURE!  See CUSCNTL.H
  //

  lstrcpy (acci[1].szClass, MUSCRL32CLASS);
  lstrcpy (acci[1].szDesc,  MUSCRL32DESCRIPTION);
  lstrcpy (acci[1].szTextDefault, MUSCRL32DEFAULTTEXT);

  //
  // Return the number of controls that the DLL supports
  //

  return 2;
}


/******************************************************************************\
*
*  FUNCTION:    MuScrl32SizeToText
*
*  INPUTS:      flStyle    - control style
*               flExtStyle - control extended style
*               hFont      - handle of font used to draw text
*               pszText    - control text
*
*  RETURNS:     Width (in pixels) control must be to accomodate text, or
*               -1 if an error occurs.
*
*  COMMENTS:    Just no-op here (since we never actually display text in
*               the control it doesn't need to be resized).
*
\******************************************************************************/

INT CALLBACK MuScrl32SizeToText (DWORD flStyle, DWORD flExtStyle,
                                 HFONT hFont,   LPSTR pszText)
{
  return -1;
}


/******************************************************************************\
*
*  FUNCTION:    MuScrl32WndProc (standard window procedure INPUTS/RETURNS)
*
*  COMMENTS:    This is the window procedure for our custom control. At
*               creation we alloc a MUSCRL32INFO struct, initialize it,
*               and associate it with this particular control. 
*
\******************************************************************************/

LRESULT CALLBACK MuScrl32WndProc (HWND hWnd, UINT nMsg, WPARAM wParam,
                                  LPARAM lParam)
{
    PMUSCROLL       pMS;
    POINT           pt;
    RECT            rect;
    LONG            x, y;
    LONG            cx, cy;
    WORD            wState;

    /*
     * Get a pointer to the MUSCROLL structure for this control.
     * Note that if we do this before WM_NCCREATE where we allocate
     * the memory, pMS will be NULL, which is not a problem since
     * we do not access it until after WM_NCCREATE.
     */
    pMS=(PMUSCROLL)GetWindowLong(hWnd, GWL_MUSCRL32DATA);

    //Let the API handler process WM_USER+xxxx messages
    if (nMsg >= WM_USER)
        return LMicroScrollAPI(hWnd, nMsg, wParam, lParam, pMS);

    //Handle standard Windows messages.
    switch (nMsg)
    {
        case WM_NCCREATE:
        case WM_CREATE:
            return LMicroScrollCreate(hWnd, nMsg, pMS, (LPCREATESTRUCT)lParam);

        case WM_DESTROY:
            //Free the control's memory.
            if (pMS)
                LocalFree( LocalHandle((LPVOID)pMS) );
        break;

        case WM_ERASEBKGND:
            /*
             * Eat this message to avoid erasing portions that
             * we are going to repaint in WM_PAINT.  Part of a
             * change-state-and-repaint strategy is to rely on
             * WM_PAINT to do anything visual, which includes
             * erasing invalid portions.  Letting WM_ERASEBKGND
             * erase the background is redundant.
             */
        break;

        case WM_PAINT:
            return LMicroScrollPaint(hWnd, pMS);

        case WM_ENABLE:
            /*
             * Handles disabling/enabling case.  Example of a
             * change-state-and-repaint strategy since we let the
             * painting code take care of the visuals.
             */
            if (wParam)
                StateClear(pMS, MUSTATE_GRAYED);
            else
                StateSet(pMS, MUSTATE_GRAYED);

            //Force a repaint since the control will look different.
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        break;

        case WM_SHOWWINDOW:
            /*
             * Set or clear the hidden flag. Windows will
             * automatically force a repaint if we become visible.
             */
            if (wParam)
                StateClear(pMS, MUSTATE_HIDDEN);
            else
                StateSet(pMS, MUSTATE_HIDDEN);

        break;

        case WM_CANCELMODE:
            /*
             * IMPORTANT MESSAGE!  WM_CANCELMODE means that a
             * dialog or some other modal process has started.
             * we must make sure that we cancel any clicked state
             * we are in, kill the timers, and release the capture.
             */
            StateClear(pMS, MUSTATE_DOWNCLICK | MUSTATE_UPCLICK);
            KillTimer(hWnd, IDT_FIRSTCLICK);
            KillTimer(hWnd, IDT_HOLDCLICK);
            ReleaseCapture();
        break;

        case WM_TIMER:
            /*
             * We run two timers:  the first is the initial delay
             * after the first click before we begin repeating, the
             * second is the repeat rate.
             */
            if (wParam==IDT_FIRSTCLICK)
            {
                KillTimer(hWnd, wParam);
                SetTimer(hWnd, IDT_HOLDCLICK, CTICKS_HOLDCLICK, NULL);
            }

            /*
             * Send a new scroll message if the mouse is still in the
             * originally clicked area.
             */
            if (!StateTest(pMS, MUSTATE_MOUSEOUT))
                PositionChange(hWnd, pMS);

        break;

        case WM_LBUTTONDBLCLK:
        case WM_LBUTTONDOWN:
            /*
             * When we get a mouse down message, we know that the mouse
             * is over the control.  We then do the following steps
             * to set up the new state:
             *  1.  Hit-test the coordinates of the click to
             *      determine in which half the click occurred.
             *  2.  Set the appropriate MUSTATE_*CLICK state
             *      and repaint that clicked half.  This is another
             *      example of a change-state-and-repaint strategy.
             *  3.  Send an initial scroll message.
             *  4.  Set the mouse capture.
             *  5.  Set the initial delay timer before repeating
             *      the scroll message.
             *
             * A WM_LBUTTONDBLCLK message means that the user clicked
             * the control twice in succession which we want to treat
             * like WM_LBUTTONDOWN.  This is safe since we will receive
             * WM_LBUTTONUP before the WM_LBUTTONDBLCLK.
             */

            //Get the mouse coordinates.
            x=(LONG)LOWORD(lParam);
            y=(LONG)HIWORD(lParam);

            /*
             * Only need to hit-test the upper half for a vertical
             * control or the left half for a horizontal control.
             */
            GetClientRect(hWnd, &rect);
            // Okay to use >> to divide here since we know these are positive
            cx=rect.right  >> 1;
            cy=rect.bottom >> 1;

            if (MSS_VERTICAL & pMS->dwStyle)
                wState=(y > cy) ? MUSTATE_DOWNCLICK : MUSTATE_UPCLICK;
            else
                wState=(x > cx) ? MUSTATE_RIGHTCLICK : MUSTATE_LEFTCLICK;

            //Change-state-and-repaint
            StateSet(pMS, wState);
            ClickedRectCalc(hWnd, pMS, &rect);
            InvalidateRect(hWnd, &rect, TRUE);
            UpdateWindow(hWnd);

            PositionChange(hWnd, pMS);
            SetCapture(hWnd);
            SetTimer(hWnd, IDT_FIRSTCLICK, CTICKS_FIRSTCLICK, NULL);
        break;

        case WM_MOUSEMOVE:
            /*
             * On WM_MOUSEMOVE messages we want to know if the mouse
             * has moved out of the control when the control is in
             * a clicked state.  If the control has not been clicked,
             * then we have nothing to do.  Otherwise we want to set
             * the MUSTATE_MOUSEOUT flag and repaint so the button
             * visually comes up.
             */
            if (!StateTest(pMS, MUSTATE_CLICKED))
                break;

            //Get the area we originally clicked and the new POINT
            ClickedRectCalc(hWnd, pMS, &rect);
            pt.x = (LONG)LOWORD(lParam);
            pt.y = (LONG)HIWORD(lParam);

            wState=pMS->wState;

            //Hit-Test the rectange and change the state if necessary.
            if (PtInRect(&rect, pt))
                StateClear(pMS, MUSTATE_MOUSEOUT);
            else
                StateSet(pMS, MUSTATE_MOUSEOUT);

            /*
             * If the state changed, repaint the appropriate part of
             * the control.
             */
            if (wState!=pMS->wState)
            {
                InvalidateRect(hWnd, &rect, TRUE);
                UpdateWindow(hWnd);
            }

        break;

        case WM_LBUTTONUP:
            /*
             * A mouse button up event is much like WM_CANCELMODE since
             * we have to clean out whatever state the control is in:
             *  1.  Kill any repeat timers we might have created.
             *  2.  Release the mouse capture.
             *  3.  Clear the clicked states and repaint, another example
             *      of a change-state-and-repaint strategy.
             */
            KillTimer(hWnd, IDT_FIRSTCLICK);
            KillTimer(hWnd, IDT_HOLDCLICK);

            ReleaseCapture();

            /*
             * Repaint if necessary, only if we are clicked AND the mouse
             * is still in the boundaries of the control.
             */
            if (StateTest(pMS, MUSTATE_CLICKED) &&
                StateTest(pMS, ~MUSTATE_MOUSEOUT))
            {
                //Calculate the rectangle before clearing states.
                ClickedRectCalc(hWnd, pMS, &rect);

                //Clear the states so we repaint properly.
                StateClear(pMS, MUSTATE_MOUSEOUT);
                StateClear(pMS, MUSTATE_CLICKED);

                InvalidateRect(hWnd, &rect, TRUE);
                UpdateWindow(hWnd);
            }

            //Insure that we clear out the states.
        break;

        default:
            return DefWindowProc(hWnd, nMsg, wParam, lParam);
    }

  return ((LRESULT) TRUE);
}


/*
 * ClickedRectCalc
 *
 * Purpose:
 *  Calculates the rectangle of the clicked region based on the
 *  state flags MUSTATE_UPCLICK, MUSTATE_DOWNCLICK, MUSTATE_LEFTCLICK,
 *  and MUSTATE_RIGHTLICK, depending on the style.
 *
 * Parameter:
 *  hWnd            HWND handle to the control window.
 *  lpRect          LPRECT rectangle structure to fill.
 *
 * Return Value:
 *  void
 *
 */

void PASCAL ClickedRectCalc(HWND hWnd, PMUSCROLL pMS, LPRECT lpRect)
{
    LONG       cx, cy;

    GetClientRect(hWnd, lpRect);
    // Okay to use >> to divide here since we know these are positive
    cx=lpRect->right  >> 1;
    cy=lpRect->bottom >> 1;

    if (MSS_VERTICAL & pMS->dwStyle)
    {
        if (StateTest(pMS, MUSTATE_DOWNCLICK))
            lpRect->top=cy;

        if (StateTest(pMS, MUSTATE_UPCLICK))
            lpRect->bottom=1+cy;
    }
    else
    {
        //MSS_HORIZONTAL
        if (StateTest(pMS, MUSTATE_RIGHTCLICK))
            lpRect->left=cx;

        if (StateTest(pMS, MUSTATE_LEFTCLICK))
            lpRect->right=1+cx;
    }

    return;
}


/*
 * PositionChange
 *
 * Purpose:
 *  Checks what part of the control is clicked, modifies the current
 *  position accordingly (taking MSS_INVERTRANGE into account) and
 *  sends an appropriate message to the associate.  For MSS_VERTICAL
 *  controls we send WM_VSCROLL messages and for MSS_HORIZONTAL controls
 *  we send WM_HSCROLL.
 *
 *  The scroll code in the message is always SB_LINEUP for the upper
 *  or left half of the control (vertical and horizontal, respectively)
 *  and SB_LINEDOWN for the bottom or right half.
 *
 *  This function does not send a message if the position is pegged
 *  on the minimum or maximum of the range if MSS_NOPEGSCROLL is
 *  set in the style bits.
 *
 * Parameters:
 *  hWnd            HWND of the control.
 *  pMS             PMUSCROLL pointer to control data structure.
 *
 * Return Value:
 *  void
 */

void PASCAL PositionChange(HWND hWnd, PMUSCROLL pMS)
{
    UINT           msgScroll;
    WORD           wScrollCode;
    BOOL           fPegged=FALSE;

    if (StateTest(pMS, MUSTATE_UPCLICK | MUSTATE_LEFTCLICK))
        wScrollCode=SB_LINEUP;

    if (StateTest(pMS, MUSTATE_DOWNCLICK | MUSTATE_RIGHTCLICK))
        wScrollCode=SB_LINEDOWN;

    msgScroll=(MSS_VERTICAL & pMS->dwStyle) ? WM_VSCROLL : WM_HSCROLL;

    /*
     * Modify the current position according to the following rules:
     *
     * 1. On SB_LINEUP with an inverted range, increment the position.
     *    If the position is already at the maximum, set the pegged flag.
     *
     * 2. On SB_LINEUP with an normal range, decrement the position.
     *    If the position is already at the minimum, set the pegged flag.
     *
     * 3. On SB_LINEDOWN with an inverted range, treat like SB_LINEUP
     *    with a normal range.
     *
     * 4. On SB_LINEDOWN with an normal range, treat like SB_LINEUP
     *    with an inverted range.
     */

    if (wScrollCode==SB_LINEUP)
    {
        if (MSS_INVERTRANGE & pMS->dwStyle)
        {
            if (pMS->nPos==pMS->nMax)
                fPegged=TRUE;
            else
                pMS->nPos++;
        }
        else
        {
            if (pMS->nPos==pMS->nMin)
                fPegged=TRUE;
            else
                pMS->nPos--;
        }
    }
    else
    {
        if (MSS_INVERTRANGE & pMS->dwStyle)
        {
            if (pMS->nPos==pMS->nMin)
                fPegged=TRUE;
            else
                pMS->nPos--;
        }
        else
        {
            if (pMS->nPos==pMS->nMax)
                fPegged=TRUE;
            else
                pMS->nPos++;
        }
    }

    /*
     * Send a message if we changed and are not pegged, or did not change
     * and MSS_NOPEGSCROLL is clear.
     */
    if (!fPegged || !(MSS_NOPEGSCROLL & pMS->dwStyle))
    {
        // Note that, like normal WM_VSCROLL messages, we can only pass a 16-bit position
        SendMessage( pMS->hWndAssociate, msgScroll,
                     MAKEWPARAM(wScrollCode,(WORD)(SHORT)pMS->nPos), (LPARAM)hWnd );
    }

    return;
}




/******************************************************************************\
*
*  FUNCTION:    MuScrollStyle
*
*  INPUTS:      hWndParent - handle of parent window (dialog editor)
*               pccs       - pointer to a CCSTYLE structure
*
*  RETURNS:     TRUE  if success,
*               FALSE if error occured
*
*  LOCAL VARS:  rc - return code from DialogBox
*
\******************************************************************************/

BOOL CALLBACK MuScrl32Style (HWND hWndParent, LPCCSTYLE pccs)
{
  int rc;

  // Initialize global pointer to LPCCSTYLE structure for use by dialogproc
  gpccs = pccs;

  if ((rc = DialogBox (ghMod, MAKEINTRESOURCE(IDD_STYLEDIALOG), hWndParent,
                       (DLGPROC)MuScrl32DlgProc)) == -1)
  {
    MessageBox (hWndParent, (LPCTSTR) "MuScrl32Style(): DialogBox failed",
                (LPCTSTR) "Err!- MUSCRL32.DLL",
                MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
    rc = 0;
  }

  return (BOOL) rc;
}


/******************************************************************************\
*
*  FUNCTION:    MuScrl32DlgProc (standard dialog procedure INPUTS/RETURNS)
*
*  COMMENTS:    This dialog comes up in response to a user requesting to
*               modify the control style. This sample allows for changing
*               the control's text, and this is done by modifying the
*               CCSTYLE structure pointed at by "gpccs" (a pointer
*               that was passed to us by dlgedit).
*
\******************************************************************************/

LRESULT CALLBACK MuScrl32DlgProc (HWND hDlg, UINT nMsg, WPARAM wParam,
                                  LPARAM lParam)
{
    DWORD           dwStyle;

    switch (nMsg)
    {
        case WM_INITDIALOG:

            if (gpccs==NULL)
                return FALSE;

            SetDlgItemText(hDlg, ID_TEXTEDIT, gpccs->szText);

            dwStyle=gpccs->flStyle;

            // Brute force setting of dialog controls
            if (dwStyle & MSS_VERTICAL)
                CheckRadioButton(hDlg, ID_RADIOVERTICAL,
                                 ID_RADIOHORIZONTAL, ID_RADIOVERTICAL);

            if (dwStyle & MSS_HORIZONTAL)
                CheckRadioButton(hDlg, ID_RADIOVERTICAL,
                                 ID_RADIOHORIZONTAL, ID_RADIOHORIZONTAL);

            if (dwStyle & MSS_TEXTHASRANGE)
                CheckDlgButton(hDlg, ID_CHECKTEXTHASRANGE, 1);

            if (dwStyle & MSS_NOPEGSCROLL)
                CheckDlgButton(hDlg, ID_CHECKNOPEGSCROLL, 1);

            if (dwStyle & MSS_INVERTRANGE)
                CheckDlgButton(hDlg, ID_CHECKINVERTRANGE, 1);

        break;

        case WM_COMMAND:
            switch (LOWORD(wParam))     // ID of control
            {
                /*
                 * Check for valid range text in the Text field if the
                 * Text Has Range box is checked.
                 */

                case ID_CHECKTEXTHASRANGE:
                    if (HIWORD(wParam)==BN_CLICKED)
                    {
                        if (IsDlgButtonChecked(hDlg, wParam))
                            FRangePositionCheck(hDlg);
                    }
                break;

                case IDOK:
                    if (IsDlgButtonChecked(hDlg, ID_CHECKTEXTHASRANGE))
                    {
                        //Verify the range text again.
                        if (!FRangePositionCheck(hDlg))
                            return TRUE;
                    }

                    //Save the Style and text.
                    dwStyle=gpccs->flStyle;     // Need to get the current style first and pass it
                    dwStyle=DWFormStyleFlags( hDlg, dwStyle );
                    gpccs->flStyle = dwStyle;

                    GetDlgItemText(hDlg, ID_TEXTEDIT, gpccs->szText, CCHCCTEXT);

                    EndDialog(hDlg, TRUE);

                break;

                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                break;

                default:
                    break;
            }
        break;

        default:
            return FALSE;
    }

    return TRUE;
}


/*
 * DWFormStyleFlags
 *
 * Purpose:
 *  Returns a DWORD with flags set to whatever options are checked in
 *  the dialog box.  It is up to the implementer of the dialog box to
 *  code the exact bit fields in this function.
 *
 * Parameters:
 *  hDlg        HWND of the style dialog.
 *  dwStyle     DWORD with original style passed in.
 *
 * Return Value:
 *  DWORD        Style Flags value.
 */

DWORD PASCAL DWFormStyleFlags( HWND hDlg, DWORD dwStyle )
{
    // Turn off all custom styles to start with
    dwStyle &= ~( MSS_VERTICAL | MSS_HORIZONTAL | MSS_TEXTHASRANGE 
                   | MSS_NOPEGSCROLL | MSS_INVERTRANGE );

    // With custom styles cleaned, set whichever have been set in dlg
    if (IsDlgButtonChecked(hDlg, ID_RADIOVERTICAL))
        dwStyle |= MSS_VERTICAL;
    else
        dwStyle |= MSS_HORIZONTAL;

    if (IsDlgButtonChecked(hDlg, ID_CHECKTEXTHASRANGE))
        dwStyle |= MSS_TEXTHASRANGE;

    if (IsDlgButtonChecked(hDlg, ID_CHECKNOPEGSCROLL))
        dwStyle |= MSS_NOPEGSCROLL;

    if (IsDlgButtonChecked(hDlg, ID_CHECKINVERTRANGE))
        dwStyle |= MSS_INVERTRANGE;

    return dwStyle;
}


/*
 * FRangePositionCheck
 *
 * Purpose:
 *  Checks if the control text contains a valid string and that
 *  the initial position is within the given range.
 *
 * Parameters:
 *  hDlg            HWND of the style dialog.
 *
 * Return Value:
 *  BOOL            TRUE if the text is valid, FALSE otherwise.
 *
 */

BOOL PASCAL FRangePositionCheck(HWND hDlg)
{
    HWND        hEdit;
    char        szTemp1[CCHCCTEXT];
    char        szTemp2[CCHCCTEXT];
    BOOL        fTextOK;
    INT       nMin;
    INT       nMax;
    INT       nPos;

    /*
     * We checked the MSS_TEXTHASRANGE box, so verify that there
     * is valid text in the Text edit control.  If not, just
     * cue the user with a beep and a SetFocus.
     */
    hEdit=GetDlgItem(hDlg, ID_TEXTEDIT);
    GetWindowText(hEdit, szTemp1, CCHCCTEXT);

    fTextOK=FTextParse(szTemp1, &nMin, &nMax, &nPos);

    if (fTextOK && (nPos < nMin || nPos > nMax))
    {
        LoadString(ghMod, IDS_CLASSNAME,  szTemp1, 60);
        LoadString(ghMod, IDS_RANGEERROR, szTemp2, 60);

        MessageBox(hDlg, szTemp2, szTemp1, MB_OK | MB_ICONEXCLAMATION);
        SetFocus(hEdit);
        SendMessage(hEdit, EM_SETSEL, 0, -1);
        fTextOK = FALSE;
    }

    if (!fTextOK)
    {
        MessageBeep(0);
        SetFocus(hEdit);
        SendMessage(hEdit, EM_SETSEL, 0, -1);
    }

    return fTextOK;
}
