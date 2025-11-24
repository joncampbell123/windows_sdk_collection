/*
 * DOCUMENT.CPP
 * Patron Chapter 2
 *
 * Implementation of the CPatronDoc derivation of CDocument that
 * manages pages for us.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "patron.h"
#include <memory.h>
#include <dlgs.h>       //Pring Dlg button IDs



/*
 * CPatronDoc::CPatronDoc
 * CPatronDoc::~CPatronDoc
 *
 * Constructor Parameters:
 *  hInst           HINSTANCE of the application.
 *  pFR             PCFrame of the frame object.
 */

CPatronDoc::CPatronDoc(HINSTANCE hInst, PCFrame pFR)
    : CDocument(hInst, pFR)
    {
    m_pPG=NULL;
    m_lVer=VERSIONCURRENT;
    return;
    }


CPatronDoc::~CPatronDoc(void)
    {
    if (NULL!=m_pPG)
        delete m_pPG;

    return;
    }





/*
 * CPatronDoc::FInit
 *
 * Purpose:
 *  Initializes an already created document window.  The client
 *  actually creates the window for us, then passes that here for
 *  further initialization.
 *
 * Parameters:
 *  pDI             PDOCUMENTINIT containing initialization
 *                  parameters.
 *
 * Return Value:
 *  BOOL            TRUE if the function succeeded, FALSE otherwise.
 */

BOOL CPatronDoc::FInit(PDOCUMENTINIT pDI)
    {
    //Change the stringtable range to our customization.
    pDI->idsMin=IDS_DOCUMENTMIN;
    pDI->idsMax=IDS_DOCUMENTMAX;

    //Do default initialization
    if (!CDocument::FInit(pDI))
        return FALSE;

    //Pages are created when we get a ULoad later.
    return TRUE;
    }






/*
 * CPatronDoc::FMessageHook
 *
 * Purpose:
 *  Processes WM_SIZE for the document so we can resize the Pages
 *  window.
 *
 * Parameters:
 *  <WndProc Parameters>
 *  pLRes           LRESULT * in which to store the return
 *                  value for the message.
 *
 * Return Value:
 *  BOOL            TRUE to prevent further processing,
 *                  FALSE otherwise.
 */

BOOL CPatronDoc::FMessageHook(HWND hWnd, UINT iMsg, WPARAM wParam
    , LPARAM lParam, LRESULT *pLRes)
    {
    UINT        dx, dy;
    RECT        rc;

    *pLRes=0;

    //Eat to prevent flickering
    if (WM_ERASEBKGND==iMsg)
        return TRUE;

    if (WM_SIZE==iMsg && NULL!=m_pPG)
        {
        dx=LOWORD(lParam);
        dy=HIWORD(lParam);

        if (SIZE_MINIMIZED!=wParam)
            {
            //Resize Pages window to fit the new document size.
            GetClientRect(hWnd, &rc);
            m_pPG->RectSet(&rc, FALSE);
            }
        }

    /*
     * We return FALSE even on WM_SIZE so we can let the default
     * procedure handle maximized MDI child windows appropriately.
     */
    return FALSE;
    }








/*
 * CPatronDoc::Clear
 *
 * Purpose:
 *  Sets all contents in the document back to defaults with no
 *  filename.
 *
 * Paramters:
 *  None
 *
 * Return Value:
 *  None
 */

void CPatronDoc::Clear(void)
    {
    //Completely reset the pages
    m_pPG->New();

    CDocument::Clear();
    m_lVer=VERSIONCURRENT;
    return;
    }





/*
 * CPatronDoc::ULoad
 *
 * Purpose:
 *  Loads a given document without any user interface overwriting
 *  the previous contents of the editor.
 *
 * Parameters:
 *  fChangeFile     BOOL indicating if we're to update the window
 *                  title and the filename from using this file.
 *  pszFile         LPTSTR to the filename to load.  Could be NULL
 *                  for an untitled document.
 *
 * Return Value:
 *  UINT            An error value from DOCERR_...
 */

UINT CPatronDoc::ULoad(BOOL fChangeFile, LPTSTR pszFile)
    {
    RECT        rc;

    //We don't support opening anything yet.
    if (NULL!=pszFile)
        return DOCERR_NONE;

    //Attempt to create our contained Pages window.
    m_pPG=new CPages(m_hInst);
    GetClientRect(m_hWnd, &rc);

    if (!m_pPG->FInit(m_hWnd, &rc, WS_CHILD | WS_VISIBLE
        , ID_PAGES, NULL))
        return DOCERR_NOFILE;

    //Go initialize the Pages for the default printer.
    if (!PrinterSetup(NULL, TRUE))
        return DOCERR_COULDNOTOPEN;

    Rename(NULL);

    //Go create an initial page.
    m_pPG->PageInsert(0);

    FDirtySet(FALSE);
    return DOCERR_NONE;
    }





/*
 * CPatronDoc::Print
 *
 * Purpose:
 *  Prints the current document.
 *
 * Parameters:
 *  hWndFrame       HWND of the frame to use for dialog parents.
 *
 * Return Value:
 *  BOOL            TRUE if printing happened, FALSE if it didn't
 *                  start or didn't complete.
 */

BOOL CPatronDoc::Print(HWND hWndFrame)
    {
    PRINTDLG        pd;
    BOOL            fSuccess;

    memset(&pd, 0, sizeof(PRINTDLG));
    pd.lStructSize=sizeof(PRINTDLG);
    pd.hwndOwner  =hWndFrame;
    pd.nCopies    =1;
    pd.nFromPage  =(USHORT)-1;
    pd.nToPage    =(USHORT)-1;
    pd.nMinPage   =1;
    pd.nMaxPage   =m_pPG->NumPagesGet();

    pd.lpfnPrintHook=PrintDlgHook;

    //Get the current document printer settings
    pd.hDevMode=m_pPG->DevModeGet();

    pd.Flags=PD_RETURNDC | PD_ALLPAGES | PD_COLLATE
        | PD_HIDEPRINTTOFILE | PD_NOSELECTION | PD_ENABLEPRINTHOOK;

    if (!PrintDlg(&pd))
        return FALSE;

    if (NULL!=pd.hDevMode)
        GlobalFree(pd.hDevMode);

    if (NULL!=pd.hDevNames)
        GlobalFree(pd.hDevNames);

    //Go do the actual printing.
    fSuccess=m_pPG->Print(pd.hDC, PSZ(IDS_DOCUMENTNAME), pd.Flags
        , pd.nFromPage, pd.nToPage, pd.nCopies);

    if (!fSuccess)
        {
        MessageBox(m_hWnd, PSZ(IDS_PRINTERROR)
            , PSZ(IDS_DOCUMENTCAPTION), MB_OK);
        }

    return fSuccess;
    }






/*
 * CPatronDoc::PrinterSetup
 *
 * Purpose:
 *  Selects a new printer and options for this document.
 *
 * Parameters:
 *  hWndFrame       HWND of the frame to use for dialog parents.
 *  fDefault        BOOL to avoid any dialog and just use the
 *                  default.
 *
 * Return Value:
 *  UINT            Undefined
 *
 */

UINT CPatronDoc::PrinterSetup(HWND hWndFrame, BOOL fDefault)
    {
    PRINTDLG        pd;

    //Attempt to get printer metrics for the default printer.
    memset(&pd, 0, sizeof(PRINTDLG));
    pd.lStructSize=sizeof(PRINTDLG);

    if (fDefault)
        pd.Flags=PD_RETURNDEFAULT;
    else
        {
        pd.hwndOwner=hWndFrame;
        pd.Flags=PD_PRINTSETUP;

        //Get the current document printer settings
        pd.hDevMode=m_pPG->DevModeGet();
        }

    if (!PrintDlg(&pd))
        return FALSE;

    if (!m_pPG->DevModeSet(pd.hDevMode, pd.hDevNames))
        {
        GlobalFree(pd.hDevNames);
        GlobalFree(pd.hDevMode);
        return FALSE;
        }

    FDirtySet(TRUE);
    return 1;
    }







/*
 * CPatronDoc::NewPage
 *
 * Purpose:
 *  Creates a new page in the document's pages control after the
 *  current page.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  UINT            Index of the new page.
 */

UINT CPatronDoc::NewPage(void)
    {
    FDirtySet(TRUE);
    return m_pPG->PageInsert(0);
    }







/*
 * CPatronDoc::DeletePage
 *
 * Purpose:
 *  Deletes the current page from the document.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  UINT            Index of the now current page.
 */

UINT CPatronDoc::DeletePage(void)
    {
    FDirtySet(TRUE);
    return m_pPG->PageDelete(0);
    }







/*
 * CPatronDoc::NextPage
 *
 * Purpose:
 *  Shows the next page in the pages window.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  UINT            Index of the new page.
 */

UINT CPatronDoc::NextPage(void)
    {
    UINT        iPage;

    iPage=m_pPG->CurPageGet();
    return m_pPG->CurPageSet(++iPage);
    }







/*
 * CPatronDoc::PreviousPage
 *
 * Purpose:
 *  Shows the previous page in the pages window.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  UINT            Index of the new page.
 */

UINT CPatronDoc::PreviousPage(void)
    {
    UINT        iPage;

    //If iPage is zero, then we wrap around to the end.
    iPage=m_pPG->CurPageGet();
    return m_pPG->CurPageSet(--iPage);
    }






/*
 * CPatronDoc::FirstPage
 *
 * Purpose:
 *  Shows the first page page in the pages window.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  UINT            Index of the new page.
 */

UINT CPatronDoc::FirstPage(void)
    {
    return m_pPG->CurPageSet(0);
    }






/*
 * CPatronDoc::LastPage
 *
 * Purpose:
 *  Shows the last page in the pages window.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  UINT            Index of the last page.
 */

UINT CPatronDoc::LastPage(void)
    {
    return m_pPG->CurPageSet(NOVALUE);
    }




/*
 * PrintDlgHook
 *
 * Purpose:
 *  Callback hook for the Print Dialog so we can hide the Setup
 *  button.  Patron only allows Setup before anything exists on
 *  the page, and is not written to handle setup at Print time.
 */

UINT CALLBACK PrintDlgHook(HWND hDlg, UINT iMsg, WPARAM wParam
    , LPARAM lParam)
    {
    if (WM_INITDIALOG==iMsg)
        {
        HWND        hWnd;

        hWnd=GetDlgItem(hDlg, psh1);
        ShowWindow(hWnd, SW_HIDE);
        return TRUE;
        }

    return FALSE;
    }
