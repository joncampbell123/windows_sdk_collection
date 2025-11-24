/*
 -  T B L L I S T . C P P
 -  Copyright (C) 1995 Microsoft Corporation
 -
 *
 */

#include <afxwin.h>
#include <mapiwin.h>
#include <mapix.h>
#include "resource.h"
#include "tbllist.h"


//Global Font object pointers
extern CFont*  pBoldFont;
extern CFont*  pNormFont;
extern CFont*  pLightFont;
extern BOOL    fUNICODE;
extern BOOL    fObjectType;
extern LPSPropTagArray ABH_PropTagArray;

/*************************************************************************
 *  CTblList Message Map.
 *************************************************************************/

BEGIN_MESSAGE_MAP(CTblList, CListBox)
    ON_WM_VSCROLL()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

/*************************************************************************
 *  CTblList Methods.
 *************************************************************************/

/*
 ** CTblList::CTblList
 *
 *  PARAMETERS:
 *      UINT        - ID of the listbox resource
 *      LPMAPITABLE - Table being viewed
 *      BOOL        - Hierarchy Table flag
 *
 *  DESCRIPTION:
 *      Constructor for the CTblList Class
 *
 *  RETURNS:
 *      void
 */

CTblList::CTblList( UINT nIDTemplate, LPMAPITABLE lpMAPITable, ULONG ulCurrentTableType )
{
    BOOL err;

    m_nIDTemplate           = nIDTemplate;
    m_lpMAPITable           = lpMAPITable;
    m_ulCurrentTableType    = ulCurrentTableType;
    m_nPageSize             = 0;
    m_ulCount               = 0;
    
    m_lpbBitmap = new CBitmap();
    err = m_lpbBitmap->LoadBitmap( IDB_ABIMAGES );
    if ( !err )
        MessageBox( "No Bitmap loaded!", "NOTE:", MB_OK );
}


/*
 ** CTblList::~CTblList
 *
 *  PARAMETERS:
 *      none
 *
 *  DESCRIPTION:
 *      Destructor for the CTblList Class
 *
 *  RETURNS:
 *      void
 */

CTblList::~CTblList()
{
    delete m_lpbBitmap;    
}


/*
 ** CTblList::InitListBox
 *
 *  PARAMETERS:
 *      none
 *
 *  DESCRIPTION:
 *      Initializes the ListBox
 *
 *  RETURNS:
 *      void
 *
 */

void CTblList::InitListBox()
{
    HRESULT hResult     = NULL;
            m_nPageSize = GetPageSize();
            
    if(!m_lpMAPITable)
        return;            

    if( FAILED(hResult = m_lpMAPITable->GetRowCount( TBL_NOWAIT, &m_ulCount )))
        return;

    if( m_ulCount > m_nPageSize )
    {
        EnableScrollBarCtrl( SB_VERT, TRUE );
        SetScrollRange( SB_VERT, (int)0, (int)(m_ulCount-m_nPageSize), FALSE );
        ShowScrollBar( SB_VERT, TRUE );
    }

    if( FAILED(hResult = m_lpMAPITable->SetColumns( (LPSPropTagArray)ABH_PropTagArray, 0 )))
        return;

    VScroll( m_nPageSize );
    SetCurSel( 0 );
}


/*
 -  CTblList::
 -  OnVScroll
 -
 *  Purpose:
 *      Handles Scrolling Commands
 *
 */

void CTblList::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    HRESULT hResult     = NULL;
    int     nOffset     = 0;
    float   fOffset     = (float)0;
    short   nScrollInc;
    int     nNewPos;
    int     nVScrollMin, nVScrollMax;
    int     nVScrollPos = 0;
    
    nVScrollPos = GetScrollPos(SB_VERT);

    //Get the scroll bar range
    GetScrollRange( SB_VERT, &nVScrollMin, &nVScrollMax );

    switch (nSBCode)
    {
        case SB_TOP: //move to the top
            nScrollInc = -nVScrollPos;

            if( FAILED(hResult = m_lpMAPITable->SeekRow( BOOKMARK_BEGINNING, 0, NULL )))
                return;

        break;

        case SB_BOTTOM: //move to the bottom
            nScrollInc = nVScrollMax - nVScrollPos;

            if( FAILED(hResult = m_lpMAPITable->SeekRow( BOOKMARK_END, -m_nPageSize, NULL )))
                return;

        break;

        case SB_LINEUP: //move one line up
            nScrollInc = -1;
        break;

        case SB_LINEDOWN: //move one line down
            nScrollInc = 1;
        break;

        case SB_PAGEUP: //move one page up
            nScrollInc = -(m_nPageSize-1);
        break;

        case SB_PAGEDOWN: //move one page down
            nScrollInc = (m_nPageSize-1);
        break;

        case SB_THUMBPOSITION: //track the thumb box
            nScrollInc = nPos - nVScrollPos;

            if( nPos == 0 )
            {
                if( FAILED(hResult = m_lpMAPITable->SeekRow( BOOKMARK_BEGINNING, 0, NULL )))
                    return;
            }
            else if( nPos == nVScrollMax )
            {
                if( FAILED(hResult = m_lpMAPITable->SeekRow( BOOKMARK_END, -m_nPageSize, NULL )))
                    return;
            }

            if ( !((nPos == 0) || (nPos == nVScrollMax)) )
            {

                if( FAILED(hResult = m_lpMAPITable->SeekRowApprox( nPos, nVScrollMax )))
                    return;

                // Calculate an offset to adjust for the Scrollbar being at the top or
                // bottom of the list.  This is important if the paging if the view
                // is to take into account the location of the current table pointer
                // and the amount that is cached in view.

                fOffset = (float)nPos / (float)nVScrollMax;
                nOffset = (int)((float)m_nPageSize * fOffset);

                if( FAILED(hResult = m_lpMAPITable->SeekRow( BOOKMARK_CURRENT, -nOffset, NULL )))
                    return;
            }

        break;

        default:
            nScrollInc = 0;
    }

    // Calculate new vertical thumb box position so that:
    // 0 <= nNewPos <= nVScrollMax
    nNewPos = max(0, min(nVScrollPos + nScrollInc, nVScrollMax));

    // adjust scroll increment
    if( (nSBCode  == SB_THUMBPOSITION) || (nSBCode  == SB_TOP) || (nSBCode  == SB_BOTTOM) )
    {
        while(DeleteString( 0 )) {}
        nScrollInc = m_nPageSize;
    }
    else
        nScrollInc = nNewPos - nVScrollPos;

    // is nScrollInc not zero?
    if (nScrollInc)
    {
        // move the thumb box
        SetScrollPos( SB_VERT, nNewPos ); 

        // scroll the list boxes
        VScroll(nScrollInc);
    }
}



/*
 -  CTblList::
 -  VScroll
 -
 *  Purpose:
 *      Handles ITable interface Commands
 *
 */

void CTblList::VScroll(short nScrollInc)
{
    short       nScrollAmt  = 0;
    LPSRowSet   lpRows      = NULL;
    HRESULT     hResult     = NULL;
    ULONG       cRows       = 0;

    if( nScrollInc < 1 )
    {
        nScrollAmt = nScrollInc-m_nPageSize;

        if( FAILED(hResult = m_lpMAPITable->SeekRow( BOOKMARK_CURRENT, nScrollAmt, NULL )))
            return;

        nScrollInc = -nScrollInc;

        if( !FAILED(hResult = m_lpMAPITable->QueryRows( nScrollInc, 0, &lpRows )))
        {
            if( lpRows->cRows )
            {
                for( cRows = 0; cRows < lpRows->cRows; cRows++)
                    DeleteString( (UINT)((m_nPageSize-1)-cRows) );

                for( cRows = 0; cRows < lpRows->cRows; cRows++)
                    InsertString( (int)cRows, (char*) lpRows->aRow[cRows].lpProps );

                MAPIFreeBuffer( lpRows );
            }
            else
                MAPIFreeBuffer( lpRows );
        }

        nScrollAmt = m_nPageSize-nScrollInc;

        if( FAILED(hResult = m_lpMAPITable->SeekRow( BOOKMARK_CURRENT, nScrollAmt, NULL )))
            return;

    }
    else
    {
        if( !FAILED(hResult = m_lpMAPITable->QueryRows( nScrollInc, 0, &lpRows )))
        {
            if( lpRows->cRows )
            {
                if( GetCount() )
                {
                    for( cRows = 0; cRows < lpRows->cRows; cRows++)
                        DeleteString( 0 );
                }

                for( cRows = 0; cRows < lpRows->cRows; cRows++)
                    AddString( (char*) lpRows->aRow[cRows].lpProps );

                MAPIFreeBuffer( lpRows );
            }
            else
                MAPIFreeBuffer( lpRows );
        }
    }
}


/*
 -  CTblList::
 -  DeleteString
 -
 *  Purpose:
 *      Handles Deleting of Row from Table
 *
 */
int CTblList::DeleteString(UINT nIndex)
{
    int cStrings = 0;
    int nCount = (this)->GetCount();
    
    if (nCount > 0)
    {
        LPSPropValue lpProps = NULL;
        lpProps = (LPSPropValue)GetItemData( nIndex );

        if( (DWORD)lpProps != LB_ERR )
        {
            cStrings = CListBox::DeleteString( nIndex );
            MAPIFreeBuffer( lpProps );
        }
    }

    return GetCount();
}


/*
 -  CTblList::
 -  GetPageSize
 -
 */

int CTblList::GetPageSize()
{
    CRect       rect;
    CDC *       pdcListBox = NULL;
    TEXTMETRIC  tmListBox;

    m_nPageSize = 0;

    GetWindowRect( &rect );

    pdcListBox = GetDC();
    pdcListBox->GetTextMetrics( &tmListBox );

    m_nPageSize = (rect.bottom - rect.top) / tmListBox.tmHeight;

    return m_nPageSize;
}


/*
 -  CTblList::
 -  DrawItem
 -
 */

void CTblList::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
    CDC *   pDC = CDC::FromHandle(lpDIS->hDC);
    CFont * lpOldFont;
    int     idx = 1;
    CSize   size;
    int     x = 0;
    int     nIndent = 0;
    
    LPSPropValue lpProps = NULL;

    if( ODA_DRAWENTIRE & lpDIS->itemAction )
    {
        pDC->PatBlt( lpDIS->rcItem.left, lpDIS->rcItem.top,
        lpDIS->rcItem.right - lpDIS->rcItem.left,
        lpDIS->rcItem.bottom - lpDIS->rcItem.top,
        PATCOPY );

        lpProps = (LPSPropValue)lpDIS->itemData;

        if (!lpProps)
            return;

        CDC * pDCBitmap = new CDC;
        ULONG   IconBM = 0;

        lpOldFont = pDC->SelectObject( pLightFont );

        if((PROP_TYPE(lpProps[3].ulPropTag) == PT_NULL) || (fObjectType == TRUE))
        {
            switch( lpProps[2].Value.ul )
            {
                case MAPI_MAILUSER:
                    IconBM = BMWIDTH * 12;
                    break;
                case MAPI_DISTLIST:
                    IconBM = BMWIDTH * 13;
                    lpOldFont = pDC->SelectObject( pNormFont );
                    break;
                case MAPI_ABCONT:
                    IconBM = BMWIDTH * 14;
                    break;
                default:    
                    IconBM = BMWIDTH * 17;
                    break;
            }
        } 
        else if( m_ulCurrentTableType == fHierarchy )
        {
            switch( lpProps[3].Value.ul )
            {
                case DT_MODIFIABLE:
                    IconBM = BMWIDTH * 7;
                    break;
                case DT_GLOBAL:
                    IconBM = BMWIDTH * 8;
                    break;
                case DT_LOCAL:
                    IconBM = BMWIDTH * 9;
                    break;
                case DT_WAN:
                    IconBM = BMWIDTH * 10;
                    break;
                case DT_NOT_SPECIFIC:
                    IconBM = BMWIDTH * 11;
                    break;
                default:    
                    IconBM = BMWIDTH * 16;
                    break;
            }
        }
        else if( m_ulCurrentTableType != fHierarchy )
        {
            switch( lpProps[3].Value.ul )
            {
                case DT_MAILUSER:
                    IconBM = 0;
                    break;
                case DT_DISTLIST:
                    IconBM = BMWIDTH * 1;
                    lpOldFont = pDC->SelectObject( pNormFont );
                    break;
                case DT_FORUM:
                    IconBM = BMWIDTH * 2;
                    lpOldFont = pDC->SelectObject( pNormFont );
                    break;
                case DT_AGENT:
                    IconBM = BMWIDTH * 3;
                    lpOldFont = pDC->SelectObject( pNormFont );
                    break;
                case DT_ORGANIZATION:
                    IconBM = BMWIDTH * 4;
                    lpOldFont = pDC->SelectObject( pNormFont );
                    break;
                case DT_PRIVATE_DISTLIST:
                    IconBM = BMWIDTH * 5;
                    lpOldFont = pDC->SelectObject( pNormFont );
                    break;
                case DT_REMOTE_MAILUSER:
                    IconBM = BMWIDTH * 6;
                    break;
                default:    
                    IconBM = BMWIDTH * 16;
                    break;
            }
        }

        pDCBitmap->CreateCompatibleDC( pDC );
        pDCBitmap->SelectObject( m_lpbBitmap );

        if( m_ulCurrentTableType == fHierarchy )
        {
            nIndent = (int)(INDENT * lpProps[4].Value.ul);

            if (nIndent == 0)
                lpOldFont = pDC->SelectObject( pBoldFont );
                

            if(PROP_TYPE(lpProps[5].ulPropTag) != PT_NULL)
            {
                if(lpProps[5].Value.ul & AB_SUBCONTAINERS) 
                pDC->BitBlt( (int)(lpDIS->rcItem.left + nIndent), 
                             (int)lpDIS->rcItem.top,
                             (int)15, 
                             (int)BMHEIGHT,
                             pDCBitmap,
                             (int)BMWIDTH * 18,
                             (int)0,
                             SRCCOPY );
            }

            nIndent += 13;
        }

        pDC->BitBlt( (int)(lpDIS->rcItem.left + nIndent), 
                     (int)lpDIS->rcItem.top,
                     (int)BMWIDTH, 
                     (int)BMHEIGHT,
                     pDCBitmap,
                     (int)IconBM,
                     (int)0,
                     SRCCOPY );

        delete pDCBitmap;

        if(PROP_TYPE(lpProps[0].ulPropTag) == PT_UNICODE)
        {

#ifdef _WINNT        
            TextOutW( lpDIS->hDC,
                      lpDIS->rcItem.left+BMWIDTH+nIndent, lpDIS->rcItem.top,
                      (LPWSTR)lpProps[0].Value.lpszW,
                      lstrlenW((LPWSTR)lpProps[0].Value.lpszW ));
#endif                      

        }
        else
        {
            pDC->TextOut( lpDIS->rcItem.left+BMWIDTH+nIndent, lpDIS->rcItem.top,
                          (LPCSTR)lpProps[0].Value.lpszA,
                          lstrlen((LPCSTR)lpProps[0].Value.lpszA ));
        }
        

        if( ODS_SELECTED & lpDIS->itemState )
            pDC->PatBlt( lpDIS->rcItem.left, lpDIS->rcItem.top,
                         lpDIS->rcItem.right - lpDIS->rcItem.left,
                         lpDIS->rcItem.bottom - lpDIS->rcItem.top,
                         DSTINVERT );

        if( ODS_FOCUS & lpDIS->itemState )
            pDC->DrawFocusRect( &lpDIS->rcItem );
    }
    else
    {
        if( ODA_SELECT & lpDIS->itemAction )
            pDC->PatBlt( lpDIS->rcItem.left, lpDIS->rcItem.top,
                         lpDIS->rcItem.right - lpDIS->rcItem.left,
                         lpDIS->rcItem.bottom - lpDIS->rcItem.top,
                         DSTINVERT );

        if( ODA_FOCUS & lpDIS->itemAction )
            pDC->DrawFocusRect( &lpDIS->rcItem );
    }
}


/*
 -  CTblList::
 -  MeasureItem
 -
 */

void CTblList::MeasureItem(LPMEASUREITEMSTRUCT)
{
}


/*
 -  CTblList::
 -  CompareItem
 -
 */

int CTblList::CompareItem(LPCOMPAREITEMSTRUCT)
{
    return 0;
}

/*
 -  CTblList::
 -  DeleteItem
 -
 */

void CTblList::DeleteItem(LPDELETEITEMSTRUCT)
{
}

/*
 -  CTblList::
 -  OnKeyDown
 -
 *  Purpose:
 *
 */

void CTblList::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
int nTopIndex = GetTopIndex();
int nCurSel   = GetCurSel();
int nGetCount = GetCount();

    switch (nChar)
    {
        case VK_HOME:
            OnVScroll( SB_TOP, 0, NULL );
            SetCurSel( 0 );
            
        break;

        case VK_END:
            OnVScroll( SB_BOTTOM, 0, NULL );
            SetCurSel( nGetCount - 1 );
            
        break;

        case VK_UP:
            if(nCurSel == 0)
            {
                OnVScroll( SB_LINEUP, 0, NULL );
                SetCurSel( 0 );
            }
            else
                CListBox::OnKeyDown( nChar, nRepCnt, nFlags );

        break;

        case VK_DOWN:
            if(nCurSel == (nTopIndex + nGetCount - 1))
            {
                OnVScroll( SB_LINEDOWN, 0, NULL );
                SetCurSel( nGetCount - 1 );
            }    
            else
                CListBox::OnKeyDown( nChar, nRepCnt, nFlags );
                
        break;

        case VK_PRIOR:
            if(nCurSel == 0)
            {
                OnVScroll( SB_PAGEUP, 0, NULL );
                SetCurSel( 0 );
            } 
            else
                CListBox::OnKeyDown( nChar, nRepCnt, nFlags );

        break;

        case VK_NEXT:
            if(nCurSel == (nTopIndex + nGetCount - 1))
            {
                OnVScroll( SB_PAGEDOWN, 0, NULL );
                SetCurSel( nGetCount - 1 );
            }
            else
                CListBox::OnKeyDown( nChar, nRepCnt, nFlags );

        break;

        default:
            CListBox::OnKeyDown( nChar, nRepCnt, nFlags );
            return;
    }
}

