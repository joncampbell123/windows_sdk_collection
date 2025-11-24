/*
 * PAGES.CPP
 * Patron Chapter 2
 *
 * Implementation of the CPages class.  See PAGEWIN.CPP for
 * additional member functions.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */



#include "patron.h"



/*
 * CPages:CPages
 * CPages::~CPages
 *
 * Constructor Parameters:
 *  hInst           HINSTANCE of the application we're in.
 */

CPages::CPages(HINSTANCE hInst)
    : CWindow(hInst)
    {
    m_iPageCur=NOVALUE;  //Pages are 0 indexed, this is one before
    m_cPages=0;
    m_hWndPageList=NULL;
    m_fSystemFont=FALSE;

    //Initialize to 8.5*11 inch with .25 inch margins as a default.
    m_cx=(LOMETRIC_PER_INCH*17)/2;
    m_cy=LOMETRIC_PER_INCH*11;

    m_xMarginLeft=LOMETRIC_PER_INCH/4;
    m_xMarginRight=LOMETRIC_PER_INCH/4;
    m_yMarginTop=LOMETRIC_PER_INCH/4;
    m_yMarginBottom=LOMETRIC_PER_INCH/4;

    m_xPos=0L;
    m_yPos=0L;

    m_dwIDNext=0;
    m_hDevMode=NULL;
    return;
    }


CPages::~CPages(void)
    {
    //Insure memory is cleaned up in the list
    New();

    if (NULL!=m_hFont && !m_fSystemFont)
        DeleteObject(m_hFont);

    if (NULL!=m_hWndPageList)
        DestroyWindow(m_hWndPageList);

    return;
    }





/*
 * CPages::FInit
 *
 * Purpose:
 *  Instantiates a pages window within a given parent.  The
 *  parent may be a main application window, could be an MDI child
 *  window. We really do not care.
 *
 * Parameters:
 *  hWndParent      HWND of the parent of this window
 *  pRect           LPRECT that this window should occupy
 *  dwStyle         DWORD containing the window's style flags.
 *                  Should contain WS_CHILD | WS_VISIBLE in
 *                  typical circumstances.
 *  uID             UINT ID to associate with this window
 *  pv              LPVOID unused for now.
 *
 * Return Value:
 *  BOOL            TRUE if the function succeeded, FALSE otherwise.
 */

BOOL CPages::FInit(HWND hWndParent, LPRECT pRect, DWORD dwStyle
    , UINT uID, LPVOID pv)
    {
    int     cy;

    m_hWnd=CreateWindowEx(WS_EX_NOPARENTNOTIFY, SZCLASSPAGES
        , SZCLASSPAGES, dwStyle, pRect->left, pRect->top
        , pRect->right-pRect->left, pRect->bottom-pRect->top
        , hWndParent, (HMENU)uID, m_hInst, this);

    if (NULL==m_hWnd)
        return FALSE;

    /*
     * Create the hidden listbox we'll use to track pages.  We give
     * it the owner-draw style so we can just store pointers in it.
     */
    m_hWndPageList=CreateWindow(TEXT("listbox"), TEXT("Page List")
        , WS_POPUP | LBS_OWNERDRAWFIXED, 0, 0, 100, 100
        , HWND_DESKTOP, NULL, m_hInst, NULL);

    if (NULL==m_hWndPageList)
        return FALSE;

    //Create a 14 point Arial font, or use the system variable font.
    cy=MulDiv(-14, LOMETRIC_PER_INCH, 72);
    m_hFont=CreateFont(cy, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE
        , ANSI_CHARSET, OUT_TT_PRECIS, CLIP_TT_ALWAYS, PROOF_QUALITY
        , VARIABLE_PITCH | FF_SWISS, TEXT("Arial"));

    if (NULL==m_hFont)
        {
        m_hFont=(HFONT)GetStockObject(ANSI_VAR_FONT);
        m_fSystemFont=TRUE;
        }

    return TRUE;
    }




/*
 * CPages::New
 *
 * Purpose:
 *  Cleans out and reinitializes the data to defaults.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

void CPages::New(void)
    {
    PCPage      pPage;
    UINT        i;

    //First walk the page list and clean out the allocations.
    for (i=0; i < m_cPages; i++)
        {
        if (FPageGet(i, &pPage, FALSE))
            delete pPage;
        }

    if (NULL!=m_hWndPageList)
        SendMessage(m_hWndPageList, LB_RESETCONTENT, 0, 0L);

    if (NULL!=m_hDevMode)
        GlobalFree(m_hDevMode);

    return;
    }








/*
 * CPages::RectGet
 *
 * Purpose:
 *  Returns the rectangle of the Pages window in parent coordinates.
 *
 * Parameters:
 *  pRect           LPRECT in which to return the rectangle.
 *
 * Return Value:
 *  None
 */

void CPages::RectGet(LPRECT pRect)
    {
    RECT        rc;
    POINT       pt;

    //Retrieve the size of our rectangle in parent coordinates.
    GetWindowRect(m_hWnd, &rc);
    pt.x=rc.left;
    pt.y=rc.top;
    ScreenToClient(GetParent(m_hWnd), &pt);

    SetRect(pRect, pt.x, pt.y, pt.x+(rc.right-rc.left)
        , pt.y+(rc.bottom-rc.top));

    return;
    }






/*
 * CPages::RectSet
 *
 * Purpose:
 *  Sets a new rectangle for the Pages window which sizes to fit.
 *  Coordinates are given in parent terms.
 *
 * Parameters:
 *  pRect           LPRECT containing the new rectangle.
 *  fNotify         BOOL indicating if we're to notify anyone of
 *                  the change.
 *
 * Return Value:
 *  None
 */

void CPages::RectSet(LPRECT pRect, BOOL fNotify)
    {
    UINT        cx, cy;

    if (NULL==pRect)
        return;

    cx=pRect->right-pRect->left;
    cy=pRect->bottom-pRect->top;

    SetWindowPos(m_hWnd, NULL, pRect->left, pRect->top
        , (UINT)cx, (UINT)cy, SWP_NOZORDER);

    UpdateScrollRanges();
    return;
    }




/*
 * CPages::SizeGet
 *
 * Purpose:
 *  Retrieves the size of the pages window in parent coordinates.
 *
 * Parameters:
 *  pRect           LPRECT in which to return the size.  The right
 *                  and bottom fields will contain the dimensions.
 *
 * Return Value:
 *  None
 */

void CPages::SizeGet(LPRECT pRect)
    {
    RectGet(pRect);
    return;
    }







/*
 * CPages::SizeSet
 *
 * Purpose:
 *  Sets a new size in parent coordinates for the Pages window.
 *
 * Parameters:
 *  pRect           LPRECT containing the new rectangle.
 *  fNotify         BOOL indicating if we're to notify anyone of
 *                  the change.
 *
 * Return Value:
 *  None
 */

void CPages::SizeSet(LPRECT pRect, BOOL fNotify)
    {
    UINT        cx, cy;

    if (NULL==pRect)
        return;

    cx=pRect->right-pRect->left;
    cy=pRect->bottom-pRect->top;

    SetWindowPos(m_hWnd, NULL, 0, 0, (UINT)cx, (UINT)cy
        , SWP_NOMOVE | SWP_NOZORDER);

    UpdateScrollRanges();
    return;
    }






/*
 * CPages::PageInsert
 *
 * Purpose:
 *  Creates a new page immediately after the current page.  If
 *  there are no pages then this creates page 1.
 *
 * Parameters:
 *  uReserved       UINT unused
 *
 * Return Value:
 *  UINT            Index of the new page, 0 on failure.
 */

UINT CPages::PageInsert(UINT uReserved)
    {
    //Create and open the new page.
    if (!FPageAdd(m_iPageCur, m_dwIDNext, TRUE))
        return 0;

    m_dwIDNext++;
    m_iPageCur++;
    m_cPages++;

    InvalidateRect(m_hWnd, NULL, FALSE);
    UpdateWindow(m_hWnd);
    return m_iPageCur;
    }







/*
 * CPages::PageDelete
 *
 * Removes the current page from the page list.
 *
 * Parameters:
 *  uReserved       UINT unused
 *
 * Return Value:
 *  UINT            Index to the now current page from the page
 *                  list, NOVALUE on error.
 */

UINT CPages::PageDelete(UINT uReserved)
    {
    PCPage      pPage;

    if (!FPageGet(m_iPageCur, &pPage, FALSE))
        return NOVALUE;

    //Delete the page in both the listbox and in memory.
    SendMessage(m_hWndPageList, LB_DELETESTRING, m_iPageCur, 0L);
    delete pPage;


    /*
     * If this is the last page then the current is one less.  If
     * it's the only page the current is zero.  Otherwise the
     * current is the next page.
     */

    if (m_iPageCur==m_cPages-1)   //Covers last or only page.
        m_iPageCur--;

    m_cPages--;

    if (0!=m_cPages)
        InvalidateRect(m_hWnd, NULL, FALSE);
    else
        InvalidateRect(m_hWnd, NULL, TRUE);

    UpdateWindow(m_hWnd);
    return m_iPageCur;
    }






/*
 * CPages::CurPageGet
 *
 * Purpose:
 *  Retrieves the index of the current page we're viewing.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  UINT            Index of the current page.
 */

UINT CPages::CurPageGet(void)
    {
    return m_iPageCur;
    }





/*
 * CPages::CurPageSet
 *
 * Purpose:
 *  Sets the index of the current page to view.
 *
 * Parameters:
 *  iPage           UINT index of the page to view. 0 means first
 *                  page, NOVALUE means last page.
 *
 * Return Value:
 *  UINT            Index of the previous current page, NOVALUE on
 *                  error.
 */

UINT CPages::CurPageSet(UINT iPage)
    {
    UINT    iPageNew;
    UINT    iPagePrev=m_iPageCur;

    switch (iPage)
        {
        case 0:
            iPageNew=0;
            break;

        case NOVALUE:
            iPageNew=m_cPages-1;
            break;

        default:
            if (iPage >= m_cPages)
                iPage=0;

            iPageNew=iPage;
            break;
        }

    //No reason to switch to the same page.
    if (iPagePrev==iPageNew)
        return iPage;

    m_iPageCur=iPageNew;

    InvalidateRect(m_hWnd, NULL, FALSE);
    UpdateWindow(m_hWnd);
    return iPagePrev;
    }



/*
 * CPages::NumPagesGet
 *
 * Purpose:
 *  Returns the number of pages this object current contains.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  UINT            Number of pages we contain.
 */

UINT CPages::NumPagesGet(void)
    {
    return m_cPages;
    }





/*
 * CPages::DevModeSet
 *
 * Purpose:
 *  Provides the Pages with the current printer information.
 *
 * Parameters:
 *  hDevMode        HGLOBAL to the memory containing the DEVMODE.
 *                  This function assumes responsibility for this
 *                  handle.
 *  hDevNames       HGLOBAL providing the driver name and device
 *                  name from which we can create a DC for
 *                  information.
 *
 * Return Value:
 *  BOOL            TRUE if we could accept this configuration,
 *                  FALSE otherwise.  If we return TRUE we also
 *                  delete the old memory we hold.
 */

BOOL CPages::DevModeSet(HGLOBAL hDevMode, HGLOBAL hDevNames)
    {
    LPDEVNAMES      pdn;
    LPTSTR          psz;

    if (NULL==hDevMode || NULL==hDevNames)
        return FALSE;

    psz=(LPTSTR)GlobalLock(hDevNames);

    if (NULL==psz)
        return FALSE;

    pdn=(LPDEVNAMES)psz;
    lstrcpy(m_szDriver, psz+pdn->wDriverOffset);
    lstrcpy(m_szDevice, psz+pdn->wDeviceOffset);
    lstrcpy(m_szPort,   psz+pdn->wOutputOffset);
    GlobalUnlock(hDevNames);
    GlobalFree(hDevNames);

    //Save this new memory and get rid of the old.
    if (NULL!=m_hDevMode)
        GlobalFree(m_hDevMode);

    m_hDevMode=hDevMode;

    return ConfigureForDevice();
    }







/*
 * CPages::DevModeGet
 *
 * Purpose:
 *  Retrieves a copy of the current DEVMODE structure for this
 *  Pages window.  The caller is responsible for this memory.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HGLOBAL         Handle to the memory containing the DEVMODE
 *                  structure.
 */

HGLOBAL CPages::DevModeGet(void)
    {
    HGLOBAL     hMem;
    DWORD       cb;
    DWORD       i;
    LPBYTE      pb1, pb2;

    cb=GlobalSize(m_hDevMode);

    if (0==cb)
        return NULL;

    hMem=GlobalAlloc(GHND, cb);

    if (NULL==hMem)
        return NULL;

    pb1=(LPBYTE)GlobalLock(hMem);
    pb2=(LPBYTE)GlobalLock(m_hDevMode);

    //Copy the existing DEVMODE structure
    for (i=0; i < cb; i++)
        *pb1++=*pb2++;

    GlobalUnlock(m_hDevMode);
    GlobalUnlock(hMem);

    return hMem;
    }




/*
 * CPages::ConfigureForDevice
 *
 * Purpose:
 *  Recalculates our drawing configuration based on the contents of
 *  an hDC.  If no HDC is given we use the contents of our DevMode
 *  stream.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if successful, FALSE otherwise.
 */

BOOL CPages::ConfigureForDevice(void)
    {
    POINT           ptOffset, ptPaper;
    RECT            rc;
    HDC             hDC;
    LPDEVMODE       pdm;

    pdm=(LPDEVMODE)GlobalLock(m_hDevMode);

    if (NULL==pdm)
        return FALSE;

    //Get the DC then configure
    hDC=CreateIC(m_szDriver, m_szDevice, m_szPort, pdm);

    GlobalUnlock(m_hDevMode);

    if (NULL==hDC)
        return FALSE;

    //Get usable page dimensions:  already sensitive to orientation
    m_cx=GetDeviceCaps(hDC, HORZSIZE)*10-16; //*10: mm to LOMETRIC
    m_cy=GetDeviceCaps(hDC, VERTSIZE)*10-16; //-16: for driver bugs.

    //Calculate the printer-limited margins on sides in LOMETRIC.
    Escape(hDC, GETPRINTINGOFFSET, NULL, NULL, &ptOffset);
    Escape(hDC, GETPHYSPAGESIZE,   NULL, NULL, &ptPaper);

    SetRect(&rc, ptOffset.x, ptOffset.y, ptPaper.x, ptPaper.y);
    SetMapMode(hDC, MM_LOMETRIC);
    RectConvertMappings(&rc, hDC, FALSE);

    //Left and top margins are the printing offset.
    m_xMarginLeft= rc.left+8;   //+8 to match -16 above
    m_yMarginTop =-rc.top+8;    //LOMETRIC makes this negative.

    //Right is (paper width)-(usable width)-(left margin)
    m_xMarginRight =rc.right-m_cx-m_xMarginLeft;

    //Bottom is (paper height)-(usable height)-(top margin)+1
    m_yMarginBottom=-rc.bottom-m_cy-m_yMarginTop+1;

    UpdateScrollRanges();

    DeleteDC(hDC);
    return TRUE;
    }






/*
 * CPages::FPageGet
 * (Protected)
 *
 * Purpose:
 *  Returns a page of a given index returning a BOOL so it's simple
 *  to use this function inside if statements.
 *
 * Parameters:
 *  iPage           UINT page to retrieve.
 *  ppPage          PCPage * in which to return the page
 *                  pointer
 *  fReserved       BOOL reserved for future use.
 *
 * Return Value:
 *  BOOL            TRUE if successful, FALSE otherwise.
 */

BOOL CPages::FPageGet(UINT iPage, PCPage *ppPage
    , BOOL fReserved)
    {
    if (NULL==ppPage)
        return FALSE;

    if (LB_ERR!=SendMessage(m_hWndPageList, LB_GETTEXT
        , iPage, (LONG)ppPage))
        return TRUE;

    return FALSE;
    }





/*
 * CPages::FPageAdd
 * (Protected)
 *
 * Purpose:
 *  Creates a new page initialized to the given values.
 *
 * Parameters:
 *  iPage           UINT Location at which to insert page; new page
 *                  is inserted after this position.  NOVALUE for
 *                  the end.
 *  cObjects        UINT number of objects on this page.
 *  dwID            DWORD ID for this page.
 *  fReserved       BOOL reserved for future use.
 *
 * Return Value:
 *  BOOL            TRUE if the function succeeded, FALSE otherwise.
 */

BOOL CPages::FPageAdd(UINT iPage, DWORD dwID, BOOL fReserved)
    {
    PCPage      pPage;
    LRESULT     lr;

    pPage=new CPage(dwID);

    if (NULL==pPage)
        return FALSE;

    if (NOVALUE==iPage)
        iPage--;

    //Now try to add to the listbox.
    lr=SendMessage(m_hWndPageList, LB_INSERTSTRING, iPage+1
        , (LONG)pPage);

    if (LB_ERRSPACE==lr)
        {
        delete pPage;
        return FALSE;
        }

    return TRUE;
    }
