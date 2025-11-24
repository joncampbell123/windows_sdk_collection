/*
 -  T B L V I E W . C P P
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Contains the implementation of a generic IMAPITable viewer.
 *      This viewer is contained in a DLL and is accessed by the user
 *      throught a single entry point: ViewMapiTable(LPMAPITABLE FAR *, HWND).
 *      The UI is presented through 3 main dialogs: CTblDlg, CSortDlg,
 *      and CResDlg.  These are MFC 2.0 based dialog classes.
 *
 */


#ifdef WIN32
#ifdef CHICAGO
#define _INC_OLE
#endif
#define INC_OLE2
#define INC_RPC
#endif

#include <afxwin.h>     
#include <windowsx.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN16
#include <compobj.h>
#endif

#ifdef WIN32
#include <objbase.h>
#include <objerror.h>
#ifdef CHICAGO
#include <ole2.h>
#endif
#endif


#include <mapiwin.h>
#include <mapix.h>
#include <strtbl.h>
#include <misctool.h>
#include <pvalloc.h>
#include <tventry.h>
#include <limits.h>
#include "resource.h"
#include "tblview.h"


/* Global Objects */
CWnd        *pStatusWnd;
CWnd        *pLogWnd;
HRESULT     hError          = 0;
ULONG       ulNumResTypes   = 0;        // determine number of restriction types

/*
 -  ViewMapiTable
 -
 *  Purpose:
 *      Main entry point for table viewer.
 *
 *  Parameters:
 *      lpTable         - Pointer to an IMAPITable
 *
 *  Returns:
 *      void.
 *
 */

extern "C"
BOOL ViewMapiTable(LPMAPITABLE FAR *lppMAPITable, HWND hWnd)
{
    CTblDlg     tblDlg(*lppMAPITable, hWnd);
    return (tblDlg.DoModal() == IDOK);
}


/* CTblDlg implementation */

BEGIN_MESSAGE_MAP(CTblDlg, CModalDialog)
    ON_LBN_SELCHANGE(IDC_DISPLAYVALUE, OnSelDisplayValue)
    ON_COMMAND(IDC_SETBOOKMARK,        OnSetBookmark)
    ON_COMMAND(IDC_FORCESORT,          OnForceSort)
    ON_COMMAND(IDC_FORCERES,           OnForceRestriction)
    ON_COMMAND(IDC_GETSTATUS,          OnGetStatus)
    ON_COMMAND(IDC_QUERYPOS,           OnQueryPosition)
    ON_COMMAND(IDC_QUERYSORTORDER,     OnQuerySortOrder)
    ON_COMMAND(IDC_GETROWCOUNT,        OnGetRowCount)
    ON_COMMAND(IDC_GETLASTERROR,       OnGetLastError)
    ON_COMMAND(IDC_ABORT,              OnAbort)
    ON_COMMAND(IDC_FIND,               OnFind)
    ON_COMMAND(IDC_FREEBOOKMARK,       OnFreeBookmark)
    ON_COMMAND(IDC_RESTRICTION,        OnRestriction)
    ON_COMMAND(IDC_SEEK,               OnSeek)
    ON_COMMAND(IDC_SETCOLUMNS,         OnSetColumns)
    ON_COMMAND(IDC_SORTORDER,          OnSortOrder)
    ON_COMMAND(IDC_CLOSE,              OnClose)
    ON_COMMAND(IDC_CLEARSTATUS,        OnClearStatus)
    ON_COMMAND(IDC_CLEARMAPILOG,       OnClearMapiLog)
END_MESSAGE_MAP()


/*
 -  CTblDlg::
 -  CTblDlg
 -
 *  Purpose:
 *      Constructor for main dialog class.
 *
 */

CTblDlg::CTblDlg(LPMAPITABLE lpTbl, HWND hWnd)
    : CModalDialog(IDD_TBLDLG, FromHandle(hWnd))
{
    m_lpTable           = lpTbl;
    m_lpSort            = NULL;
    m_lpRes             = NULL;
    m_cRows             = 0;
    m_ulptDisplay       = PR_DISPLAY_NAME;
    m_BookMarks.cValues = 0;
}

/*------------------------------------*/
/* Message Handlers for CTblDlg class */
/*------------------------------------*/

/*
 -  CTblDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Constructor for main dialog class.
 *
 */

BOOL CTblDlg::OnInitDialog()
{
    pStatusWnd = GetDlgItem(IDC_STATUS);
    pLogWnd = GetDlgItem(IDC_MAPILOG);

    RenderTable();

    GetDlgItem(IDC_DISPLAYVALUE)->SetFocus();
    SendDlgItemMessage(IDC_DISPLAYVALUE, LB_SETCURSEL, 0, 0L);

    OnSelDisplayValue();

    SendDlgItemMessage(IDC_MAPILOG, LB_SETHORIZONTALEXTENT,
            (WPARAM)VALUES_LB_HOR_SIZE, 0);

    return TRUE;
}


/*
 -  CTblDlg::
 -  OnSelDisplayValue
 -
 *  Purpose:
 *      Displays the table contents in the right pane based upon the
 *      current row in the left pane.
 *
 */

void CTblDlg::OnSelDisplayValue()
{
    DWORD       idxCurSel;
    SCODE       sc;
    LPSRowSet   lpRow = NULL;
    LONG        cRows = 0;
    char        szBuff[256];
    char        szBuffer[256];

    idxCurSel = SendDlgItemMessage(IDC_DISPLAYVALUE, LB_GETCURSEL, 0, 0L);

    if(idxCurSel == LB_ERR)
    {
        SendDlgItemMessage(IDC_DISPLAYVALUE, LB_RESETCONTENT, 0, 0L);
        SendDlgItemMessage(IDC_ROWVALUES, LB_RESETCONTENT, 0, 0L);
        return;
    }

    /*  Check for being at the end of the table, if so display a warning,
        and return to the beginning of the table. */
    if( idxCurSel >= m_cRows )
    {
        SetStatus("End of table reached", (SCODE)0);
        MessageBox( "You are at the end of the table.", "Warning",MB_OK | MB_ICONINFORMATION);
        wsprintf(szBuff, " %s", "End of table");
        SendDlgItemMessage(IDC_ROWVALUES,LB_RESETCONTENT, 0, 0L);
        lstrcpy(szBuffer,"No row values available");
        SendDlgItemMessage(IDC_ROWVALUES,LB_ADDSTRING, 0, (LPARAM)szBuffer);
        return;
    }

    if( FAILED(sc = GetScode(hError = m_lpTable->SeekRow(BOOKMARK_BEGINNING, idxCurSel, &cRows))) )
        SetStatus("SeekRow failed", sc);

    UpdateRow(m_lpTable);
}


/*
 -  CTblDlg
 -  OnSetBookmark
 -
 *  Purpose:
 *      Sets the requested bookmark.  Adds the bookmark the internal list
 *      of bookmarks.
 *
 */

void CTblDlg::OnSetBookmark()
{
    DWORD       idxCurSel;
    SCODE       sc;
    LPSRowSet   lpRow;
    LONG        cRows;
    BOOKMARK    bk;
    ULONG       idx;

    /* Get index of current row and Seek to there in the table */
    idxCurSel = SendDlgItemMessage(IDC_DISPLAYVALUE, LB_GETCURSEL, 0, 0L);

    if( FAILED(sc = GetScode(hError = m_lpTable->SeekRow(BOOKMARK_BEGINNING, idxCurSel, &cRows))) )
        SetStatus("SeekRow", sc);

    /* Create the Bookmark */
    if( FAILED(sc = GetScode(hError = m_lpTable->CreateBookmark(&bk))) )
        SetStatus("CreateBookmark failed", sc);

    /* QueryRow and get PropValue of Display Value field */
    if( FAILED(sc = GetScode(hError = m_lpTable->QueryRows(1, TBL_NOADVANCE, &lpRow))) )
        SetStatus("QueryRows failed", sc);

    for(idx = 0; idx < lpRow->aRow[0].cValues; idx++)
        if(lpRow->aRow[0].lpProps[idx].ulPropTag == m_ulptDisplay)
            break;

    /* Add new Bookmark to list */
    AddBookmark(&m_BookMarks, bk, &lpRow->aRow[0].lpProps[idx]);

    FreeRowSet(lpRow);
}


/*
 -  CTblDlg
 -  OnForceSort
 -
 *  Purpose:
 *      Sorts the table based upom the requested sort criteria.
 *
 */

void CTblDlg::OnForceSort()
{
    SCODE   sc;
    ULONG   ulCount = 0;

    if(m_lpSort)
    {
        if( FAILED(sc = GetScode(hError = m_lpTable->SortTable(m_lpSort, 0))) )
        {
            SetStatus("SortTable failed", sc);
            return;
        }

        if( FAILED(sc = GetScode(hError = m_lpTable->GetRowCount(TBL_NOWAIT, &ulCount))) )
        {
            SetStatus("SortTable failed", sc);
            return;
        }

        if( ulCount != m_cRows )
        {
            SetStatus("Sort has not completed", sc);
            return;
        }

        RenderTable();
    }
}


/*
 -  CTblDlg
 -  OnForceRestriction
 -
 *  Purpose:
 *      Restricts the table based upon the requested restrictions.
 *
 */

void CTblDlg::OnForceRestriction()
{
    SCODE   sc;

    /* Perform restriction */
    if( FAILED(sc = GetScode(hError = m_lpTable->Restrict(m_lpRes, 0))) )
        SetStatus("Restrict failed", sc);

    // NYI
    /* LogRestriction(m_lpRes, pLogWnd->m_hWnd); */

    if( FAILED(sc = GetScode(hError = m_lpTable->GetRowCount(0, &m_cRows))) )
        SetStatus("GetRowCount failed", sc);

    RenderTable();
}


/*
 -  CTblDlg
 -  OnGetStatus
 -
 *  Purpose:
 *      Gets the current table status and type and places them in the logging
 *      window.
 *
 */

void CTblDlg::OnGetStatus()
{
    ULONG       ulTableStatus;
    ULONG       ulTableType;
    char        szStatus[64];
    char        szType[64];
    char        szBuff[256];
    SCODE       sc;

    if( FAILED(sc = GetScode(hError = m_lpTable->GetStatus(&ulTableStatus, &ulTableType))) )
    {
        SetStatus("GetStatus failed", sc);
        return;
    }
    else
    {
        wsprintf( szStatus, "%s%s%s%s%s%s%s%s",
                ((ulTableStatus == TBLSTAT_COMPLETE)         ? "TBLSTAT_COMPLETE "       : ""),
                ((ulTableStatus == TBLSTAT_QCHANGED)         ? "TBLSTAT_QCHANGED "       : ""),
                ((ulTableStatus == TBLSTAT_SORTING)          ? "TBLSTAT_SORTING "        : ""),
                ((ulTableStatus == TBLSTAT_SORT_ERROR)       ? "TBLSTAT_SORT_ERROR "     : ""),
                ((ulTableStatus == TBLSTAT_SETTING_COLS)     ? "TBLSTAT_SETTING_COLS "   : ""),
                ((ulTableStatus == TBLSTAT_SETCOL_ERROR)     ? "TBLSTAT_SETCOL_ERROR "   : ""),
                ((ulTableStatus == TBLSTAT_RESTRICTING)      ? "TBLSTAT_RESTRICTING "    : ""),
                ((ulTableStatus == TBLSTAT_RESTRICT_ERROR)   ? "TBLSTAT_RESTRICT_ERROR " : ""));
        wsprintf( szType, "%s%s%s",
                ((ulTableType == TBLTYPE_SNAPSHOT)           ? "TBLTYPE_SNAPSHOT "       : ""),
                ((ulTableType == TBLTYPE_KEYSET)             ? "TBLTYPE_KEYSET "         : ""),
                ((ulTableType == TBLTYPE_DYNAMIC)            ? "TBLTYPE_DYNAMIC "        : ""));

        wsprintf( szBuff, "TBLSTAT: %s(%lu)  TBLTYPE: %s(%lu)", szStatus,
                  ulTableStatus, szType, ulTableType );
        pLogWnd->SendMessage(LB_ADDSTRING, 0, (LPARAM)szBuff );
    }
}


/*
 -  CTblDlg
 -  OnClearStatus
 -
 *  Purpose:
 *      Clears the status window.
 *
 */

void CTblDlg::OnClearStatus()
{
    pStatusWnd->SetWindowText("");
}


/*
 -  CTblDlg
 -  OnClearMapiLog
 -
 *  Purpose:
 *      Clears the logging window.
 *
 */

void CTblDlg::OnClearMapiLog()
{
    LONG        j, nSc;

    nSc = SendDlgItemMessage( IDC_MAPILOG, LB_GETCOUNT, 0, 0L);

    if( nSc == LB_ERR )
        return;

    for( j=nSc; j>=0; j-- )
        SendDlgItemMessage(IDC_MAPILOG, LB_DELETESTRING, (WPARAM)j, 0L);
}


/*
 -  CTblDlg
 -  OnQueryPosition
 -
 *  Purpose:
 *      Retrieves the current position in the table and updates the control.
 *
 */

void CTblDlg::OnQueryPosition()
{
    SCODE       sc;
    ULONG       ulRow, ulNum, ulDenom;
    char        szBuff[256];

    if( FAILED(sc = GetScode(hError = m_lpTable->QueryPosition(&ulRow, &ulNum, &ulDenom))) )
        SetStatus("QueryPosition failed", sc);

    if(&ulRow == (ULONG *)0xFFFFFFF)
        MessageBox("QueryPosition returned *lpulRow = 0xFFFFFFFF", "Return values", MB_OK | MB_ICONINFORMATION);

    else
    {
        wsprintf(szBuff, "QueryPosition return values:\
                          \n     ulRow (Current row 0 based): %lu\
                          \n     ulNum (Numerator for fractional position): %lu\
                          \n     ulDenom (Denominator for fractional position): %lu", ulRow, ulNum, ulDenom);
        MessageBox(szBuff, "Return values", MB_OK | MB_ICONINFORMATION);

        strcpy(szBuff,"QueryPosition return values");
        pLogWnd->SendMessage(LB_ADDSTRING, 0, (LPARAM)szBuff);

        wsprintf(szBuff,"     lpulRow (Current row 0 based): %lu", ulRow);
        pLogWnd->SendMessage(LB_ADDSTRING, 0, (LPARAM)szBuff);

        wsprintf(szBuff,"     lpulNum (Numerator for fractional position): %lu", ulNum);
        pLogWnd->SendMessage(LB_ADDSTRING, 0, (LPARAM)szBuff);

        wsprintf(szBuff,"     lpulDenom (Denominator for fractional position): %lu", ulDenom);
        pLogWnd->SendMessage(LB_ADDSTRING, 0, (LPARAM)szBuff);
    }

}


/*
 -  CTblDlg
 -  OnQuerySortOrder
 -
 *  Purpose:
 *      Retrieves the current sort order.
 *
 */

void CTblDlg::OnQuerySortOrder()
{
    SCODE           sc;
    LPSSortOrderSet lpSortCriteria;
    LPSSortOrder    lpso = NULL;
    ULONG           idx;
    char            szBuff[128];
    char            szPropTag[128];
    char            szSO[512];
    char            szSOtemp[128];

    if( FAILED(sc = GetScode(hError = m_lpTable->QuerySortOrder(&lpSortCriteria))) )
        SetStatus("QuerySortOrder failed", sc);
    else
    {
        if(lpSortCriteria)
        {
            strcpy(szSO,"Current Sort Order");
            pLogWnd->SendMessage(LB_ADDSTRING, 0, (LPARAM)szSO );
            for(idx = 0; idx < lpSortCriteria->cSorts; idx++)
            {
                lpso = (LPSSortOrder)PvAlloc(sizeof(SSortOrder));
                *lpso = lpSortCriteria->aSort[idx];
                GetString("PropTags", lpso->ulPropTag, szPropTag);

                if(lpso->ulOrder == TABLE_SORT_ASCEND)
                    wsprintf(szBuff, "   A %s", szPropTag);
                else
                    wsprintf(szBuff, "   D %s", szPropTag);

                pLogWnd->SendMessage(LB_ADDSTRING, 0, (LPARAM)szBuff);
                wsprintf(szSOtemp, "\n   %s", szBuff);
                strcat(szSO, szSOtemp);

                PvFree(lpso);
            }
            MessageBox(szSO, "Return value", MB_OK | MB_ICONINFORMATION);
            strcpy(szBuff,"End of Sort Order");
            pLogWnd->SendMessage(LB_ADDSTRING, 0, (LPARAM)szBuff );

            MAPIFreeBuffer(lpSortCriteria);
        }

        else
            MessageBox("NULL was returned, the Sort Order could not be determined",
                       "Return value", MB_OK | MB_ICONINFORMATION);
    }

}


/*
 -  CTblDlg
 -  OnGetLastError
 -
 *  Purpose:
 *      Retrieves the last MAPI error.
 *
 */

void CTblDlg::OnGetLastError()
{
//    ULONG       ulLowLevelError = 0;
    LPMAPIERROR lpMAPIError     = NULL;
//    ULONG       ulContext       = 0;
    char        szBuff[256];

    m_lpTable->GetLastError(hError, 0, &lpMAPIError);
                
    if(lpMAPIError)
    {
        wsprintf(szBuff,"lpObj->GetLastError()  ulLowLevelError == %lu,ulVersion == %lu lpszMessage == %s, lpszComponent == %s, ulContext == %lu",
                lpMAPIError->ulLowLevelError,
                lpMAPIError->ulVersion,
                ((lpMAPIError->lpszError == NULL)     ? "NULL" : lpMAPIError->lpszError),
                ((lpMAPIError->lpszComponent == NULL) ? "NULL" : lpMAPIError->lpszComponent),
                lpMAPIError->ulContext);

        MessageBox(szBuff, "Warning",MB_OK | MB_ICONINFORMATION);
        pStatusWnd->SetWindowText(lpMAPIError->lpszError);
        MAPIFreeBuffer(lpMAPIError);
    }
    else
        MessageBox("No errors returned from GetLastError().", "Warning", MB_OK | MB_ICONINFORMATION);
}


/*
 -  CTblDlg
 -  OnGetRowCount
 -
 *  Purpose:
 *      Retrieves the row count.
 *
 */

void CTblDlg::OnGetRowCount()
{
    SCODE       sc;
    ULONG       ulRowCount;
    char        szBuff[256];

    if( FAILED(sc = GetScode(hError = m_lpTable->GetRowCount(0, &ulRowCount))) )
        SetStatus("GetRowCount failed", sc);
    else
    {
        wsprintf(szBuff,"GetRowCount returned %lu row(s).", ulRowCount);
        MessageBox(szBuff, "Return value", MB_OK | MB_ICONINFORMATION);
        pStatusWnd->SetWindowText(szBuff);
    }
}


/*
 -  CTblDlg
 -  OnAbort
 -
 *  Purpose:
 *      This function aborts asynchronous operations.
 *
 */

void CTblDlg::OnAbort()
{
    SCODE       sc;

    if( FAILED(sc = GetScode(hError = m_lpTable->Abort())) )
        SetStatus("Abort failed", sc);
}


/*
 -  CTblDlg
 -  OnFind
 -
 *  Purpose:
 *      This function seeks to the next row in the table that contains a
 *      property matching the specified criteria.
 *
 */

void CTblDlg::OnFind()
{
    SCODE       sc;
    ULONG       ulRow, ulNum, ulDenom;
    LPSRowSet   lpRow = NULL;
    CFindDlg    dlgFind(m_lpTable, &m_BookMarks);

    if(dlgFind.DoModal() == IDOK)
    {
        if( FAILED(sc = GetScode(hError = m_lpTable->QueryPosition(&ulRow, &ulNum, &ulDenom))) )
            SetStatus("QueryPosition failed", sc);

        SendDlgItemMessage(IDC_DISPLAYVALUE, LB_SETCURSEL, (WPARAM)ulRow, 0L);
        GetDlgItem(IDC_DISPLAYVALUE)->SetFocus();

        UpdateRow(m_lpTable);
    }
}


/*
 -  CTblDlg
 -  OnFreeBookmark
 -
 *  Purpose:
 *      Free the requested bookmarks.
 *
 */

void CTblDlg::OnFreeBookmark()
{
    CFreeBkDlg  dlgFreeBk(m_lpTable, &m_BookMarks);
    dlgFreeBk.DoModal();
}


/*
 -  CTblDlg
 -  OnRestriction
 -
 *  Purpose:
 *      Brings up the Restriction dialog assigns the restriction variable
 *
 */

void CTblDlg::OnRestriction()
{
    SCODE           sc;
    LPSRestriction  lpRes;
    LPSPropTagArray lpCurColumns;

    if( FAILED(sc = GetScode(hError = m_lpTable->QueryColumns(TBL_ALL_COLUMNS, &lpCurColumns))) )
        SetStatus("QueryColumns failed", sc);

    lpRes = (LPSRestriction)PvAlloc(sizeof(SRestriction));

    CResDlg         dlgRes(lpCurColumns, lpRes);

    if(dlgRes.DoModal() == IDOK)
    {
        FreeRestriction(m_lpRes);
        PvFree(m_lpRes);
        m_lpRes = lpRes;
    }
    else
        PvFree(lpRes);

    MAPIFreeBuffer(lpCurColumns);
}


/*
 -  CTblDlg
 -  OnSeek
 -
 *  Purpose:
 *      Seeks the specified row(s) or fractional position.
 *
 */

void CTblDlg::OnSeek()
{
    SCODE       sc;
    ULONG       ulRow, ulNum, ulDenom;
    LPSRowSet   lpRow = NULL;
    CSeekDlg    dlgSeek(m_lpTable, &m_BookMarks);
    char        szBuffer[256];

    if(dlgSeek.DoModal() == IDOK)
    {
        if( FAILED(sc = GetScode(hError = m_lpTable->QueryPosition(&ulRow, &ulNum, &ulDenom))) )
            SetStatus("QueryPosition failed", sc);

        /*  End of table was reached, bring up a warning dialog and place focus
            on the END OF TABLE entry */
        if(ulNum == ulDenom)
        {
            SetStatus("End of table reached", (SCODE)0);
            MessageBox( "You are at the end of the table.", "Warning",MB_OK | MB_ICONINFORMATION);
            SendDlgItemMessage(IDC_DISPLAYVALUE, LB_SETCURSEL, (WPARAM)m_cRows, 0L);
            SendDlgItemMessage(IDC_ROWVALUES,LB_RESETCONTENT, 0, 0L);
            lstrcpy(szBuffer,"No row values available");
            SendDlgItemMessage(IDC_ROWVALUES,LB_ADDSTRING, 0, (LPARAM)szBuffer);
            return;
        }

        SendDlgItemMessage(IDC_DISPLAYVALUE, LB_SETCURSEL, (WPARAM)ulRow, 0L);
        GetDlgItem(IDC_DISPLAYVALUE)->SetFocus();

        UpdateRow(m_lpTable);
    }
}


/*
 -  CTblDlg
 -  OnSetColumns
 -
 *  Purpose:
 *      Changes the columns currently in view.
 *
 */

void CTblDlg::OnSetColumns()
{
    CSetColDlg  dlgSetCol(m_lpTable);

    dlgSetCol.DoModal();

    RenderTable();

    UpdateRow(m_lpTable);
}


/*
 -  CTblDlg
 -  OnSortOrder
 -
 *  Purpose:
 *      Brings up the sort dialog and assigns the sort criteria.
 *
 */

void CTblDlg::OnSortOrder()
{
    LPSSortOrderSet lpSort;
    CSortDlg        dlgSort(m_lpTable, &lpSort);

    if(dlgSort.DoModal() == IDOK)
    {
        PvFree(m_lpSort);
        m_lpSort = lpSort;
    }
}


/*
 -  CTblDlg
 -  OnClose
 -
 *  Purpose:
 *      Closes the Table viewere and frees memory previously allocated.
 *
 */

void CTblDlg::OnClose()
{
    PvFree(m_lpSort);
    FreeRestriction(m_lpRes);
    PvFree(m_lpRes);
    EndDialog(IDOK);
}


/*
 -  CTblDLg::
 -  UpdateRow
 -
 *  Purpose:
 *      Update the current row and free the allocated memory
 *
 */

void CTblDlg::UpdateRow(LPMAPITABLE lpTable)
{
    SCODE       sc;
    LPSRowSet   lpRow = NULL;

    if( FAILED(sc = GetScode(hError = lpTable->QueryRows(1, TBL_NOADVANCE, &lpRow))) )
        SetStatus("QueryRows failed", sc);

    CTblDlg::RenderRow(lpRow->aRow[0].cValues, lpRow->aRow[0].lpProps);

    FreeRowSet(lpRow);
}


/*
 -  CTblDlg::
 -  RenderRow
 -
 *  Purpose:
 *      Updates the current row in the right pane(row values).
 *
 */

void CTblDlg::RenderRow(ULONG cValues, LPSPropValue lpProps)
{
    ULONG   idx;
    INT     rgTabStops[2];
    char    szBuffer[1024];
    char    szID[50];
    char    szData[512];
    char    szType[32];

    /* Clear the ListBox, Set its size, and set its TabStops */
    SendDlgItemMessage(IDC_ROWVALUES, LB_RESETCONTENT, 0, 0);

    SendDlgItemMessage(IDC_ROWVALUES, LB_SETHORIZONTALEXTENT,
            (WPARAM)VALUES_LB_HOR_SIZE, 0);

    rgTabStops[0] = VALUES_LB_TAB1;
    rgTabStops[1] = VALUES_LB_TAB2;

    SendDlgItemMessage(IDC_ROWVALUES, LB_SETTABSTOPS,
            (WPARAM)2, (LPARAM)rgTabStops);

    /* Populate the ListBox with the data for this row */
    for(idx = 0; idx < cValues; idx++)
    {
        szID[0]     = '\0';
        szData[0]   = '\0';
        szType[0]   = '\0';
        szBuffer[0] = '\0';

        if(GetString("PropIDs", PROP_ID(lpProps[idx].ulPropTag), szID))
        {
            lstrcat(szBuffer, szID );
            lstrcat(szBuffer, "\t");
        }
        else
        {
            wsprintf(szBuffer,"%#08X\t", PROP_ID(lpProps[idx].ulPropTag));
        }

        if(GetString("PropType", PROP_TYPE(lpProps[idx].ulPropTag), szType))
        {
            lstrcat(szBuffer, szType);
            lstrcat(szBuffer, "\t");
        }
        else
        {
            wsprintf(szType, "%#08X\t", PROP_TYPE(lpProps[idx].ulPropTag));
            lstrcat(szBuffer, szType);
        }

        SzGetPropValue(szData, (LPSPropValue)&lpProps[idx]);
        lstrcat(szBuffer, szData);

        SendDlgItemMessage(IDC_ROWVALUES, LB_ADDSTRING, 0, (LPARAM)szBuffer);
    }

    SendDlgItemMessage(IDC_ROWVALUES, LB_SETCURSEL, (WPARAM)-1, 0);
}


/*
 -  CTblDlg::
 -  RenderTable
 -
 *  Purpose:
 *      Updates the current rows in the left pane(Display values).
 *
 */

void CTblDlg::RenderTable()
{
    SCODE       sc          = S_OK;
    LONG        cRows       = 0;
    LPSRowSet   lpRows      = NULL;
    ULONG       idx, idxDN;
    char        szBuff[512];
    char        szBuffer[256];
    char        szErrorBuff[200];
    char        szDummy[50];

    ULONG       ulSize      = LONG_MAX;

    if( FAILED(sc = GetScode(hError = m_lpTable->QueryRows(ulSize, TBL_NOADVANCE, &lpRows))) )
    {
        SetStatus("QueryRows failed", sc);
        wsprintf(szErrorBuff,"CTblDlg::RenderTable(),  lpTable->QueryRows(0x%X,TBL_NOADVANCE,&lpRows) SC: %s",ulSize,
                     GetString( "MAPIErrors", sc, szDummy) );
        MessageBox(szErrorBuff,"Error", MB_ICONSTOP | MB_OK);
        return;
    }

    if( FAILED(sc = GetScode(hError = m_lpTable->SeekRow(BOOKMARK_BEGINNING, 0, &cRows))) )
    {
        SetStatus("SeekRow failed", sc);
        wsprintf(szErrorBuff,"CTblDlg::RenderTable(), lpTable->SeekRow(BOOKMARK_BEGINNING, 0, &cRows)) SC: %s",
                     GetString( "MAPIErrors", sc, szDummy) );
        MessageBox(szErrorBuff,"Error", MB_ICONSTOP | MB_OK);
        return;
    }
    
    ASSERT(lpRows);

    m_cRows = lpRows->cRows;
    if(!m_cRows)
    {
        SendDlgItemMessage(IDC_DISPLAYVALUE, LB_RESETCONTENT, 0, 0L);
        SendDlgItemMessage(IDC_ROWVALUES, LB_RESETCONTENT, 0, 0L);
        FreeRowSet(lpRows);
        return;
    }

    FreeRowSet(lpRows);

    if( FAILED(sc = GetScode(hError = m_lpTable->QueryRows(ulSize, TBL_NOADVANCE, &lpRows))) )
        SetStatus("QueryRows failed", sc);

    if(lpRows->cRows != m_cRows)
        m_cRows = lpRows->cRows;

    /* Look for m_ulptDisplay.  If found, remember its index in the
       PropArray, else display UI and ask user for the property they want
       to display in the left pane. */
    for(idx = 0; idx < lpRows->aRow[0].cValues; idx++)
    {
        if(lpRows->aRow[0].lpProps[idx].ulPropTag == m_ulptDisplay)
            break;
    }

    if(idx < lpRows->aRow[0].cValues)
    {
        idxDN = idx;
    }
    else
    {
        MessageBox("Click OK to select a Display Property.",
                    "Warning", MB_OK | MB_ICONINFORMATION);

        CPickPropDlg dlgPickProp(&lpRows->aRow[0], &idxDN);
        dlgPickProp.DoModal();
    }

    /* Keep PropTag of Display Value property */
    m_ulptDisplay = lpRows->aRow[0].lpProps[idxDN].ulPropTag;

    /* Reset then populate Table listbox with Display Values */
    SendDlgItemMessage(IDC_DISPLAYVALUE, LB_RESETCONTENT, 0, 0L);

    for(idx = 0; idx < m_cRows; idx++)
    {
        SzGetPropValue(szBuff, &lpRows->aRow[idx].lpProps[idxDN]);
        SendDlgItemMessage(IDC_DISPLAYVALUE, LB_ADDSTRING, 0, (LPARAM)szBuff);
    }

    lstrcpy(szBuffer,"END OF TABLE");
    SendDlgItemMessage(IDC_DISPLAYVALUE, LB_ADDSTRING, 0, (LPARAM)szBuffer);
    SendDlgItemMessage(IDC_DISPLAYVALUE, LB_SETCURSEL, 0, 0L);

    FreeRowSet(lpRows);
}


/*
 -  SetStatus(Overloaded)
 -
 *  Purpose:
 *      Displays a status message in the status window at bottom of dialog.
 *
 */

void SetStatus(LPSTR szMsg, SCODE sc)
{
    char    szError[80];
    char    szBuff[80];

    GetString("MAPIErrors", sc, szError);
    wsprintf(szBuff, "%s: %s", szMsg, szError);
    pStatusWnd->SetWindowText(szBuff);
    pLogWnd->SendMessage(LB_ADDSTRING, 0, (LPARAM) szBuff);
}


#ifdef WIN16
/*
 -  SetStatus(Overloaded)
 -
 *  Purpose:
 *      Displays a status message in the status window at bottom of dialog.
 *
 */

void SetStatus(LPSTR szMsg, HRESULT hr)
{
    SetStatus(szMsg, GetScode(hr));
}
#endif   

/*-----------------------------*/
/* CSetColDlg Message Handlers */
/*-----------------------------*/

BEGIN_MESSAGE_MAP(CSetColDlg, CModalDialog)
    ON_COMMAND(IDC_ADD,         OnAdd)
    ON_COMMAND(IDC_REMOVE,      OnRemove)
    ON_COMMAND(IDC_REMOVEALL,   OnRemoveAll)
    ON_COMMAND(IDC_ADDHEX,      OnAddHexValue)
    ON_COMMAND(IDC_ALLMAPITAGS, OnAllMAPITags)
    ON_COMMAND(IDC_SET,         OnSet)
    ON_COMMAND(IDOK,            OnClose)
    ON_COMMAND(IDCANCEL,        OnCancel)
END_MESSAGE_MAP()


/*
 -  CSetColDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Constructor for main dialog class.
 *
 */

BOOL CSetColDlg::OnInitDialog()
{
    SCODE           sc;
    ULONG           idx;
    char            szBuff[256];
    LPSPropTagArray lpAllColumns;
    LPSPropTagArray lpCurColumns;

    if( FAILED(sc = GetScode(hError = m_lpTable->QueryColumns(TBL_ALL_COLUMNS, &lpAllColumns))) )
        SetStatus("QueryColumns failed", sc);

    if( FAILED(sc = GetScode(hError = m_lpTable->QueryColumns(0, &lpCurColumns))) )
        SetStatus("QueryColumns failed", sc);

    for(idx = 0; idx < lpAllColumns->cValues; idx++)
    {
        GetString("PropTags", lpAllColumns->aulPropTag[idx], szBuff);
        SendDlgItemMessage(IDC_ALLCOL, LB_ADDSTRING, 0, (LPARAM)szBuff);
    }

    for(idx = 0; idx < lpCurColumns->cValues; idx++)
    {
        GetString("PropTags", lpCurColumns->aulPropTag[idx], szBuff);
        SendDlgItemMessage(IDC_CURCOL, LB_ADDSTRING, 0, (LPARAM)szBuff);
    }

    MAPIFreeBuffer(lpAllColumns);
    MAPIFreeBuffer(lpCurColumns);

    return TRUE;
}


/*
 -  CSetColDlg
 -  OnAdd
 -
 *  Purpose:
 *      Adds the requested columns for a SetColumns call.
 *
 */

void CSetColDlg::OnAdd()
{
    DWORD       idx;
    INT         *lpItems;
    LONG        nSc;
    char        szPropTag[128];
    char        szBuff[128];

    nSc = SendDlgItemMessage( IDC_ALLCOL, LB_GETSELCOUNT, 0, 0L);
    lpItems = (INT *)PvAlloc(sizeof(int)*nSc);
    idx = SendDlgItemMessage(IDC_ALLCOL, LB_GETSELITEMS, (WPARAM)nSc, (LPARAM)lpItems );

    if( idx == LB_ERR )
    {
        wsprintf(szBuff, "An error occured when adding a PropTag");
        pLogWnd->SendMessage(LB_ADDSTRING, 0, (LPARAM) szBuff);

        PvFree(lpItems);
        return;
    }

    for( nSc=0; nSc<idx; nSc++ )
    {
        SendDlgItemMessage(IDC_ALLCOL, LB_GETTEXT, lpItems[nSc], (LPARAM)szPropTag);
        SendDlgItemMessage(IDC_CURCOL, LB_ADDSTRING, 0, (LPARAM)szPropTag);
    }
    PvFree(lpItems);
    wsprintf(szBuff, "All PropTag(s) were added to SetColumns");
    pLogWnd->SendMessage(LB_ADDSTRING, 0, (LPARAM) szBuff);
}


/*
 -  CSetColDlg
 -  OnRemove
 -
 *  Purpose:
 *      Removes the requested columns for a SetColumns call.
 *
 */

void CSetColDlg::OnRemove()
{
    DWORD       idx;
    INT         *lpItems;
    LONG        nSc;

    nSc = SendDlgItemMessage( IDC_CURCOL, LB_GETSELCOUNT, 0, 0L);
    if( nSc == 0)
        return;

    lpItems = (INT *)PvAlloc(sizeof(int)*nSc);
    idx = SendDlgItemMessage(IDC_CURCOL, LB_GETSELITEMS, (WPARAM)nSc, (LPARAM)lpItems );

    if( idx == LB_ERR )
    {
        PvFree(lpItems);
        return;
    }

    while(nSc-- > 0)
        SendDlgItemMessage(IDC_CURCOL, LB_DELETESTRING, (WPARAM)lpItems[nSc], 0L);

    PvFree(lpItems);
}


/*
 -  CSetColDlg
 -  OnRemove
 -
 *  Purpose:
 *      Removes all columns for a SetColumns call.
 *
 */

void CSetColDlg::OnRemoveAll()
{
    SendDlgItemMessage(IDC_CURCOL, LB_RESETCONTENT, (WPARAM)0, 0L);
}


/*
 -  CSetColDlg
 -  OnRemove
 -
 *  Purpose:
 *      Takes the requested columns and updates the table.
 *
 */

void CSetColDlg::OnSet()
{
    SCODE           sc;
    DWORD           idx, cPropTags;
    char            szPropTag[128];
    ULONG *         lpulTag;
    LPSPropTagArray lpspta;
    char            *szEnd      = NULL;    

    cPropTags = SendDlgItemMessage(IDC_CURCOL, LB_GETCOUNT, 0, 0L);

    lpspta = (LPSPropTagArray)PvAlloc(sizeof(SPropTagArray) + cPropTags * sizeof(ULONG));

    lpspta->cValues = cPropTags;

    lpulTag = &lpspta->aulPropTag[0];

    for(idx=0; idx<cPropTags; idx++)
    {
        SendDlgItemMessage(IDC_CURCOL, LB_GETTEXT, (WPARAM)idx, (LPARAM)szPropTag);
        if(!GetID("PropTags", szPropTag, (LONG *)lpulTag))
        #ifdef WIN16
            *lpulTag = strtoul(szPropTag,&szEnd,16);
        #else
            sscanf(szPropTag, "0x%lX", lpulTag);
        #endif
        lpulTag++;
    }


    if( FAILED(sc = GetScode(hError = m_lpTable->SetColumns(lpspta, 0))) )
        SetStatus("SetColumns failed", sc);

    PvFree(lpspta);
}


/*
 -  CSetColDlg::
 -  OnAddHexValue()
 *
 *  Purpose:
 *      Select a hex value for the PROP ID and adjust the
 *      proptag listbox and the prop data accordingly
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      Nothing
 *
 */

void CSetColDlg::OnAddHexValue()
{
    char    szBuffer[512];
    ULONG   ulIDVal     = 0;
    LONG    len         = -1;

    // get value out of the edit control
    *(WORD *)szBuffer = sizeof(szBuffer) -1;    // first char has buffer length

    len  = SendDlgItemMessage(IDC_HEXVALUE, EM_GETLINE, 0, (LPARAM)(void *)szBuffer);

    szBuffer[len] = '\0';        // null terminate our buffer

    if( ! AsciiToHex(len, szBuffer, &ulIDVal ) )

    {
        MessageBox("AsciiToHex conversion failed", "Error", MB_OK | MB_ICONEXCLAMATION );
        return;
    }

    if( !GetString("PropIDs", ulIDVal, szBuffer) )
    {
        wsprintf(szBuffer, "0x%X",ulIDVal);
        SendDlgItemMessage(IDC_CURCOL, LB_ADDSTRING, 0, (LPARAM)szBuffer);
    }
    else
        SendDlgItemMessage(IDC_CURCOL, LB_ADDSTRING, 0, (LPARAM)szBuffer);

    strcpy(szBuffer,"");
    SendDlgItemMessage(IDC_HEXVALUE, WM_SETTEXT, 0, (LPARAM)szBuffer);
}


/*
 -  CSetColDlg
 -  OnAddMAPITags
 -
 *  Purpose
 *      Adds all MAPI PropTags to the  list.
 *
 */

void CSetColDlg::OnAllMAPITags()
{
    ULONG       idx;
    ULONG       ulRowCount;
    char        szBuffer[64];

    //Reset content of listbox
    SendDlgItemMessage(IDC_ALLCOL, LB_RESETCONTENT, 0, (LPARAM)0);

    // ADD DATA TO LISTBOX OF PROP IDS
    szBuffer[0] = '\0' ;
    // this (UNKNOWN PROPID) is always in the 0th place in listbox
    SendDlgItemMessage(IDC_ALLCOL, LB_ADDSTRING, 0, (LPARAM)(void *)"AN UNKNOWN PROPID");

    ulRowCount = GetRowCount("PropIDs");
    for( idx = 0; idx < ulRowCount ; idx++ )
    {
        GetRowString("PropIDs",idx,szBuffer);
        SendDlgItemMessage(IDC_ALLCOL, LB_ADDSTRING, 0, (LPARAM)szBuffer);
    }
}


/*
 -  CSetColDlg
 -  OnClose
 -
 *  Purpose
 *      Closes the SetColumns dialog.
 *
 */

void CSetColDlg::OnClose()
{
    if( SendDlgItemMessage(IDC_CURCOL, LB_GETCOUNT, 0, 0L) == 0)
        MessageBox("You must select at least 1 property.",
                   "Warning", MB_OK | MB_ICONINFORMATION);
    else
        EndDialog(IDOK);
}


/*
 -  CSetColDlg
 -  OnCancel
 -
 *  Purpose
 *      Cancels the SetColumns dialog.
 *
 */

void CSetColDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}


/*-------------------------------*/
/* CPickPropDlg Message Handlers */
/*-------------------------------*/

BEGIN_MESSAGE_MAP(CPickPropDlg, CModalDialog)
END_MESSAGE_MAP()


/*
 -  CPickPropDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Constructor for main dialog class.
 *
 */

BOOL CPickPropDlg::OnInitDialog()
{
    ULONG   idx;
    char    szPropTag[256];

    for(idx = 0; idx < m_lpRow->cValues; idx++)
    {
        GetString("PropTags", m_lpRow->lpProps[idx].ulPropTag, szPropTag);
        SendDlgItemMessage(IDC_PICKPROP, LB_ADDSTRING, 0, (LPARAM)szPropTag);
    }

    return TRUE;
}

/*
 -  CPickPropDlg
 -  OnOK
 -
 *  Purpose:
 *      Retrieves the currently selected property for display values
 *
 */

void CPickPropDlg::OnOK()
{
    *m_lpdwIndex = SendDlgItemMessage(IDC_PICKPROP, LB_GETCURSEL, 0, 0L);
    EndDialog(IDOK);
}


/*-----------------------------*/
/* CFreeBkDlg Message Handlers */
/*-----------------------------*/

BEGIN_MESSAGE_MAP(CFreeBkDlg, CModalDialog)
    ON_COMMAND(IDC_FREE,    OnFree)
    ON_COMMAND(IDC_FREEALL, OnFreeAll)
    ON_COMMAND(IDC_CLOSE,   OnClose)
END_MESSAGE_MAP()


/*
 -  CFreeBkDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Constructor for main dialog class.
 *
 */

BOOL CFreeBkDlg::OnInitDialog()
{
    ULONG   idx;
    char    szBuff[512];

    if(!m_lpBkList->cValues)
    {
        GetDlgItem(IDC_FREE)->EnableWindow(FALSE);
        GetDlgItem(IDC_FREEALL)->EnableWindow(FALSE);
        return TRUE;
    }

    for(idx = 0; idx < m_lpBkList->cValues; idx++)
    {
        SzGetPropValue(szBuff, m_lpBkList->bkList[idx].lpProp);
        SendDlgItemMessage(IDC_BKVALUES, LB_ADDSTRING, 0, (LPARAM)szBuff);
    }

    return TRUE;
}


/*
 -  CFreeBkDlg::
 -  OnFree
 -
 *  Purpose:
 *      Frees the requested bookmarks.
 *
 */

void CFreeBkDlg::OnFree()
{
    DWORD       cLeft = 0;
    INT         *lpItems;
    LONG        nSc;
    SCODE       sc;

    nSc = SendDlgItemMessage( IDC_BKVALUES, LB_GETSELCOUNT, 0, 0L);
    lpItems = (INT *)PvAlloc(sizeof(int)*nSc);
    if( SendDlgItemMessage(IDC_BKVALUES, LB_GETSELITEMS, (WPARAM)nSc, (LPARAM)lpItems ) == LB_ERR )
    {
        PvFree(lpItems);
        return;
    }

    while(nSc-- > 0)
    {
        cLeft = SendDlgItemMessage(IDC_BKVALUES, LB_DELETESTRING, (WPARAM)lpItems[nSc], 0L);
        if( FAILED(sc = GetScode(hError = m_lpTable->FreeBookmark(m_lpBkList->bkList[nSc].bk))) )
            SetStatus("FreeBookmark failed", sc);
        RemoveBookmark(m_lpBkList, nSc);
    }
    PvFree(lpItems);

    if(!cLeft)
    {
        GetDlgItem(IDC_FREE)->EnableWindow(FALSE);
        GetDlgItem(IDC_FREEALL)->EnableWindow(FALSE);
    }
}


/*
 -  CFreeBkDlg::
 -  OnFreeAll
 -
 *  Purpose:
 *      Frees all of the bookmarks.
 *
 */

void CFreeBkDlg::OnFreeAll()
{
    SCODE   sc;
    DWORD   idx;
    DWORD   cBks;

    cBks = m_lpBkList->cValues;

    for(idx = cBks; idx > 0; idx--)
    {
        if( FAILED(sc = GetScode(hError = m_lpTable->FreeBookmark(m_lpBkList->bkList[idx-1].bk))) )
            SetStatus("FreeBookmark failed", sc);
        RemoveBookmark(m_lpBkList, idx-1);
    }

    SendDlgItemMessage(IDC_BKVALUES, LB_RESETCONTENT, 0, 0L);

    GetDlgItem(IDC_FREE)->EnableWindow(FALSE);
    GetDlgItem(IDC_FREEALL)->EnableWindow(FALSE);
}


/*
 -  CFreeBkDlg::
 -  OnClose
 -
 *  Purpose:
 *      Closes the FreeBookmark dialog.
 *
 */

void CFreeBkDlg::OnClose()
{
    EndDialog(IDOK);
}


/*-----------------------------*/
/* CSortDlg Message Handlers */
/*-----------------------------*/

BEGIN_MESSAGE_MAP(CSortDlg, CModalDialog)
    ON_COMMAND(IDC_ADDA,        OnAdd)
    ON_COMMAND(IDC_ADDD,        OnAddDesc)
    ON_COMMAND(IDC_REMOVESORT,  OnRemove)
    ON_COMMAND(IDC_REMOVEALL,   OnRemoveAll)
    ON_COMMAND(IDC_ADDHEXA,     OnAddHexValueA)
    ON_COMMAND(IDC_ADDHEXD,     OnAddHexValueD)    
    ON_COMMAND(IDC_ALLMAPITAGS, OnAllMAPITags)    
END_MESSAGE_MAP()


/*
 -  CSortDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Constructor for main dialog class.
 *
 */

BOOL CSortDlg::OnInitDialog()
{
    SCODE           sc;
    ULONG           idx;
    DWORD           dwIndex;
    LPSSortOrder    lpso = NULL;
    LPSSortOrderSet lpCurSort = NULL;
    LPSPropTagArray lpAllColumns = NULL;
    char            szBuff[128];
    char            szPropTag[128];

    fDesc = FALSE;
    if( FAILED(sc = GetScode(hError = m_lpTable->QueryColumns(TBL_ALL_COLUMNS, &lpAllColumns))) )
        SetStatus("QueryColumns",sc);

    for(idx = 0; idx < lpAllColumns->cValues; idx++)
    {
        GetString("PropTags", lpAllColumns->aulPropTag[idx], szPropTag);
        SendDlgItemMessage(IDC_COLUMN, LB_ADDSTRING, 0, (LPARAM)szPropTag);
    }

    if( FAILED(sc = GetScode(hError = m_lpTable->QuerySortOrder(&lpCurSort))) )
    {
        SetStatus("QuerySortOrder failed", sc);
        goto exit;
    }

    for(idx = 0; idx < lpCurSort->cSorts; idx++)
    {
        lpso = (LPSSortOrder)PvAlloc(sizeof(SSortOrder));

        *lpso = lpCurSort->aSort[idx];

        GetString("PropTags", lpso->ulPropTag, szPropTag);

        if(lpso->ulOrder == TABLE_SORT_ASCEND)
            wsprintf(szBuff, "A %s", szPropTag);
        else
            wsprintf(szBuff, "D %s", szPropTag);

        dwIndex = SendDlgItemMessage(IDC_SORT, LB_ADDSTRING, 0, (LPARAM)szBuff);

        SendDlgItemMessage(IDC_SORT, LB_SETITEMDATA, (WPARAM)dwIndex, (LPARAM)lpso);
    }

    MAPIFreeBuffer(lpCurSort);

exit:
    MAPIFreeBuffer(lpAllColumns);

    return TRUE;
}


/*
 -  CSortDlg
 -  OnAdd
 -
 *  Purpose:
 *      Adds an ascending sort order criteria.
 *
 */

void CSortDlg::OnAdd()
{
    DWORD           dwIndex;
    char            szBuff[128];
    char            szPropTag[128];
    LPSSortOrder    lpso;

    dwIndex = SendDlgItemMessage(IDC_COLUMN, LB_GETCURSEL, 0, 0L);

    if(dwIndex == LB_ERR)
        return;

    SendDlgItemMessage(IDC_COLUMN, LB_GETTEXT,
            (WPARAM)dwIndex, (LPARAM)szPropTag);

    lpso = (LPSSortOrder)PvAlloc(sizeof(SSortOrder));

    if(fDesc)
    {
        if(!GetID("PropTags", szPropTag, (LONG *)&lpso->ulPropTag))
            lpso->ulPropTag = strtoul(szPropTag, NULL ,0);
        lpso->ulOrder   = TABLE_SORT_DESCEND;

        wsprintf(szBuff, "D %s", szPropTag);
        fDesc = FALSE;
    }

    else
    {
        if(!GetID("PropTags", szPropTag, (LONG *)&lpso->ulPropTag))
            lpso->ulPropTag = strtoul(szPropTag, NULL ,0);
        lpso->ulOrder   = TABLE_SORT_ASCEND;

        wsprintf(szBuff, "A %s", szPropTag);
    }

    dwIndex = SendDlgItemMessage(IDC_SORT, LB_ADDSTRING, 0, (LPARAM)szBuff);

    SendDlgItemMessage(IDC_SORT, LB_SETITEMDATA, (WPARAM)dwIndex, (LPARAM)lpso);
}

/*
 -  CSortDlg
 -  OnAddDesc
 -
 *  Purpose:
 *      Adds an ascending sort order criteria.
 *
 */

void CSortDlg::OnAddDesc()
{
    fDesc = TRUE;
    OnAdd();
}


/*
 -  CSortDlg
 -  OnRemove
 -
 *  Purpose:
 *      Removes a sort order.
 *
 */

void CSortDlg::OnRemove()
{
    DWORD           dwIndex;
    LPSSortOrder    lpso;

    dwIndex = SendDlgItemMessage(IDC_SORT, LB_GETCURSEL, 0, 0L);

    if(dwIndex == LB_ERR)
        return;

    lpso = (LPSSortOrder)SendDlgItemMessage(IDC_SORT,
            LB_GETITEMDATA, (WPARAM)dwIndex, 0L);

    PvFree(lpso);

    SendDlgItemMessage(IDC_SORT, LB_DELETESTRING, (WPARAM)dwIndex, 0L);
}


/*
 -  CSortDlg
 -  OnRemoveAll
 -
 *  Purpose:
 *      Removes all sort orders.
 *
 */

void CSortDlg::OnRemoveAll()
{
    LPSSortOrder    lpso;
    DWORD           idx;
    DWORD           cSorts;

    cSorts = SendDlgItemMessage(IDC_SORT, LB_GETCOUNT, 0, 0L);

    for(idx = 0; idx < cSorts; idx++)
    {
        lpso = (LPSSortOrder)SendDlgItemMessage(IDC_SORT,
                LB_GETITEMDATA, (WPARAM)idx, 0L);

        PvFree(lpso);
    }

    SendDlgItemMessage(IDC_SORT, LB_RESETCONTENT, 0, 0L);
}


/*
 -  CSortDlg::
 -  OnAddHexValueA()
 *
 *  Purpose:
 *      Select a hex value for the PROP ID and adjust the
 *      proptag listbox and the prop data accordingly.  Order is Ascending
 *
 *  Parameters:
 *      None
 *
 *  Returns:
 *      Nothing
 *
 */
              
void CSortDlg::OnAddHexValueA()
{
    char            szBuffer[512];
    ULONG           ulIDVal     = 0;
    LONG            len         = -1;
    DWORD           dwIndex;
    char            szBuff[128];
    char            szPropTag[128];
    LPSSortOrder    lpso;

    // get value out of the edit control
    *(WORD *)szBuffer = sizeof(szBuffer) -1;    // first char has buffer length

    len  = SendDlgItemMessage(IDC_HEXVALUE, EM_GETLINE, 0, (LPARAM)(void *)szBuffer);

    szBuffer[len] = '\0';        // null terminate our buffer

    if( ! AsciiToHex(len, szBuffer, &ulIDVal ) )

    {
        MessageBox("AsciiToHex conversion failed", "Error", MB_OK | MB_ICONEXCLAMATION );
        return;
    }

    lpso = (LPSSortOrder)PvAlloc(sizeof(SSortOrder));

    if(fDesc)
    {
        if( GetString("PropIDs", ulIDVal, szBuffer) )
            GetID("PropTags", szPropTag, (LONG *)&lpso->ulPropTag);
        else
        {
            lpso->ulPropTag = ulIDVal;
            wsprintf(szPropTag, "0x%X",ulIDVal);
        }                        
                        
        lpso->ulOrder   = TABLE_SORT_DESCEND;

        wsprintf(szBuff, "D %s", szPropTag);
        fDesc = FALSE;
    }

    else
    {
        if( GetString("PropIDs", ulIDVal, szBuffer) )
            GetID("PropTags", szPropTag, (LONG *)&lpso->ulPropTag);
        else
        {
            lpso->ulPropTag = ulIDVal;
            wsprintf(szPropTag, "0x%X",ulIDVal);
        }                        

        lpso->ulOrder   = TABLE_SORT_ASCEND;

        wsprintf(szBuff, "A %s", szPropTag);
    }

    dwIndex = SendDlgItemMessage(IDC_SORT, LB_ADDSTRING, 0, (LPARAM)szBuff);

    SendDlgItemMessage(IDC_SORT, LB_SETITEMDATA, (WPARAM)dwIndex, (LPARAM)lpso);

    strcpy(szBuff,"");
    SendDlgItemMessage(IDC_HEXVALUE, WM_SETTEXT, 0, (LPARAM)szBuff); 
}


/*
 -  CSortDlg
 -  OnAddHexValueD
 -
 *  Purpose:
 *      Adds an ascending sort order criteria.
 *
 */

void CSortDlg::OnAddHexValueD()
{
    fDesc = TRUE;
    OnAddHexValueA();
}


/*
 -  CSetColDlg
 -  OnAddMAPITags
 -
 *  Purpose
 *      Adds all MAPI PropTags to the  list.
 *
 */

void CSortDlg::OnAllMAPITags()
{
    ULONG       idx;
    ULONG       ulRowCount;
    char        szBuffer[64];

    //Reset content of listbox
    SendDlgItemMessage(IDC_COLUMN, LB_RESETCONTENT, 0, (LPARAM)0);

    // ADD DATA TO LISTBOX OF PROP IDS
    szBuffer[0] = '\0' ;
    // this (UNKNOWN PROPID) is always in the 0th place in listbox
    SendDlgItemMessage(IDC_COLUMN, LB_ADDSTRING, 0, (LPARAM)(void *)"AN UNKNOWN PROPID");

    ulRowCount = GetRowCount("PropIDs");
    for( idx = 0; idx < ulRowCount ; idx++ )
    {
        GetRowString("PropIDs",idx,szBuffer);
        SendDlgItemMessage(IDC_COLUMN, LB_ADDSTRING, 0, (LPARAM)szBuffer);
    }
}


/*
 -  CSortDlg
 -  OnOK
 -
 *  Purpose:
 *      Exits the Sort dialog and assigns all the sort orders to
 *      be used when sort is executed(forced).
 *
 */

void CSortDlg::OnOK()
{
    LPSSortOrder    lpso;
    LPSSortOrderSet lpsos = NULL;
    DWORD           idx;
    DWORD           cSorts;

    cSorts = SendDlgItemMessage(IDC_SORT, LB_GETCOUNT, 0, 0L);

    lpsos = (LPSSortOrderSet)PvAlloc(sizeof(SSortOrderSet) + cSorts * sizeof(SSortOrder));
    lpsos->cSorts = cSorts;
    lpsos->cCategories = 0;
    lpsos->cExpanded = 0;

    for(idx = 0; idx < cSorts; idx++)
    {
        lpso = (LPSSortOrder)SendDlgItemMessage(IDC_SORT,
                LB_GETITEMDATA, (WPARAM)idx, 0L);

        lpsos->aSort[idx] = *lpso;

        PvFree(lpso);
    }

    SendDlgItemMessage(IDC_SORT, LB_RESETCONTENT, 0, 0L);

    *m_lppSort = lpsos;

    EndDialog(IDOK);
}


/*
 -  CSortDlg
 -  OnCancel
 -
 *  Purpose:
 *      Cancels the sort dialog.
 *
 */

void CSortDlg::OnCancel()
{
    LPSSortOrder    lpso;
    DWORD           idx;
    DWORD           cSorts;

    cSorts = SendDlgItemMessage(IDC_SORT, LB_GETCOUNT, 0, 0L);

    for(idx = 0; idx < cSorts; idx++)
    {
        lpso = (LPSSortOrder)SendDlgItemMessage(IDC_SORT,
                LB_GETITEMDATA, (WPARAM)idx, 0L);

        PvFree(lpso);
    }

    SendDlgItemMessage(IDC_SORT, LB_RESETCONTENT, 0, 0L);

    *m_lppSort = NULL;

    EndDialog(IDCANCEL);
}



/*-----------------------------*/
/* CSeekDlg Message Handlers   */
/*-----------------------------*/

BEGIN_MESSAGE_MAP(CSeekDlg, CModalDialog)
    ON_COMMAND(IDC_SEEKROW,       OnTypeSeekRow)
    ON_COMMAND(IDC_SEEKROWAPPROX, OnTypeSeekRowApprox)
    ON_COMMAND(IDC_BEGINNING,     OnBkBeginning)
    ON_COMMAND(IDC_CURRENT,       OnBkCurrent)
    ON_COMMAND(IDC_END,           OnBkEnd)
    ON_COMMAND(IDC_USER,          OnBkUser)
END_MESSAGE_MAP()


/*
 -  CSeekDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Constructor for main dialog class.
 *
 */

BOOL CSeekDlg::OnInitDialog()
{
    ULONG   idx;
    char    szBuff[512];

    CheckDlgButton(IDC_SEEKROW, TRUE);
    CheckDlgButton(IDC_BEGINNING, TRUE);

    m_fSeek = TRUE;
    m_nBk = 0;

    if(!m_lpBkList->cValues)
    {
        GetDlgItem(IDC_USER)->EnableWindow(FALSE);
    }
    else
    {
        for(idx = 0; idx < m_lpBkList->cValues; idx++)
        {
            SzGetPropValue(szBuff, m_lpBkList->bkList[idx].lpProp);

            SendDlgItemMessage(IDC_BKSEEK, LB_ADDSTRING, 0, (LPARAM)szBuff);
        }
    }

    GetDlgItem(IDC_ROWCOUNT)->SetFocus();

    return TRUE;
}


/*
 -  CSeekDlg
 -  OnTypeSeekRow
 -
 *  Purpose:
 *      Changes the view in the SeekRow/SeekRowApprox dialog.  This view is for
 *      SeekRow().
 *
 */

void CSeekDlg::OnTypeSeekRow()
{
    GetDlgItem(IDT_NUMERATOR)->EnableWindow(FALSE);
    GetDlgItem(IDC_NUMERATOR)->EnableWindow(FALSE);
    GetDlgItem(IDT_DENOMINATOR)->EnableWindow(FALSE);
    GetDlgItem(IDC_DENOMINATOR)->EnableWindow(FALSE);
    GetDlgItem(IDT_ROWCOUNT)->EnableWindow(TRUE);
    GetDlgItem(IDC_ROWCOUNT)->EnableWindow(TRUE);

    m_fSeek = TRUE;
}


/*
 -  CSeekDlg
 -  OnTypeSeekRowApprox
 -
 *  Purpose:
 *      Changes the view in the SeekRow/SeekRowApprox dialog.  This view is for
 *      SeekRowApprox().
 *
 */

void CSeekDlg::OnTypeSeekRowApprox()
{
    GetDlgItem(IDT_NUMERATOR)->EnableWindow(TRUE);
    GetDlgItem(IDC_NUMERATOR)->EnableWindow(TRUE);
    GetDlgItem(IDT_DENOMINATOR)->EnableWindow(TRUE);
    GetDlgItem(IDC_DENOMINATOR)->EnableWindow(TRUE);
    GetDlgItem(IDT_ROWCOUNT)->EnableWindow(FALSE);
    GetDlgItem(IDC_ROWCOUNT)->EnableWindow(FALSE);

    m_fSeek = FALSE;
}


/*
 -  CSeekDlg
 -  OnBkBeginning
 -
 *  Purpose:
 *      Sets the Beginning bookmark.
 *
 */

void CSeekDlg::OnBkBeginning()
{
    m_nBk = 0;
}


/*
 -  CSeekDlg
 -  OnBkCurrent
 -
 *  Purpose:
 *      Sets the Current bookmark.
 *
 */

void CSeekDlg::OnBkCurrent()
{
    m_nBk = 1;
}


/*
 -  CSeekDlg
 -  OnBkEnd
 -
 *  Purpose:
 *      Sets the End bookmark.
 *
 */

void CSeekDlg::OnBkEnd()
{
    m_nBk = 2;
}


/*
 -  CSeekDlg
 -  OnBkUser
 -
 *  Purpose:
 *      Sets the user bookmark.
 *
 */

void CSeekDlg::OnBkUser()
{
    m_nBk = 3;
}


/*
 -  CSeekDlg
 -  OnOK
 -
 *  Purpose:
 *      Executes the SeekRow or SeekRowApprox command and closes the dialog.
 *
 */

void CSeekDlg::OnOK()
{
    SCODE       sc;
    BOOL        fSeek = FALSE;
    LONG        lRowCount;
    LONG        lRowsSeeked;
    ULONG       ulNum;
    ULONG       ulDenom;
    DWORD       dwIndex;
    BOOKMARK    bk;

    if(m_fSeek)
    {
        lRowCount = (LONG)(int)GetDlgItemInt(IDC_ROWCOUNT);
    }
    else
    {
        ulNum   = (ULONG)GetDlgItemInt(IDC_NUMERATOR, NULL, FALSE);
        ulDenom = (ULONG)GetDlgItemInt(IDC_DENOMINATOR, NULL, FALSE);
    }

    if(m_fSeek)
    {
        switch(m_nBk)
        {
        case 0:
            bk = BOOKMARK_BEGINNING;
            break;

        case 1:
            bk = BOOKMARK_CURRENT;
            break;

        case 2:
            bk = BOOKMARK_END;
            break;

        case 3:
            dwIndex = SendDlgItemMessage(IDC_BKSEEK, LB_GETCURSEL, 0, 0L);

            bk = m_lpBkList->bkList[dwIndex].bk;
            break;
        }
    }

    if(m_fSeek)
    {
        if( FAILED(sc = GetScode(hError = m_lpTable->SeekRow(bk, lRowCount, &lRowsSeeked))) )
            SetStatus("SeekRow failed", sc);

        if( lRowCount != lRowsSeeked )
            SetStatus("lRowCount != lRowSeeked beginning or end of table reached", (SCODE)0);
    }
    else
    {
        if( FAILED(sc = GetScode(hError = m_lpTable->SeekRowApprox(ulNum, ulDenom))) )
            SetStatus("SeekRowApprox failed", sc);
    }

    EndDialog(IDOK);
}


/*
 -  CSeekDlg
 -  OnCancel
 -
 *  Purpose:
 *      Cancels the SeekRow/SeekRowApprox dialgo
 *
 */

void CSeekDlg::OnCancel()
{
    EndDialog(IDCANCEL);
}



/*-----------------------------*/
/* CFindDlg Message Handlers   */
/*-----------------------------*/

BEGIN_MESSAGE_MAP(CFindDlg, CModalDialog)
    ON_COMMAND(IDC_BUILDRES,    OnBuildRes)
    ON_COMMAND(IDC_FORWARD,     OnDirForward)
    ON_COMMAND(IDC_BACKWARD,    OnDirBackward)
    ON_COMMAND(IDC_INVALID,     OnDirInvalid)
    ON_COMMAND(IDC_BEGINNING,   OnBkBeginning)
    ON_COMMAND(IDC_CURRENT,     OnBkCurrent)
    ON_COMMAND(IDC_END,         OnBkEnd)
    ON_COMMAND(IDC_USER,        OnBkUser)
END_MESSAGE_MAP()


/*
 -  CFindDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Constructor for main dialog class.
 *
 */

BOOL CFindDlg::OnInitDialog()
{
    ULONG   idx;
    char    szBuff[512];

    CheckDlgButton(IDC_FORWARD, TRUE);
    CheckDlgButton(IDC_BEGINNING, TRUE);

    m_nBk = 0;
    m_ulFlags = 0;

    if(!m_lpBkList->cValues)
    {
        GetDlgItem(IDC_USER)->EnableWindow(FALSE);
    }
    else
    {
        for(idx = 0; idx < m_lpBkList->cValues; idx++)
        {
            SzGetPropValue(szBuff, m_lpBkList->bkList[idx].lpProp);

            SendDlgItemMessage(IDC_BKSEEK, LB_ADDSTRING, 0, (LPARAM)szBuff);
        }
    }

    GetDlgItem(IDC_FORWARD)->SetFocus();

    return TRUE;
}


/*
 -  CFindDlg
 -  OnBuildRes
 -
 *  Purpose:
 *      Brings up the Restriction dialog from the Find dialog and assigns
 *      the restriction to be executed when the Find is executed.
 *
 */

void CFindDlg::OnBuildRes()
{
    SCODE           sc;
    LPSRestriction  lpRes;
    LPSPropTagArray lpCurColumns;

    if( FAILED(sc = GetScode(hError = m_lpTable->QueryColumns(0, &lpCurColumns))) )
        SetStatus("QueryColumns failed", sc);

    lpRes = (LPSRestriction)PvAlloc(sizeof(SRestriction));

    CResDlg         dlgBuildRes(lpCurColumns, lpRes);

    if(dlgBuildRes.DoModal() == IDOK)
    {
        FreeRestriction(m_lpFindRes);
        PvFree(m_lpFindRes);
        m_lpFindRes = lpRes;
    }
    else
        PvFree(lpRes);

    MAPIFreeBuffer(lpCurColumns);
}


/*
 -  CFindDlg
 -  OnDirForward
 -
 *  Purpose:
 *      Sets the direction to Forward for FindRow()
 *
 */

void CFindDlg::OnDirForward()
{
    m_ulFlags = 0;
}


/*
 -  CFindDlg
 -  OnDirForward
 -
 *  Purpose:
 *      Sets the direction to Backward for FindRow()
 *
 */

void CFindDlg::OnDirBackward()
{
    m_ulFlags = DIR_BACKWARD;
}


/*
 -  CFindDlg
 -  OnDirForward
 -
 *  Purpose:
 *      Sets the direction to be invalid for FindRow()*
 */

void CFindDlg::OnDirInvalid()
{
    m_ulFlags = 42;
}


/*
 -  CSeekDlg
 -  OnBkBeginning
 -
 *  Purpose:
 *      Sets the Beginning bookmark.
 *
 */

void CFindDlg::OnBkBeginning()
{
    m_nBk = 0;
}


/*
 -  CSeekDlg
 -  OnBkBeginning
 -
 *  Purpose:
 *      Sets the Current bookmark.
 *
 */

void CFindDlg::OnBkCurrent()
{
    m_nBk = 1;
}


/*
 -  CSeekDlg
 -  OnBkBeginning
 -
 *  Purpose:
 *      Sets the End bookmark.
 *
 */

void CFindDlg::OnBkEnd()
{
    m_nBk = 2;
}


/*
 -  CSeekDlg
 -  OnBkBeginning
 -
 *  Purpose:
 *      Sets the user bookmark.
 *
 */

void CFindDlg::OnBkUser()
{
    m_nBk = 3;
}


void CFindDlg::OnOK()
{
    SCODE       sc;
    DWORD       dwIndex;
    BOOKMARK    bk;

    switch(m_nBk)
    {
    case 0:
        bk = BOOKMARK_BEGINNING;
        break;

    case 1:
        bk = BOOKMARK_CURRENT;
        break;

    case 2:
        bk = BOOKMARK_END;
        break;

    case 3:
        dwIndex = SendDlgItemMessage(IDC_BKSEEK, LB_GETCURSEL, 0, 0L);

        bk = m_lpBkList->bkList[dwIndex].bk;
        break;
    }

    if( FAILED(sc = GetScode(hError = m_lpTable->FindRow(m_lpFindRes, bk, m_ulFlags))) )
        SetStatus("FindRow failed", sc);

    PvFree(m_lpFindRes);

    EndDialog(IDOK);
}


void CFindDlg::OnCancel()
{
    PvFree(m_lpFindRes);
    EndDialog(IDCANCEL);
}



/*---------------------------------*/
/* CRestriction Message Handlers   */
/*---------------------------------*/

BEGIN_MESSAGE_MAP(CAcceptRestrictionDlg, CModalDialog)
    ON_COMMAND(IDMODIFY,    OnModify)
END_MESSAGE_MAP()

void CAcceptRestrictionDlg::OnModify()
{
    EndDialog(IDCANCEL);
}

/********************************************************************/
/*
 -  CAcceptRestrictionDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *
 */
/********************************************************************/

BOOL CAcceptRestrictionDlg::OnInitDialog()
{
    int             rgTabStops[12];
    DWORD           dwReturn        = 0;      
        
    SendDlgItemMessage(IDC_ACCEPTRES,LB_RESETCONTENT,0,0);

    rgTabStops[0] = 20;
    rgTabStops[1] = 30;
    rgTabStops[2] = 40;
    rgTabStops[3] = 50;
    rgTabStops[4] = 60;
    rgTabStops[5] = 70;
    rgTabStops[6] = 80;
    rgTabStops[7] = 90;
    rgTabStops[8] = 100;
    rgTabStops[9] = 110;
    rgTabStops[10]= 120;

    dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_SETTABSTOPS,
                    (WPARAM) 11,(LPARAM)rgTabStops );

    DisplayRestriction(m_prest);

    return TRUE;    
}

/*******************************************************************/
/*
 -  CAcceptRestrictionDlg::
 -  DisplayRestriction
 *
 *  Purpose:
 *
 *  Parameters:
 *
 *  Returns:
 *
 */
/*******************************************************************/

void CAcceptRestrictionDlg::DisplayRestriction(LPSRestriction lpRes)
{
    DWORD           dwReturn        = 0;      
    char            szBuff1[256];
    char            szBuff2[256];
    char            szBuff3[256];
    char            szBuffer[1024];
    ULONG           i = 0;
    static  ULONG   cTabs = 0;
    static  char    szTabs[11][22] = {"",
                     "\t",
                     "\t\t",
                     "\t\t\t",
                     "\t\t\t\t",
                     "\t\t\t\t\t",
                     "\t\t\t\t\t\t",
                     "\t\t\t\t\t\t\t",
                     "\t\t\t\t\t\t\t\t",
                     "\t\t\t\t\t\t\t\t\t",
                     "\t\t\t\t\t\t\t\t\t\t"};

    if(!lpRes)
    {
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)(char *)"lpRestriction == NULL"); 
        return;
    }        

    switch(lpRes->rt)
    {
    case RES_CONTENT:       
        wsprintf(szBuffer,"%sSContentRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        GetString("FuzzyLevel", lpRes->res.resContent.ulFuzzyLevel, szBuff1);
        wsprintf(szBuffer,"%sFuzzy Level: %s", szTabs[cTabs+1], szBuff1);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        GetString("PropTags", lpRes->res.resContent.ulPropTag, szBuff1);
        wsprintf(szBuffer,"%s%s contains %s", szTabs[cTabs+1], szBuff1,
            SzGetPropValue(szBuff2, lpRes->res.resContent.lpProp));
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_PROPERTY:
        wsprintf(szBuffer,"%sSPropertyRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        GetString("PropTags", lpRes->res.resProperty.ulPropTag, szBuff1);
        GetString("RelOp", lpRes->res.resProperty.relop, szBuff2);
        wsprintf(szBuffer,"%s%s %s %s", szTabs[cTabs+1], szBuff1, szBuff2,
            SzGetPropValue(szBuff3, lpRes->res.resProperty.lpProp));
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_COMPAREPROPS:
        wsprintf(szBuffer,"%sSComparePropsRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        GetString("PropTags", lpRes->res.resCompareProps.ulPropTag1, szBuff1);
        GetString("RelOp", lpRes->res.resCompareProps.relop, szBuff2);
        GetString("PropTags", lpRes->res.resCompareProps.ulPropTag2, szBuff3);              
        wsprintf(szBuffer,"%s%s %s %s", szTabs[cTabs+1], szBuff1, szBuff2, szBuff3);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_BITMASK:
        wsprintf(szBuffer,"%sSBitMaskRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        GetString("PropTags", lpRes->res.resBitMask.ulPropTag, szBuff1);
        GetString("Bmr", lpRes->res.resBitMask.relBMR, szBuff2);        
        wsprintf(szBuffer,"%s(%s & 0x%08lX) %s", szTabs[cTabs+1], szBuff1,
                 lpRes->res.resBitMask.ulMask, szBuff2);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_SIZE:
        wsprintf(szBuffer,"%sSSizeRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        GetString("PropTags", lpRes->res.resSize.ulPropTag, szBuff1);
        GetString("RelOp", lpRes->res.resSize.relop, szBuff2);
        wsprintf(szBuffer,"%ssizeof(%s) %s %lu", szTabs[cTabs+1], szBuff1, szBuff2, 
                 lpRes->res.resSize.cb);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_EXIST:
        wsprintf(szBuffer,"%sSExistRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        GetString("PropTags", lpRes->res.resExist.ulPropTag, szBuff1); 
        wsprintf(szBuffer,"%s%s Exists", szTabs[cTabs+1], szBuff1);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;

    case RES_SUBRESTRICTION:
        wsprintf(szBuffer,"%sSSubRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s ulSubObject: %s", szTabs[cTabs+1],
            (lpRes->res.resSub.ulSubObject == PR_MESSAGE_ATTACHMENTS) 
                        ? "PR_MESSAGE_ATTACHMENTS" :
                          "PR_MESSAGE_RECIPIENTS"     );
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        DisplayRestriction(lpRes->res.resSub.lpRes);
        cTabs--;
        break;

    case RES_NOT:
        wsprintf(szBuffer,"%sNot:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        DisplayRestriction(lpRes->res.resNot.lpRes);
        cTabs--;
        break;

    case RES_AND:
        wsprintf(szBuffer,"%sAnd:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        for(i = 0; i < lpRes->res.resAnd.cRes; i++)
        {
            DisplayRestriction(&lpRes->res.resAnd.lpRes[i]);
        }
        cTabs--;
        break;

    case RES_OR:
        wsprintf(szBuffer,"%sOr:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        for(i = 0; i < lpRes->res.resOr.cRes; i++)
        {
            DisplayRestriction(&lpRes->res.resOr.lpRes[i]);
        }
        cTabs--;
        break;

    case RES_COMMENT:
        wsprintf(szBuffer,"%sSCommentRestriction:", szTabs[cTabs]);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        wsprintf(szBuffer,"%s cValues: %lu, lpPropValue: 0x%08X", szTabs[cTabs+1],
            lpRes->res.resComment.cValues,
            lpRes->res.resComment.lpProp);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        cTabs++;
        DisplayRestriction(lpRes->res.resComment.lpRes);
        cTabs--;
        break;

    default:
        wsprintf(szBuffer,"%sUNKNOWN RES TYPE lpRes->rt == %lu",szTabs[cTabs] , lpRes->rt);
        dwReturn = SendDlgItemMessage(IDC_ACCEPTRES,LB_ADDSTRING,0,(LPARAM)szBuffer);   
        break;
    }
    return;
}

/*******************************************************************/
/*
 -  CAcceptRestrictionDlg::
 -  ~CAcceptRestrictionDlg
 -
 *  Purpose:
 *      Destructor for class CAcceptRestrictionDlg. Releases and Frees memory
 *      allocated in this class
 *
 */
/*******************************************************************/

CAcceptRestrictionDlg::~CAcceptRestrictionDlg()
{

} 



////////////////////////////////////////////////////



/*-----------------------------*/
/* CResDlg Message Handlers    */
/*-----------------------------*/

BEGIN_MESSAGE_MAP(CResDlg, CModalDialog)
    ON_COMMAND(IDC_AND,      OnAnd)
    ON_COMMAND(IDC_OR,       OnOr)
    ON_COMMAND(IDC_SUBRES1,  OnSubRes1)
    ON_COMMAND(IDC_SUBRES2,  OnSubRes2)
    ON_COMMAND(IDC_SUBRES3,  OnSubRes3)
    ON_CBN_SELCHANGE(IDC_RESTYPE1, OnResType1)
    ON_CBN_SELCHANGE(IDC_RESTYPE2, OnResType2)
    ON_CBN_SELCHANGE(IDC_RESTYPE3, OnResType3)
END_MESSAGE_MAP()

/*
 -  CResDlg::
 -  CResDlg
 *
 *  Purpose:
 *      Constructor for the CResDlg class.
 */

CResDlg::CResDlg(LPSPropTagArray lpspta, LPSRestriction lpr, CWnd* lpParent)
        : CModalDialog(IDD_RESDLG, lpParent)
{
    m_lpCurColumns = lpspta;
    m_lpRes        = lpr;

    m_lpSubRes = (LPSRestriction)PvAlloc(3 * sizeof(SRestriction));
}


/*
 -  CResDlg::
 -  OnInitDialog
 -
 *  Purpose:
 *      Constructor for main dialog class.
 *
 */

BOOL CResDlg::OnInitDialog()
{
    ULONG   idx;
    char    szBuff[512];
    char    szBuffer[256];
    ULONG   ulNum;

    CheckDlgButton(IDC_AND, TRUE);

    ulNumResTypes = GetRowCount("RestrictionType");

    for(idx = 0; idx < ulNumResTypes; idx++)
    {
        GetRowString("RestrictionType",idx,szBuffer);
        SendDlgItemMessage(IDC_RESTYPE1, CB_ADDSTRING, 0,
                (LPARAM)szBuffer);
        SendDlgItemMessage(IDC_RESTYPE2, CB_ADDSTRING, 0,
                (LPARAM)szBuffer);
        SendDlgItemMessage(IDC_RESTYPE3, CB_ADDSTRING, 0,
                (LPARAM)szBuffer);
    }

    SendDlgItemMessage(IDC_RESTYPE1, CB_ADDSTRING, 0,
                (LPARAM)(char *)"None");
    SendDlgItemMessage(IDC_RESTYPE2, CB_ADDSTRING, 0,
                (LPARAM)(char *)"None");
    SendDlgItemMessage(IDC_RESTYPE3, CB_ADDSTRING, 0,
                (LPARAM)(char *)"None");

    SendDlgItemMessage(IDC_RESTYPE1, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
    SendDlgItemMessage(IDC_RESTYPE2, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
    SendDlgItemMessage(IDC_RESTYPE3, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);

    for(idx = 0; idx < m_lpCurColumns->cValues; idx++)
    {
        GetString("PropTags", m_lpCurColumns->aulPropTag[idx], szBuff);

        SendDlgItemMessage(IDC_PROPTAG1, CB_ADDSTRING, 0, (LPARAM)szBuff);
        SendDlgItemMessage(IDC_PROPTAG2, CB_ADDSTRING, 0, (LPARAM)szBuff);
        SendDlgItemMessage(IDC_PROPTAG3, CB_ADDSTRING, 0, (LPARAM)szBuff);
    }

    ulNum = GetRowCount("RelOp");

    for(idx = 0; idx < ulNum; idx++)
    {
        GetRowString("RelOp",idx,szBuffer);
  
        SendDlgItemMessage(IDC_RELATIONSHIP1, CB_ADDSTRING, 0,
                (LPARAM)szBuffer);
        SendDlgItemMessage(IDC_RELATIONSHIP2, CB_ADDSTRING, 0,
                (LPARAM)szBuffer);
        SendDlgItemMessage(IDC_RELATIONSHIP3, CB_ADDSTRING, 0,
                (LPARAM)szBuffer);
    }

    /* Hide everything until the Restriction Type is chosen
       Restriction 1 */
    GetDlgItem(IDT_PROPTAG1)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG1)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
    GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(FALSE);
    GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(FALSE);
    GetDlgItem(IDT_VALUE1)->ShowWindow(FALSE);
    GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);

    /* Restriction 2 */
    GetDlgItem(IDT_PROPTAG2)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG2)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
    GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(FALSE);
    GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(FALSE);
    GetDlgItem(IDT_VALUE2)->ShowWindow(FALSE);
    GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);

    /* Restriction 3 */
    GetDlgItem(IDT_PROPTAG3)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG3)->ShowWindow(FALSE);
    GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
    GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(FALSE);
    GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(FALSE);
    GetDlgItem(IDT_VALUE3)->ShowWindow(FALSE);
    GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);

    m_fComb = 0;
    m_nResType1 = (int) ulNumResTypes;
    m_nResType2 = (int) ulNumResTypes;
    m_nResType3 = (int) ulNumResTypes;

    GetDlgItem(IDC_AND)->SetFocus();

    return TRUE;
}


/*
 -  CResDlg
 -  OnAnd
 -
 *  Purpose:
 *      Sets the AND restriction flag.
 *
 */

void CResDlg::OnAnd()
{
    m_fComb = 0;
}


/*
 -  CResDlg
 -  OnOr
 -
 *  Purpose:
 *      Sets the OR restriction flag.
 *
 */

void CResDlg::OnOr()
{
    m_fComb = 1;
}

/*
 -  CResDlg
 -  OnResType1
 -
 *  Purpose:
 *      Changes the view of the restriction based on the type for
 *      Restriction 1.
 *
 */

void CResDlg::OnResType1()
{
    INT     idx;
    LONG    nNewResType;
    char    szBuff[128];
    char    szBuffer[256];
    ULONG   ulNum   = 0;

    nNewResType = SendDlgItemMessage(IDC_RESTYPE1, CB_GETCURSEL, 0, 0L);

    if(nNewResType == m_nResType1)
        return;

    switch(nNewResType)
    {
    case RES_AND:
    case RES_OR:
    case RES_NOT:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(TRUE);
        break;

    case RES_CONTENT:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
        
        GetDlgItem(IDT_VALUE1)->SetWindowText("Value:");
        GetDlgItem(IDT_PROPTAG1)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP1)->SetWindowText("Fuzzy Level:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP1, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("FuzzyLevel");

        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("FuzzyLevel",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP1, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_PROPERTY:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
        
        GetDlgItem(IDT_VALUE1)->SetWindowText("Value:");
        GetDlgItem(IDT_PROPTAG1)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP1)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP1, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP1, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }
        break;

    case RES_COMPAREPROPS:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(TRUE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
        
        GetDlgItem(IDT_PROPTAG1)->SetWindowText("PropTag1:");
        GetDlgItem(IDT_VALUE1)->SetWindowText("PropTag2:");
        GetDlgItem(IDT_RELATIONSHIP1)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP1, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP1, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        // update second PROPS
        SendDlgItemMessage(IDC_PROPTAG12, CB_RESETCONTENT, 0, (LPARAM)szBuff);
        for(idx = 0; idx < m_lpCurColumns->cValues; idx++)
        {
            GetString("PropTags", m_lpCurColumns->aulPropTag[idx], szBuff);
            SendDlgItemMessage(IDC_PROPTAG12, CB_ADDSTRING, 0, (LPARAM)szBuff);
        }
        break;

    case RES_BITMASK:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);

        GetDlgItem(IDT_VALUE1)->SetWindowText("Mask: ex. 0x00000678");
        GetDlgItem(IDT_PROPTAG1)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP1)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP1, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("Bmr");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("Bmr",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP1, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }
        break;

    case RES_SUBRESTRICTION:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(TRUE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(TRUE);

        GetDlgItem(IDT_VALUE1)->SetWindowText("ulSubObj:");

        // DIFFERENT SPECIAL PROPERTIES
        SendDlgItemMessage(IDC_PROPTAG12, CB_RESETCONTENT, 0, 0L);
        SendDlgItemMessage(IDC_PROPTAG12, CB_ADDSTRING,    0, (LPARAM)(char *)"PR_MESSAGE_ATTACHMENTS");
        SendDlgItemMessage(IDC_PROPTAG12, CB_ADDSTRING,    0, (LPARAM)(char *)"PR_MESSAGE_RECIPIENTS");

        break;

    case RES_SIZE:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);

        GetDlgItem(IDT_PROPTAG1)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP1)->SetWindowText("Relationship:");
        GetDlgItem(IDT_VALUE1)->SetWindowText("cb:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP1, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP1, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_EXIST:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);

        GetDlgItem(IDT_PROPTAG1)->SetWindowText("PropTag:");
        break;

    case RES_COMMENT:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(TRUE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(TRUE);

        GetDlgItem(IDT_VALUE1)->SetWindowText("cValues:");

        break;

    
    default:
        GetDlgItem(IDT_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG1)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP1)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
    }

    m_nResType1 = (int)nNewResType;
}


/*
 -  CResDlg
 -  OnResType2
 -
 *  Purpose:
 *      Changes the view of the restriction based on the type for
 *      Restriction 2.
 *
 */

void CResDlg::OnResType2()
{
    INT     idx;
    LONG    nNewResType;
    char    szBuff[128];
    char    szBuffer[256];
    ULONG   ulNum   = 0;

    nNewResType = SendDlgItemMessage(IDC_RESTYPE2, CB_GETCURSEL, 0, 0L);

    if(nNewResType == m_nResType2)
        return;

    switch(nNewResType)
    {
    case RES_AND:
    case RES_OR:
    case RES_NOT:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES2)->EnableWindow(TRUE);
        break;

    case RES_CONTENT:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(TRUE);

        GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->SetWindowText("Value:");
        GetDlgItem(IDT_PROPTAG2)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP2)->SetWindowText("Fuzzy Level:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP2, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("FuzzyLevel");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("FuzzyLevel",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP2, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }
        break;

    case RES_PROPERTY:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(TRUE);

        GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->SetWindowText("Value:");
        GetDlgItem(IDT_PROPTAG2)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP2)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP2, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP2, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_COMPAREPROPS:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(TRUE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);

        GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);
        GetDlgItem(IDT_PROPTAG2)->SetWindowText("PropTag1:");
        GetDlgItem(IDT_VALUE2)->SetWindowText("PropTag2:");
        GetDlgItem(IDT_RELATIONSHIP2)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP2, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP2, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        // update 2nd PROPS
        SendDlgItemMessage(IDC_PROPTAG22, CB_RESETCONTENT, 0, (LPARAM)szBuff);
        for(idx = 0; idx < m_lpCurColumns->cValues; idx++)
        {
            GetString("PropTags", m_lpCurColumns->aulPropTag[idx], szBuff);
            SendDlgItemMessage(IDC_PROPTAG22, CB_ADDSTRING, 0, (LPARAM)szBuff);
        }
        break;

    case RES_BITMASK:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(TRUE);

        GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->SetWindowText("Mask: ex. 0x00000678");
        GetDlgItem(IDT_PROPTAG2)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP2)->SetWindowText("Relationship:");


        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP2, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("Bmr");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("Bmr",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP2, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_SUBRESTRICTION:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(TRUE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES2)->EnableWindow(TRUE);

        GetDlgItem(IDT_VALUE2)->SetWindowText("ulSubObj:");

        // DIFFERENT SPECIAL PROPERTIES
        SendDlgItemMessage(IDC_PROPTAG22, CB_RESETCONTENT, 0, 0L);
        SendDlgItemMessage(IDC_PROPTAG22, CB_ADDSTRING,    0, (LPARAM)(char *)"PR_MESSAGE_ATTACHMENTS");
        SendDlgItemMessage(IDC_PROPTAG22, CB_ADDSTRING,    0, (LPARAM)(char *)"PR_MESSAGE_RECIPIENTS");
                
        break;

    case RES_SIZE:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);

        GetDlgItem(IDT_PROPTAG2)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP2)->SetWindowText("Relationship:");
        GetDlgItem(IDT_VALUE2)->SetWindowText("cb:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP2, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP2, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_EXIST:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);

        GetDlgItem(IDT_PROPTAG2)->SetWindowText("PropTag:");
        break;


    case RES_COMMENT:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(TRUE);
        GetDlgItem(IDC_SUBRES2)->EnableWindow(TRUE);

        GetDlgItem(IDT_VALUE2)->SetWindowText("cValues:");

        break;



    default:
        GetDlgItem(IDT_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG2)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP2)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
  
    }

    m_nResType2 = (int)nNewResType;
}


/*
 -  CResDlg
 -  OnResType3
 -
 *  Purpose:
 *      Changes the view of the restriction based on the type for
 *      Restriction 3.
 *
 */

void CResDlg::OnResType3()
{
    INT     idx;
    LONG    nNewResType;
    char    szBuff[128];
    char    szBuffer[256];
    ULONG   ulNum   = 0;

    nNewResType = SendDlgItemMessage(IDC_RESTYPE3, CB_GETCURSEL, 0, 0L);

    if(nNewResType == m_nResType3)
        return;

    switch(nNewResType)
    {
    case RES_AND:
    case RES_OR:
    case RES_NOT:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES3)->EnableWindow(TRUE);
        break;

    case RES_CONTENT:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(TRUE);

        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->SetWindowText("Value:");
        GetDlgItem(IDT_PROPTAG3)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP3)->SetWindowText("Fuzzy Level:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP3, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("FuzzyLevel");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("FuzzyLevel",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP3, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_PROPERTY:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(TRUE);

        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->SetWindowText("Value:");
        GetDlgItem(IDT_PROPTAG3)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP3)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP3, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP3, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_COMPAREPROPS:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(TRUE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);

        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);
        GetDlgItem(IDT_PROPTAG3)->SetWindowText("PropTag1:");
        GetDlgItem(IDT_VALUE3)->SetWindowText("PropTag2:");
        GetDlgItem(IDT_RELATIONSHIP3)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP3, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP3, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        // update 2nd PROPS
        SendDlgItemMessage(IDC_PROPTAG32, CB_RESETCONTENT, 0, (LPARAM)szBuff);
        for(idx = 0; idx < m_lpCurColumns->cValues; idx++)
        {
            GetString("PropTags", m_lpCurColumns->aulPropTag[idx], szBuff);
            SendDlgItemMessage(IDC_PROPTAG32, CB_ADDSTRING, 0, (LPARAM)szBuff);
        }
        break;

    case RES_BITMASK:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(TRUE);

        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->SetWindowText("Mask: ex. 0x00000678");
        GetDlgItem(IDT_PROPTAG3)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP3)->SetWindowText("Relationship:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP3, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("Bmr");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("Bmr",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP3, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }

        break;

    case RES_SUBRESTRICTION:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(TRUE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES3)->EnableWindow(TRUE);
        
        GetDlgItem(IDT_VALUE3)->SetWindowText("ulSubObj:");

        // DIFFERENT SPECIAL PROPERTIES
        SendDlgItemMessage(IDC_PROPTAG32, CB_RESETCONTENT, 0, 0L);
        SendDlgItemMessage(IDC_PROPTAG32, CB_ADDSTRING,    0, (LPARAM)(char *)"PR_MESSAGE_ATTACHMENTS");
        SendDlgItemMessage(IDC_PROPTAG32, CB_ADDSTRING,    0, (LPARAM)(char *)"PR_MESSAGE_RECIPIENTS");
        
        break;

    case RES_SIZE:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(TRUE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);

        GetDlgItem(IDT_PROPTAG3)->SetWindowText("PropTag:");
        GetDlgItem(IDT_RELATIONSHIP3)->SetWindowText("Relationship:");
        GetDlgItem(IDT_VALUE3)->SetWindowText("cb:");

        // update RELATIONSHIP
        SendDlgItemMessage(IDC_RELATIONSHIP3, CB_RESETCONTENT, 0, 0L);
        ulNum = GetRowCount("RelOp");
        for(idx = 0; idx < ulNum; idx++)
        {
            GetRowString("RelOp",idx,szBuffer);
            SendDlgItemMessage(IDC_RELATIONSHIP3, CB_ADDSTRING, 0,
                    (LPARAM)szBuffer);
        }
        break;

    case RES_EXIST:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(TRUE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);

        GetDlgItem(IDT_PROPTAG3)->SetWindowText("PropTag:");
        
        break;

    case RES_COMMENT:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(TRUE);
        GetDlgItem(IDC_SUBRES3)->EnableWindow(TRUE);

        GetDlgItem(IDT_VALUE2)->SetWindowText("cValues:");
        break;


    default:
        GetDlgItem(IDT_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG3)->ShowWindow(FALSE);
        GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);
        GetDlgItem(IDT_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDC_RELATIONSHIP3)->ShowWindow(FALSE);
        GetDlgItem(IDT_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);
        GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);
    }

    m_nResType3 = (int)nNewResType;
}



/*
 -  CResDlg
 -  OnSubRes1
 -
 *  Purpose:
 *      Creates a sub-Restriction for Restriction 1.
 *
 */

void CResDlg::OnSubRes1()
{
    LPSRestriction  lpRes;
    ULONG           rt;

    rt = SendDlgItemMessage(IDC_RESTYPE1, CB_GETCURSEL, 0, 0L);

    if(rt == RES_NOT)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[0].rt = RES_NOT;
        m_lpSubRes[0].res.resNot.lpRes = lpRes;
    }
    else if(rt == RES_SUBRESTRICTION)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[0].rt = RES_SUBRESTRICTION;
        m_lpSubRes[0].res.resSub.lpRes = lpRes;
    }
    else if(rt == RES_COMMENT)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[0].rt = RES_COMMENT;
        m_lpSubRes[0].res.resComment.lpRes = lpRes;
    }
    else
    {
        lpRes = &m_lpSubRes[0];
    }

    CResDlg dlgSubRes1(m_lpCurColumns, lpRes);
    dlgSubRes1.SetWindowText("Sub Restriction");

    if(dlgSubRes1.DoModal() != IDOK) 
    {
        if(rt == RES_NOT)
        {
//            PvFree(lpRes);        
            m_lpSubRes[0].res.resNot.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE1, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
        }
        else if(rt ==  RES_SUBRESTRICTION )
        { 
//            PvFree(lpRes);        
            m_lpSubRes[0].res.resSub.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE1, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
            
            GetDlgItem(IDT_VALUE1)->ShowWindow(FALSE);
            GetDlgItem(IDC_PROPTAG12)->ShowWindow(FALSE);
        }
        else if(rt ==  RES_COMMENT )
        {
//            PvFree(lpRes);        
            m_lpSubRes[0].res.resComment.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE1, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES1)->EnableWindow(FALSE);
         
            GetDlgItem(IDT_VALUE1)->ShowWindow(FALSE);
            GetDlgItem(IDC_VALUE1)->ShowWindow(FALSE);
        }        
    }
}


/*
 -  CResDlg
 -  OnSubRes2
 -
 *  Purpose:
 *      Creates a sub-Restriction for Restriction 2.
 *
 */

void CResDlg::OnSubRes2()
{
    LPSRestriction  lpRes;
    ULONG           rt;

    rt = SendDlgItemMessage(IDC_RESTYPE2, CB_GETCURSEL, 0, 0L);

    if(rt == RES_NOT)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[1].rt = RES_NOT;
        m_lpSubRes[1].res.resNot.lpRes = lpRes;
    }
    else if(rt == RES_SUBRESTRICTION)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[1].rt = RES_SUBRESTRICTION;
        m_lpSubRes[1].res.resSub.lpRes = lpRes;
    }
    else if(rt == RES_COMMENT)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[1].rt = RES_COMMENT;
        m_lpSubRes[1].res.resComment.lpRes = lpRes;
    }
    else
    {
        lpRes = &m_lpSubRes[1];
    }

    CResDlg dlgSubRes2(m_lpCurColumns, lpRes);
    dlgSubRes2.SetWindowText("Sub Restriction");

    if(dlgSubRes2.DoModal() != IDOK) 
    {
        if(rt == RES_NOT)
        {
//            PvFree(lpRes);        
            m_lpSubRes[1].res.resNot.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE2, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);

        }
        else if(rt ==  RES_SUBRESTRICTION )
        { 
//            PvFree(lpRes);        
            m_lpSubRes[1].res.resSub.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE2, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);
            GetDlgItem(IDT_VALUE2)->ShowWindow(FALSE);
            GetDlgItem(IDC_PROPTAG22)->ShowWindow(FALSE);
        }
        else if(rt ==  RES_COMMENT )
        {
//            PvFree(lpRes);        
            m_lpSubRes[1].res.resComment.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE2, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES2)->EnableWindow(FALSE);

            GetDlgItem(IDT_VALUE2)->ShowWindow(FALSE);
            GetDlgItem(IDC_VALUE2)->ShowWindow(FALSE);
        }        
    }
}


/*
 -  CResDlg
 -  OnSubRes3
 -
 *  Purpose:
 *      Creates a sub-Restriction for Restriction 3.
 *
 */

void CResDlg::OnSubRes3()
{
    LPSRestriction  lpRes;
    ULONG           rt;

    rt = SendDlgItemMessage(IDC_RESTYPE3, CB_GETCURSEL, 0, 0L);

    if(rt == RES_NOT)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[2].rt = RES_NOT;
        m_lpSubRes[2].res.resNot.lpRes = lpRes;
    }
    else if(rt == RES_SUBRESTRICTION)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[2].rt = RES_SUBRESTRICTION;
        m_lpSubRes[2].res.resSub.lpRes = lpRes;
    }
    else if(rt == RES_COMMENT)
    {
        lpRes = (LPSRestriction)PvAllocMore(sizeof(SRestriction), m_lpSubRes);

        m_lpSubRes[2].rt = RES_COMMENT;
        m_lpSubRes[2].res.resComment.lpRes = lpRes;
    }
    else
    {
        lpRes = &m_lpSubRes[2];
    }

    CResDlg dlgSubRes3(m_lpCurColumns, lpRes);
    dlgSubRes3.SetWindowText("Sub Restriction");


    if(dlgSubRes3.DoModal() != IDOK) 
    {
        if(rt == RES_NOT)
        {
//            PvFree(lpRes);        
            m_lpSubRes[2].res.resNot.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE3, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);

        }
        else if(rt ==  RES_SUBRESTRICTION )
        { 
//            PvFree(lpRes);        can't free PvAllocMore buffer
            m_lpSubRes[2].res.resSub.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE3, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);

            GetDlgItem(IDT_VALUE3)->ShowWindow(FALSE);
            GetDlgItem(IDC_PROPTAG32)->ShowWindow(FALSE);

        }                         
        else if(rt ==  RES_COMMENT )
        {
//            PvFree(lpRes);        
            m_lpSubRes[2].res.resComment.lpRes = NULL;
            SendDlgItemMessage(IDC_RESTYPE3, CB_SETCURSEL, (WPARAM) ulNumResTypes, 0L);
            GetDlgItem(IDC_SUBRES3)->EnableWindow(FALSE);

            GetDlgItem(IDT_VALUE3)->ShowWindow(FALSE);
            GetDlgItem(IDC_VALUE3)->ShowWindow(FALSE);
        }        
    }
}


/*
 -  CResDlg
 -  OnOK
 -
 *  Purpose:
 *      Collects the restiction and sub-restriction information and does
 *      the appropriate assignments.  This function performs the assignments
 *      for all three restrictions.
 *
 */

void CResDlg::OnOK()
{
    ULONG           cRes = 1;
    char            szBuff[1024];
    char            *szEnd      = NULL;
    LPSPropValue    lpProp;
    ULONG           idx;
    BOOL            fTrans;
    LONG            lSelection   = 0;
    ULONG           i = 0;

    m_lpRes->rt = m_fComb;
    m_lpRes->res.resAnd.cRes = 0;
    m_lpRes->res.resAnd.lpRes = m_lpSubRes;

    /* Collect values from Restriction 1 group */
    switch(m_lpSubRes[0].rt = SendDlgItemMessage(IDC_RESTYPE1,
            CB_GETCURSEL, 0, 0L))
    {
    case RES_AND:
    case RES_OR:
    case RES_NOT:
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_CONTENT:
        idx = SendDlgItemMessage(IDC_PROPTAG1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resContent.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resContent.ulFuzzyLevel = GetRowID("FuzzyLevel",lSelection);
        
        SendDlgItemMessage(IDC_VALUE1, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);
        lpProp = (LPSPropValue)PvAllocMore(sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        MakePropValue(lpProp, PROP_TAG(PT_TSTRING, 0x0000), szBuff, m_lpSubRes);

        m_lpSubRes[0].res.resContent.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_PROPERTY:
        idx = SendDlgItemMessage(IDC_PROPTAG1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resProperty.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resProperty.relop  = GetRowID("RelOp",lSelection);
        
        SendDlgItemMessage(IDC_VALUE1, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);

        lpProp = (LPSPropValue)PvAllocMore(sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        MakePropValue(lpProp,m_lpCurColumns->aulPropTag[idx] , szBuff, m_lpSubRes);
        
        m_lpSubRes[0].res.resProperty.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_COMPAREPROPS:
        idx = SendDlgItemMessage(IDC_PROPTAG1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resCompareProps.ulPropTag1 = m_lpCurColumns->aulPropTag[idx];

        idx = SendDlgItemMessage(IDC_PROPTAG12, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resCompareProps.ulPropTag2 = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resCompareProps.relop  = GetRowID("RelOp",lSelection);

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_BITMASK:
        idx = SendDlgItemMessage(IDC_PROPTAG1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resBitMask.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resBitMask.relBMR  = GetRowID("Bmr",lSelection);

        m_lpSubRes[0].res.resBitMask.relBMR = SendDlgItemMessage(IDC_RELATIONSHIP1,
                CB_GETCURSEL, 0, 0L);

        SendDlgItemMessage(IDC_VALUE1, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);

        m_lpSubRes[0].res.resBitMask.ulMask = strtoul(szBuff,&szEnd,16);
        
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_SIZE:
    
        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resSize.relop  = GetRowID("RelOp",lSelection);
    
        idx = SendDlgItemMessage(IDC_PROPTAG1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resSize.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        m_lpSubRes[0].res.resSize.cb = (ULONG)GetDlgItemInt(IDC_VALUE1, &fTrans , FALSE );

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_EXIST:
        idx = SendDlgItemMessage(IDC_PROPTAG1, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[0].res.resExist.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_SUBRESTRICTION:
        idx = SendDlgItemMessage(IDC_PROPTAG12, CB_GETCURSEL, 0, 0L);
        if( idx == 1 ) 
            m_lpSubRes[0].res.resSub.ulSubObject = PR_MESSAGE_RECIPIENTS;
        else
            m_lpSubRes[0].res.resSub.ulSubObject = PR_MESSAGE_ATTACHMENTS;

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_COMMENT:
        m_lpSubRes[0].res.resComment.cValues = (ULONG)GetDlgItemInt(IDC_VALUE1, &fTrans , FALSE );

        lpProp = (LPSPropValue)PvAllocMore( m_lpSubRes[0].res.resComment.cValues * sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        for( i = 0 ; i < m_lpSubRes[0].res.resComment.cValues ; i++)
            MakePropValue( &(lpProp[i]),PR_SUBJECT, "TEST STRING", m_lpSubRes);

        m_lpSubRes[0].res.resComment.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    default:
        break;
    }

    /* Collect values from Restriction 2 group */
    switch(m_lpSubRes[1].rt = SendDlgItemMessage(IDC_RESTYPE2,
            CB_GETCURSEL, 0, 0L))
    {
    case RES_AND:
    case RES_OR:
    case RES_NOT:
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_CONTENT:
        idx = SendDlgItemMessage(IDC_PROPTAG2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resContent.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resContent.ulFuzzyLevel = GetRowID("FuzzyLevel",lSelection);

        SendDlgItemMessage(IDC_VALUE2, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);
        lpProp = (LPSPropValue)PvAllocMore(sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        MakePropValue(lpProp, PROP_TAG(PT_TSTRING, 0x0000), szBuff, m_lpSubRes);

        m_lpSubRes[1].res.resContent.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_PROPERTY:
        idx = SendDlgItemMessage(IDC_PROPTAG2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resProperty.ulPropTag = m_lpCurColumns->aulPropTag[idx];


        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resProperty.relop  = GetRowID("RelOp",lSelection);

        SendDlgItemMessage(IDC_VALUE2, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);
        lpProp = (LPSPropValue)PvAllocMore(sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        MakePropValue(lpProp,m_lpCurColumns->aulPropTag[idx] , szBuff, m_lpSubRes);

        m_lpSubRes[1].res.resProperty.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_COMPAREPROPS:
        idx = SendDlgItemMessage(IDC_PROPTAG2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resCompareProps.ulPropTag1 = m_lpCurColumns->aulPropTag[idx];

        idx = SendDlgItemMessage(IDC_PROPTAG22, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resCompareProps.ulPropTag2 = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resCompareProps.relop  = GetRowID("RelOp",lSelection);

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_BITMASK:
        idx = SendDlgItemMessage(IDC_PROPTAG2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resBitMask.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resBitMask.relBMR  = GetRowID("Bmr",lSelection);

        SendDlgItemMessage(IDC_VALUE2, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);

        m_lpSubRes[1].res.resBitMask.ulMask = strtoul(szBuff,&szEnd,16);

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_SIZE:

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resSize.relop  = GetRowID("RelOp",lSelection);

        idx = SendDlgItemMessage(IDC_PROPTAG2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resSize.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        m_lpSubRes[1].res.resSize.cb = (ULONG)GetDlgItemInt(IDC_VALUE2, &fTrans , FALSE );

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_EXIST:
        idx = SendDlgItemMessage(IDC_PROPTAG2, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[1].res.resExist.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_SUBRESTRICTION:
        idx = SendDlgItemMessage(IDC_PROPTAG22, CB_GETCURSEL, 0, 0L);
        if( idx == 1 ) 
            m_lpSubRes[1].res.resSub.ulSubObject = PR_MESSAGE_RECIPIENTS;
        else
            m_lpSubRes[1].res.resSub.ulSubObject = PR_MESSAGE_ATTACHMENTS;

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_COMMENT:
        m_lpSubRes[1].res.resComment.cValues = (ULONG)GetDlgItemInt(IDC_VALUE2, &fTrans , FALSE );

        lpProp = (LPSPropValue)PvAllocMore( m_lpSubRes[1].res.resComment.cValues * sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        for( i = 0 ; i < m_lpSubRes[1].res.resComment.cValues ; i++)
            MakePropValue( &(lpProp[i]),PR_SUBJECT, "TEST STRING", m_lpSubRes);

        m_lpSubRes[1].res.resComment.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    default:
        break;
    }

    /* Collect values from Restriction 3 group */
    switch(m_lpSubRes[2].rt = SendDlgItemMessage(IDC_RESTYPE3,
            CB_GETCURSEL, 0, 0L))
    {
    case RES_AND:
    case RES_OR:
    case RES_NOT:
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_CONTENT:
        idx = SendDlgItemMessage(IDC_PROPTAG3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resContent.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resContent.ulFuzzyLevel = GetRowID("FuzzyLevel",lSelection);

        SendDlgItemMessage(IDC_VALUE3, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);
        lpProp = (LPSPropValue)PvAllocMore(sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        MakePropValue(lpProp, PROP_TAG(PT_TSTRING, 0x0000), szBuff, m_lpSubRes);

        m_lpSubRes[2].res.resContent.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_PROPERTY:
        idx = SendDlgItemMessage(IDC_PROPTAG3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resProperty.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resProperty.relop  = GetRowID("RelOp",lSelection);

        SendDlgItemMessage(IDC_VALUE3, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);
        lpProp = (LPSPropValue)PvAllocMore(sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        MakePropValue(lpProp,m_lpCurColumns->aulPropTag[idx] , szBuff, m_lpSubRes);

        m_lpSubRes[2].res.resProperty.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_COMPAREPROPS:
        idx = SendDlgItemMessage(IDC_PROPTAG3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resCompareProps.ulPropTag1 = m_lpCurColumns->aulPropTag[idx];

        idx = SendDlgItemMessage(IDC_PROPTAG32, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resCompareProps.ulPropTag2 = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resCompareProps.relop  = GetRowID("RelOp",lSelection);

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_BITMASK:
        idx = SendDlgItemMessage(IDC_PROPTAG3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resBitMask.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resBitMask.relBMR  = GetRowID("Bmr",lSelection);

        SendDlgItemMessage(IDC_VALUE3, WM_GETTEXT, sizeof(szBuff),
                (LPARAM)szBuff);

        m_lpSubRes[2].res.resBitMask.ulMask = strtoul(szBuff,&szEnd,16);

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_SIZE:
        lSelection = SendDlgItemMessage(IDC_RELATIONSHIP3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resSize.relop  = GetRowID("RelOp",lSelection);

        idx = SendDlgItemMessage(IDC_PROPTAG3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resSize.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        m_lpSubRes[2].res.resSize.cb = (ULONG)GetDlgItemInt(IDC_VALUE3, &fTrans , FALSE );

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_EXIST:
        idx = SendDlgItemMessage(IDC_PROPTAG3, CB_GETCURSEL, 0, 0L);
        m_lpSubRes[2].res.resExist.ulPropTag = m_lpCurColumns->aulPropTag[idx];

        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_SUBRESTRICTION:
        idx = SendDlgItemMessage(IDC_PROPTAG32, CB_GETCURSEL, 0, 0L);
        if( idx == 1 ) 
            m_lpSubRes[2].res.resSub.ulSubObject = PR_MESSAGE_RECIPIENTS;
        else
            m_lpSubRes[2].res.resSub.ulSubObject = PR_MESSAGE_ATTACHMENTS;
       
        m_lpRes[0].res.resAnd.cRes++;
        break;

    case RES_COMMENT:
        m_lpSubRes[2].res.resComment.cValues = (ULONG)GetDlgItemInt(IDC_VALUE3, &fTrans , FALSE );

        lpProp = (LPSPropValue)PvAllocMore( m_lpSubRes[2].res.resComment.cValues * sizeof(SPropValue),
                (LPVOID)m_lpSubRes);

        for( i = 0 ; i < m_lpSubRes[2].res.resComment.cValues ; i++)
            MakePropValue( &(lpProp[i]),PR_SUBJECT, "TEST STRING", m_lpSubRes);

        m_lpSubRes[2].res.resComment.lpProp = lpProp;
        m_lpRes[0].res.resAnd.cRes++;
        break;

    default:
        break;
    }

    // display restriction
    CAcceptRestrictionDlg Res(this);
    
    Res.m_prest = m_lpRes;
    if(Res.DoModal() == IDOK)
        EndDialog(IDOK);        
}


void CResDlg::OnCancel()
{
    PvFree(m_lpSubRes);
    EndDialog(IDCANCEL);
}


/*-------------------------*/
/* Misc. Support Functions */
/*-------------------------*/

/*
 -  AddBookmark
 -
 *  Purpose:
 *      Adds a bookmark to the internal list.
 *
 */

void AddBookmark(LPBKLIST lpBkList, BOOKMARK bk, LPSPropValue lpProp)
{
    LPSPropValue    lpNewProp;

    ASSERT(lpBkList->cValues < BKLIST_MAX);

    /* We associate a PropValue with a bookmark for display */
    /* purposes.  We need to make a copy of this PropValue. */
    lpNewProp = (LPSPropValue)PvAlloc(sizeof(SPropValue));

    CopyPropValue(lpNewProp, lpProp, lpNewProp);

    /* New bookmarks go in at bottom of list */
    lpBkList->bkList[lpBkList->cValues].bk = bk;
    lpBkList->bkList[lpBkList->cValues++].lpProp = lpNewProp;
}


/*
 -  RemoveBookmark
 -
 *  Purpose:
 *      Removes a bookmark from the internal list.
 *
 */

void RemoveBookmark(LPBKLIST lpBkList, DWORD idxBk)
{
    DWORD   idx;

    ASSERT(lpBkList->cValues);
    ASSERT(idxBk < lpBkList->cValues);

    /* Free the PropValue associated with this bookmark */
     PvFree(lpBkList->bkList[idxBk].lpProp);

    /* Decrement the current count of bookmarks */
    lpBkList->cValues--;

    /* Shift remaining values up one */
    for(idx = idxBk; idx < lpBkList->cValues; idx++)
    {
        lpBkList->bkList[idx].bk = lpBkList->bkList[idx+1].bk;
        lpBkList->bkList[idx].lpProp = lpBkList->bkList[idx+1].lpProp;
    }
}


/*
 -  FreeRestriction
 -
 *  Purpose:
 *      Free the memory of a restriciton.
 */
void FreeRestriction(LPSRestriction lpRes)
{
    ULONG   idx;

    if(!lpRes)
        return;

    if(lpRes->rt == RES_AND || lpRes->rt == RES_OR)
    {
        for(idx = 0; idx < lpRes->res.resAnd.cRes; idx++)
        {
            if(lpRes->res.resAnd.lpRes[idx].rt ==  RES_AND ||
               lpRes->res.resAnd.lpRes[idx].rt == RES_OR)
                FreeRestriction(&lpRes->res.resAnd.lpRes[idx]);

            if(lpRes->res.resAnd.lpRes[idx].rt == RES_NOT)
                FreeRestriction(lpRes->res.resAnd.lpRes[idx].res.resNot.lpRes);
        }

        PvFree(lpRes->res.resAnd.lpRes);
    }
}



/*--------------------*/
/* Library Init stuff */
/*--------------------*/

class CTblViewDLL : public CWinApp
{
public:
    virtual BOOL InitInstance();
    virtual BOOL ExitInstance();

    CTblViewDLL(const char *pszAppName)
            : CWinApp(pszAppName)
        {
        }
};

BOOL CTblViewDLL::InitInstance()
{
    SetDialogBkColor();
    return TRUE;
}


BOOL CTblViewDLL::ExitInstance()
{
    return TRUE;
}

CTblViewDLL  vtDLL("tblvu32.dll");
