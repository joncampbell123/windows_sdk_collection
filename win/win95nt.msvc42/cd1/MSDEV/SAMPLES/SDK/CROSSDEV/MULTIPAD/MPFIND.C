
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples. 
*       Copyright (C) 1993-1995 Microsoft Corporation.
*       All rights reserved. 
*       This source code is only intended as a supplement to 
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the 
*       Microsoft samples programs.
\******************************************************************************/

/***************************************************************************
 *                                                                         *
 *  MODULE      : MpFind.c                                                 *
 *                                                                         *
 *  PURPOSE     : Code to do text searches in MultiPad.                    *
 *                                                                         *
 *  FUNCTIONS   : RealSlowCompare () - Compares subject string with target *
 *                                     string.                             *
 *                                                                         *
 *                Local_FindText ()  - Looks for the search string in the  *
 *                                     active window.                      *
 *                                                                         *
 *                FindPrev ()        - Find previous occurence of search   *
 *                                     string.                             *
 *                                                                         *
 *                FindNext ()        - Find next occurence of search string*
 *                                                                         *
 *                FindDlgProc ()     - Dialog function for Search/Find.    *
 *                                                                         *
 *                Find ()            - Invokes FindDlgProc ()              *
 *                                                                         *
 ***************************************************************************/
#include "stdwin.h"
#include "multipad.h"

BOOL fCase         = FALSE;    /* Turn case sensitivity off */
CHAR szSearch[160] = "";       /* Initialize search string  */


/****************************************************************************
 *                                                                          *
 *  FUNCTION   : RealSlowCompare ()                                         *
 *                                                                          *
 *  PURPOSE    : Compares subject string with the target string. This fn/   *
 *               is called repeatedly so that all substrings are compared,  *
 *               which makes it O(n ** 2), hence it's name.                 *
 *                                                                          *
 *  RETURNS    : TRUE  - If pSubject is identical to pTarget.               *
 *               FALSE - otherwise.                                         *
 *                                                                          *
 ****************************************************************************/

BOOL RealSlowCompare
(
	register PSTR pSubject,
    register PSTR pTarget
)
{
    if (fCase)
    {
        while (*pTarget)
            if (*pTarget++ != *pSubject++)
                return FALSE;
    }
    else
    {
        /* If case-insensitive, convert both subject and target to lowercase
         * before comparing.
         */
        AnsiLower ((LPSTR)pTarget);
        while (*pTarget)
            if (*pTarget++ != (CHAR)(DWORD)AnsiLower ((LPSTR)(DWORD)(BYTE)*pSubject++))
                return FALSE;
    }
    return TRUE;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : Local_FindText ()                                                  *
 *                                                                          *
 *  PURPOSE    : Finds the search string in the active window according to  *
 *               search direction (dch) specified ( -1 for reverse and 1 for*
 *               forward searches).                                         *
 *                                                                          *
 ****************************************************************************/
VOID Local_FindText( register INT dch)
{
    register PSTR   pText;
    HANDLE          hT;
    WORD            cch;
    INT             i;
    UINT            wStart;
    UINT            wEnd;

    if (!*szSearch)
        return;

    /* Find the current selection range */
#if defined(WIN32) || defined(_MAC)
    SendMessage(hwndActiveEdit, EM_GETSEL, (WPARAM) &wStart, (LPARAM) &wEnd);
#else
    LONG	l;
    l = (LONG)SendMessage(hwndActiveEdit, EM_GETSEL, 0, 0);
    wStart = LOWORD(l);
    wEnd = HIWORD(l);
#endif

    /* Get the handle to the text buffer and lock it */
    hT    = (HANDLE)SendMessage (hwndActiveEdit, EM_GETHANDLE, 0, 0L);
    pText = LocalLock(hT);

    /* Get the length of the text */
    cch = (WORD)SendMessage (hwndActiveEdit, WM_GETTEXTLENGTH, 0, 0L);

    // Determine the first character in the text to begin the search and 
	// compute how many characters are before/after the current selection.
    if (dch < 0)
    {
        i = wStart;
		--wStart;
		pText += wStart; 
	}
    else
    {
        if (wStart != wEnd)
            ++wStart;
        pText += wStart;
        i = cch - wStart + 1 - lstrlen (szSearch);
	}

    /* While there are uncompared substrings... */
	for (; i > 0; --i)
    {

        /* Does this substring match? */
        if (RealSlowCompare(pText,szSearch))
        {

            /* Yes, unlock the buffer.*/
            LocalUnlock(hT);

            /* Select the located string */
            wEnd = wStart + lstrlen(szSearch);
            SendMessage(hwndActiveEdit, EM_SETSEL, GET_EM_SETSEL_MPS(wStart, wEnd));
            return;
        }

        /* increment/decrement the start and text position by 1 */
		wStart += dch;
		pText += dch;
    }

    /* Not found... unlock buffer. */
    LocalUnlock (hT);

    /* Give a message */
    MPError (GetActiveWindow(), MB_OK | MB_ICONEXCLAMATION, IDS_CANTFIND, (LPSTR)szSearch);

    return;
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : FindPrev ()                                                *
 *                                                                          *
 *  PURPOSE    : Finds the previous occurence of the search string. Calls   *
 *               Local_FindText () with a negative search direction.                *
 *                                                                          *
 ****************************************************************************/
VOID FindPrev()
{
    Local_FindText(-1);
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : FindNext ()                                                *
 *                                                                          *
 *  PURPOSE    : Finds the next occurence of search string. Calls           *
 *               Local_FindText () with a positive search direction.                *
 *                                                                          *
 ****************************************************************************/
VOID FindNext()
{
    Local_FindText(1);
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : FindDlgProc(hwnd, message, wParam, lParam)                 *
 *                                                                          *
 *  PURPOSE    : Dialog function for the Search/Find command. Prompts user  *
 *               for target string, case flag and search direction.         *
 *                                                                          *
 ****************************************************************************/

BOOL CALLBACK FindDlgProc
(
    HWND    hwnd,
    UINT    msg,
    WPARAM  wParam,
    LPARAM  lParam
)

{
    switch (msg)
    {
        case WM_INITDIALOG:
        {

            /* Check/uncheck case button */
            CheckDlgButton (hwnd, (int)IDD_CASE, (WORD)fCase);

            /* Set default search string to most recently searched string */
            SetDlgItemText (hwnd, IDD_SEARCH, szSearch);

            /* Allow search only if target is nonempty */
            if (!lstrlen (szSearch)){
                EnableWindow (GetDlgItem (hwnd, IDOK), FALSE);
                EnableWindow (GetDlgItem (hwnd, IDD_PREV), FALSE);
            }
            break;
        }
		
        case WM_COMMAND:
        {

            /* Search forward by default (see IDOK below) */
            INT i = 1;

            switch (GET_WM_COMMAND_ID(wParam, lParam))
            {
                /* if the search target becomes non-empty, enable searching */
                case IDD_SEARCH:
                    if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE)
                    {
                        if (!(WORD) SendDlgItemMessage (hwnd,
                                                        IDD_SEARCH,
                                                        WM_GETTEXTLENGTH,
                                                        0,
                                                        0L))
                            i = FALSE;
                        else
                            i = TRUE;
                        EnableWindow (GetDlgItem (hwnd, IDOK), i);
                        EnableWindow (GetDlgItem (hwnd, IDD_PREV), i);
                    }
                    break;

                case IDD_CASE:
                    /* Toggle state of case button */
                    CheckDlgButton (hwnd,
                                    (int)IDD_CASE,
                                    (WORD)!IsDlgButtonChecked (hwnd, (int)IDD_CASE));
                    break;

                case IDD_PREV:
                    /* Set direction to backwards */
                    i=-1;
                    /*** FALL THRU ***/

                case IDOK:
                    /* Save case selection */
                    fCase = IsDlgButtonChecked( hwnd, IDD_CASE);

                    /* Get search string */
                    GetDlgItemText (hwnd, IDD_SEARCH, szSearch, sizeof (szSearch));

                    /* Find the text */
                    Local_FindText (i);
                    /*** FALL THRU ***/

                /* End the dialog */
                case IDCANCEL:
                    EndDialog (hwnd, 0);
                    break;

                default:
                    return FALSE;
            }
            break;
        }
		break;

    	default:
        	return FALSE;
    }

    return TRUE;
        UNREFERENCED_PARAMETER(lParam);
}

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : Find()                                                     *
 *                                                                          *
 *  PURPOSE    : Invokes the Search/Find dialog.                            *
 *                                                                          *
 ****************************************************************************/
VOID Find()
{
    DialogBox (hInst, IDD_FIND, GetActiveWindow(), (DLGPROC) FindDlgProc);
}
