/*
 * DOCUMENT.CPP
 * Patron Chapter 5
 *
 * Implementation of the CPatronDoc derivation of CDocument that
 * manages pages for us.
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
    //CHAPTER5MOD
    m_pIStorage=NULL;
    //End CHAPTER5MOD
    return;
    }


CPatronDoc::~CPatronDoc(void)
    {
    if (NULL!=m_pPG)
        delete m_pPG;

    //CHAPTER5MOD
    if (NULL!=m_pIStorage)
        m_pIStorage->Release();

    CoFreeUnusedLibraries();
    //End CHAPTER5MOD

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
    //CHAPTER5MOD
    if (NULL!=m_pPG)
        m_pPG->FIStorageSet(NULL, FALSE, FALSE);
    //End CHAPTER5MOD

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
    //CHAPTER5MOD
    LPSTORAGE   pIStorage;
    HRESULT     hr;
    CLSID       clsID;
    DWORD       dwMode=STGM_TRANSACTED | STGM_READWRITE
                    | STGM_SHARE_EXCLUSIVE;

    if (NULL==pszFile)
        {
        //Create a new temp file.
        hr=StgCreateDocfile(NULL, dwMode | STGM_CREATE
            | STGM_DELETEONRELEASE, 0, &pIStorage);

        //Mark this our class since we check with ReadClassStg.
        if (SUCCEEDED(hr))
            WriteClassStg(pIStorage, CLSID_PatronPages);
        }
    else
        {
#if defined(WIN32) && !defined(UNICODE)
        OLECHAR pwcsFile[MAX_PATH];
        mbstowcs(pwcsFile,pszFile,MAX_PATH);
        hr=StgOpenStorage(pwcsFile, NULL, dwMode, NULL, 0, &pIStorage);
#else
        hr=StgOpenStorage(pszFile, NULL, dwMode, NULL, 0, &pIStorage);
#endif
        }

    if (FAILED(hr))
        return DOCERR_COULDNOTOPEN;

    //Check if this is our type of file and exit if not.
    hr=ReadClassStg(pIStorage, &clsID);

    if (FAILED(hr) || CLSID_PatronPages!=clsID)
        {
        pIStorage->Release();
        return DOCERR_READFAILURE;
        }

    //End CHAPTER5MOD

    //Attempt to create our contained Pages window.
    m_pPG=new CPages(m_hInst);
    GetClientRect(m_hWnd, &rc);

    //CHAPTER5MOD
    if (!m_pPG->FInit(m_hWnd, &rc, WS_CHILD | WS_VISIBLE
        , ID_PAGES, NULL))
        {
        pIStorage->Release();
        return DOCERR_READFAILURE;
        }

    if (!m_pPG->FIStorageSet(pIStorage, FALSE, (NULL==pszFile)))
        {
        pIStorage->Release();
        return DOCERR_READFAILURE;
        }

    m_pIStorage=pIStorage;
    Rename(pszFile);

    //Do initial setup if new file, otherwise Pages handles things.
    if (NULL==pszFile)
        {
        //Go initialize the Pages for the default printer.
        if (!PrinterSetup(NULL, TRUE))
            return DOCERR_COULDNOTOPEN;

        //Go create an initial page.
        m_pPG->PageInsert(0);
        }
    //End CHAPTER5MOD

    FDirtySet(FALSE);
    return DOCERR_NONE;
    }






//CHAPTER5MOD
/*
 * CPatronDoc::USave
 *
 * Purpose:
 *  Writes the file to a known filename, requiring that the user
 *  has previously used FileOpen or FileSaveAs in order to have
 *  a filename.
 *
 * Parameters:
 *  uType           UINT indicating the type of file the user
 *                  requested to save in the File Save As dialog.
 *  pszFile         LPTSTR under which to save.  If NULL, use the
 *                  current name.
 *
 * Return Value:
 *  UINT            An error value from DOCERR_...
 */

UINT CPatronDoc::USave(UINT uType, LPTSTR pszFile)
    {
    HRESULT     hr;
    LPSTORAGE   pIStorage;
    OLECHAR     pwcsFile[MAX_PATH];

    //Save or Save As with the same file is just a commit.
    if (NULL==pszFile
        || (NULL!=pszFile && 0==lstrcmpi(pszFile, m_szFile)))
        {
#if defined(WIN32) && !defined(UNICODE)
        OLECHAR pwcsTemp[256]; // should be large enough
        mbstowcs(pwcsTemp, PSZ(IDS_CLIPBOARDFORMAT), 256);
        WriteFmtUserTypeStg(m_pIStorage, m_cf
            , pwcsTemp);
#else
        WriteFmtUserTypeStg(m_pIStorage, m_cf
            , PSZ(IDS_CLIPBOARDFORMAT));
#endif
        //Insure pages are up to date.
        m_pPG->FIStorageUpdate(FALSE);

        //Commit everyting
        m_pIStorage->Commit(STGC_ONLYIFCURRENT);

        FDirtySet(FALSE);
        return DOCERR_NONE;
        }

#if defined(WIN32) && !defined(UNICODE)
    mbstowcs(pwcsFile, pszFile, MAX_PATH);
#else
    pwcsFile = pszFile;
#endif

    /*
     * When we're given a name, open the storage, creating it new
     * if it does not exist or overwriting the old one.  Then CopyTo
     * from the current to the new, Commit the new, Release the old.
     */

    hr=StgCreateDocfile(pwcsFile, STGM_TRANSACTED | STGM_READWRITE
        | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0, &pIStorage);

    if (FAILED(hr))
        return DOCERR_COULDNOTOPEN;

    WriteClassStg(pIStorage, CLSID_PatronPages);
#if defined(WIN32) && !defined(UNICODE)
    OLECHAR pwcsTemp[256]; // should be more than big enough
    mbstowcs(pwcsTemp, PSZ(IDS_CLIPBOARDFORMAT), 256);
    WriteFmtUserTypeStg(pIStorage, m_cf, pwcsTemp);
#else
    WriteFmtUserTypeStg(pIStorage, m_cf, PSZ(IDS_CLIPBOARDFORMAT));
#endif

    //Insure all pages are up-to-date.
    m_pPG->FIStorageUpdate(TRUE);

    //This also copies the CLSID we stuff in here on file creation.
    hr=m_pIStorage->CopyTo(NULL, NULL, NULL, pIStorage);

    if (FAILED(hr))
        {
        SCODE       sc;

        pIStorage->Release();
        sc=GetScode(hr);

        /*
         * If we failed because of low memory, use IRootStorage
         * to switch into the new file.
         */
        if (E_OUTOFMEMORY==sc)
            {
            LPROOTSTORAGE        pIRoot;

            //Delete file we already created
            DeleteFile(pszFile);

            if (FAILED(m_pIStorage->QueryInterface
                (IID_IRootStorage, (PPVOID)&pIRoot)))
                return DOCERR_WRITEFAILURE;

            hr=pIRoot->SwitchToFile(pwcsFile);
            pIRoot->Release();

            if (FAILED(hr))
                return DOCERR_WRITEFAILURE;

            //If successful, the Commit below finishes the save.
            pIStorage=m_pIStorage;
            m_pIStorage->AddRef();    //Matches Release below
            }
        }

    pIStorage->Commit(STGC_ONLYIFCURRENT);

    /*
     * Revert changes on the original storage.  If this was a temp
     * file, it's deleted since we used STGM_DELETEONRELEASE.
     */
    m_pIStorage->Release();

    //Make this new storage current
    m_pIStorage=pIStorage;
    m_pPG->FIStorageSet(pIStorage, TRUE, FALSE);

    FDirtySet(FALSE);
    Rename(pszFile);    //Update caption bar.

    return DOCERR_NONE;
    }
//End CHAPTER5MOD





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
