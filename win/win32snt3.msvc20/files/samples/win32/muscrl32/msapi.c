/*
 * MSAPI.C
 *
 * Contains the API implementation of the MicroScroll custom
 * control, including functional messages and function message
 * equivalents.
 *
 * Version 1.1, October 1991, Kraig Brockschmidt
 *
 * Win32 & Win32 control format port, April 1994, Tarn Faulkner
 */


#include <windows.h>
#include "mscrdll.h"

/*
 * LMicroScrollAPI
 *
 * Purpose:
 *  Processes any control-specific function messages for the
 *  MicroScroll control.
 *
 * Parameters:
 *  hWnd            HWND handle to the control window.
 *  nMsg            WORD message identifier.
 *  wParam          WORD parameter of the message.
 *  lParam          LONG parameter of the message.
 *  pMS             PMUSCROLL pointer to control-specific data.
 *
 * Return Value:
 *  LONG            Varies with the message.
 *
 */

LONG PASCAL LMicroScrollAPI(HWND hWnd, UINT nMsg, WPARAM wParam,
                            LONG lParam, PMUSCROLL pMS)
{
    DWORD           dwT;
    COLORREF        cr;
    HWND            hWndT;
    INT             nMin, nMax;
    INT             nPos;
    WORD            wID;

    switch (nMsg)
    {
        case MSM_HWNDASSOCIATESET:
            //Change the associate window of this control.
            if (!IsWindow((HWND)wParam))
                return -1;

            //Save old window handle.
            hWndT=pMS->hWndAssociate;

            //Get our ID value, then send WM_COMMAND notifications.
            wID=(WORD)GetWindowLong(hWnd, GWL_ID);

            SendMessage(hWndT, WM_COMMAND, 
                    MAKEWPARAM(wID,MSN_ASSOCIATELOSS), (LPARAM)hWnd );

            pMS->hWndAssociate=(HWND)wParam;

            SendMessage(pMS->hWndAssociate, WM_COMMAND,
                    MAKEWPARAM(wID,MSN_ASSOCIATEGAIN), (LPARAM)hWnd );

            return (LONG)hWndT;

        case MSM_HWNDASSOCIATEGET:
            return (LONG)pMS->hWndAssociate;

        // MSM_DWRANGESET
        // nMin = (INT)wParam 
        // nMax = (INT)lParam
        // return new nPos if changed, else 0
        case MSM_DWRANGESET:
            /*
             * Set the new range, sending the appropriate notifications.
             * Also send a scroll message if the position has to change.
             * If the minimum is greater than the max, return error.
             */
            if (LOWORD(lParam) >= HIWORD(lParam))
                return -1L;

            wID=(WORD)GetWindowLong(hWnd, GWL_ID);

            SendMessage(pMS->hWndAssociate, WM_COMMAND,
                    MAKEWPARAM(wID,MSN_RANGECHANGE), (LPARAM)hWnd );

            //Save old values.
            nMin = pMS->nMin;
            nMax = pMS->nMax;

            pMS->nMin = (INT)wParam;
            pMS->nMax = (INT)lParam;

            /*
             * If current position is outside of new range, force it to
             * the average of the range, otherwise leave it be.
             */
            nPos = pMS->nPos;
            if ((pMS->nMin >= nPos) || (pMS->nMax <= nPos))
            {
                pMS->nPos = (pMS->nMin + pMS->nMax) / 2;

                //Send a scroll message if we change position.
                nMsg = (MSS_VERTICAL & pMS->dwStyle) ? WM_VSCROLL : WM_HSCROLL;
                // *** Note that, like any WM_VSCROLL, we can only give a 16-bit position
                // *** See WM_VSCROLL docs for workarounds to use for 32-bit values
                wParam = MAKEWPARAM( SB_THUMBTRACK, (WORD)(SHORT)pMS->nPos );
                lParam = (LPARAM)hWnd;

                SendMessage(pMS->hWndAssociate, nMsg, wParam, lParam);
            }

            //Return new position if changed, else 0
            return (LRESULT)( (nPos == pMS->nPos) ? 0 : nPos );

        // MSM_DWRANGEGET
        // nMin = (INT*)wParam 
        // nMax = (INT*)lParam
        // return 0
        case MSM_DWRANGEGET:

            *((INT*)wParam) = pMS->nMin;
            *((INT*)lParam) = pMS->nMax;
            return (LRESULT)0;

        // MSM_WCURRENTPOSSET
        // nPos = (INT)wParam
        // return previous nPos if success, -1 if invalid pos
        case MSM_WCURRENTPOSSET:
            /*
             * Set the new position if it falls within the valid range,
             * sending the appropriate scroll message.
             */

            //Save current position
            nPos=pMS->nPos;

            if ((pMS->nMin <= (INT)wParam) && (pMS->nMax >= (INT)wParam))
            {
                pMS->nPos = (INT)wParam;
                nMsg=(MSS_VERTICAL & pMS->dwStyle) ? WM_VSCROLL : WM_HSCROLL;
                // *** Note that, like any WM_VSCROLL, we can only give a 16-bit position
                // *** See WM_VSCROLL docs for workarounds to use for 32-bit values
                wParam = MAKEWPARAM( SB_THUMBTRACK, pMS->nPos );
                lParam = (LPARAM)hWnd;

                SendMessage(pMS->hWndAssociate, nMsg, wParam, lParam);

                //Return old position.
                return (LRESULT)nPos;
            }

            //Invalid position.
            return (LRESULT)-1;

        case MSM_WCURRENTPOSGET:
            return (LRESULT)(pMS->nPos);

        // MSM_FNOPEGSCROLLSET
        // bSet_MSS_NOPEGSCROLL = (BOOL)wParam
        // return TRUE if style had previously been set, else FALSE
        case MSM_FNOPEGSCROLLSET:
            /*
             * Set or clear the MSS_NOPEGSCROLL style depending on 
             * wParam being TRUE or FALSE
             */
            dwT=pMS->dwStyle & MSS_NOPEGSCROLL;

            //Either set of clear the style.
            if ((BOOL)wParam)
                pMS->dwStyle |= MSS_NOPEGSCROLL;
            else
                pMS->dwStyle &= ~MSS_NOPEGSCROLL;

            //Return TRUE or FALSE if the bit was or wasn't set
            return (LRESULT)(dwT ? 1L : 0L);

        case MSM_FNOPEGSCROLLGET:
            return (LRESULT)(pMS->dwStyle & MSS_NOPEGSCROLL);

        // MSM_FINVERTRANGESET
        // bSet_MSM_FINVERTRANGESET = (BOOL)wParam
        // return TRUE if style had previously been set, else FALSE
        case MSM_FINVERTRANGESET:
            /*
             * Set or clear the MSS_INVERTRANGE style depending on 
             * wParam being TRUE or FALSE
             */
            dwT=pMS->dwStyle & MSS_INVERTRANGE;

            //Either set of clear the style.
            if ((BOOL)wParam)
                pMS->dwStyle |= MSS_INVERTRANGE;
            else
                pMS->dwStyle &= ~MSS_INVERTRANGE;

            //Return TRUE or FALSE if the bit was or wasn't set
            return (LRESULT)(dwT ? 1L : 0L);

        case MSM_FINVERTRANGEGET:
            return (pMS->dwStyle & MSS_INVERTRANGE);

        // MSM_CRCOLORSET
        // nColorPos = (INT)wParam
        // rgClrToSet = (COLORREF)lParam
        // return COLORREF previously set
        case MSM_CRCOLORSET:
            if (wParam >= CCOLORS)
                return 0L;

            cr=pMS->rgCr[wParam];

            //If -1 is set in rgCr the paint procedure uses a default.
            pMS->rgCr[wParam]=(COLORREF)lParam;

            //Force repaint since we changed a state.
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);

            //Return the old color.
            return (LRESULT)cr;

        case MSM_CRCOLORGET:
            if (wParam >= CCOLORS)
                return 0L;

            return (LRESULT)pMS->rgCr[wParam];
    }
    return 0L;
}


/*
 * Message API Functions
 *
 * The advantage of using a function instead of SendMessage is that
 * you get type checking on the parameters and the return value.'
 *
 * Header comments are provided on these functions in pairs.  All
 * functions take hWnd (control handle), and the Set functions
 * usually take an extra paarameter containing the new value.
 *
 */


/*
 * MSHwndAssociateSet
 * MSHwndAssociateGet
 *
 * Purpose:
 *  Change or retrieve the associate window of the control.
 *
 * Parameters:
 *  hWnd            HWND of the control window.
 *
 * Set Parameters:
 *  hWndAssociate   HWND of new associate.
 *
 * Return Value:
 *  HWND            Handle of previous associate (set) or current
 *                  associate (set).
 */

HWND CALLBACK MSHwndAssociateSet(HWND hWnd, HWND hWndAssociate)
{
    return (HWND)SendMessage(hWnd, MSM_HWNDASSOCIATESET,
                             (WPARAM)hWndAssociate, (LPARAM)0);
}

HWND CALLBACK MSHwndAssociateGet(HWND hWnd)
{
    return (HWND)SendMessage(hWnd, MSM_HWNDASSOCIATEGET, 0, 0L);
}


/*
 * MSRangeSet
 *
 * Purpose:
 *  Change the range of the control.
 *
 * Parameters:
 *  hWnd            HWND of the control window.
 *
 * Set Parameters:
 *  nMin            INT new minimum of the range.
 *  nMax            INT new maximum of the range.
 *
 * Return Value:
 *  INT             0 if position didn't need to be changed,
 *                  else new position.
 */

INT CALLBACK MSDwRangeSet(HWND hWnd, INT nMin, INT nMax)
{
    return (INT)SendMessage(hWnd, MSM_DWRANGESET, (WPARAM)nMin, (WPARAM)nMax);
}


/*
 * MSRangeGet
 *
 * Purpose:
 *  Change or retrieve the range of the control.
 *
 * Parameters:
 *  hWnd            HWND of the control window.
 *
 * Get Parameters:
 *  &pNMin           INT* to hold new minimum of the range.
 *  &pNMax           INT* to hold new maximum of the range.
 *
 * Return Value:    none
 */
 
void CALLBACK MSDwRangeGet(HWND hWnd, INT * pNMin, INT * pNMax)
{
    SendMessage(hWnd, MSM_DWRANGEGET, (WPARAM)pNMin, (LPARAM)pNMax);
}


/*
 * MSWCurrentPosSet
 * MSWCurrentPosGet
 *
 * Purpose:
 *  Change or retrieve the current position of the control.
 *
 * Parameters:
 *  hWnd            HWND of the control window.
 *
 * Set Parameters:
 *  nPos            INT new position to set.
 *
 * Return Value:
 *  INT            Previous (set) or current (get) position.
 *
 */

INT CALLBACK MSWCurrentPosSet(HWND hWnd, INT nPos)
{
    return (INT)SendMessage(hWnd, MSM_WCURRENTPOSSET, (WPARAM)nPos, 0L);
}

INT CALLBACK MSWCurrentPosGet(HWND hWnd)
{
    return (INT)SendMessage(hWnd, MSM_WCURRENTPOSGET, 0, 0L);
}


/*
 * MSFNoPegScrollSet
 * MSFNoPegScrollGet
 *
 * Purpose:
 *  Change or retrieve the state of the MSS_NOPEGSCROLL style bit.
 *
 * Parameters:
 *  hWnd            HWND of the control window.
 *
 * Set Parameters:
 *  fNoPegScroll    BOOL flag to set (TRUE) or clear (FALSE) the style.
 *
 * Return Value:
 *  BOOL            Previous (set) or current (get) state of this
 *                  style bit, either TRUE for on, FALSE for off.
 */

BOOL CALLBACK MSFNoPegScrollSet(HWND hWnd, BOOL fNoPegScroll)
{
    return (BOOL)SendMessage(hWnd, MSM_FNOPEGSCROLLSET, (WPARAM)fNoPegScroll, 0L);
}

BOOL CALLBACK MSFNoPegScrollGet(HWND hWnd)
{
    return (BOOL)SendMessage(hWnd, MSM_FNOPEGSCROLLGET, 0, 0L);
}


/*
 * MSFInvertRangeSet
 * MSFInvertRangeGet
 *
 * Purpose:
 *  Change or retrieve the state of the MSS_INVERTRANGE style bit.
 *
 * Parameters:
 *  hWnd            HWND of the control window.
 *
 * Set Parameters:
 *  fInvertRange    BOOL flag to set (TRUE) or clear (FALSE) the style.
 *
 * Return Value:
 *  BOOL            Previous (set) or current (get) state of this
 *                  style bit, either TRUE for on, FALSE for off.
 */

BOOL CALLBACK MSFInvertRangeSet(HWND hWnd, BOOL fInvertRange)
{
    return (BOOL)SendMessage(hWnd, MSM_FINVERTRANGESET, (WPARAM)fInvertRange, 0L);
}

BOOL CALLBACK MSFInvertRangeGet(HWND hWnd)
{
    return (BOOL)SendMessage(hWnd, MSM_FINVERTRANGEGET, 0, 0L);
}


/*
 * MSCrColorSet
 * MSCrColorGet
 *
 * Purpose:
 *  Change or retrieve a configurable color.
 *
 * Parameters:
 *  hWnd            HWND of the control window.
 *  nColor          UINT index to the control to modify or retrieve.
 *
 * Set Parameters:
 *  cr              COLORREF new value of the color.
 *
 * Return Value:
 *  COLORREF        Previous (set) or current (get) color value.
 *                  Zero for invalid nColor values.
 *
 */

COLORREF CALLBACK MSCrColorSet(HWND hWnd, UINT nColor, COLORREF cr)
{
    return (COLORREF)SendMessage(hWnd, MSM_CRCOLORSET, (WPARAM)nColor, (LPARAM)cr);
}

COLORREF CALLBACK MSCrColorGet(HWND hWnd, UINT nColor)
{
    return (COLORREF)SendMessage(hWnd, MSM_CRCOLORGET, (WPARAM)nColor, (LPARAM)0L);
}
