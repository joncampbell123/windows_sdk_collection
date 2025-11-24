/*
 * FIGURE.CPP
 * Cosmo Chapter 16
 *
 * Implementation of the CFigure object for Cosmo.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "cosmo.h"


/*
 * CFigure::CFigure
 * CFigure::~CFigure
 *
 * Parameters (Constructor):
 *  pfnDestroy      PFNDESTROYED to call when object is destroyed.
 *  pDoc            PCCosmoDoc we're associated with.
 */

CFigure::CFigure(PFNDESTROYED pfnDestroy, PCCosmoDoc pDoc)
    {
    m_cRef=0;
    m_pfnDestroy=pfnDestroy;

    m_pFR=NULL;     //We get this later through FrameSet.
    m_pDoc=pDoc;
    m_pPL=pDoc->m_pPL;

    m_fEmbedded=FALSE;

    //NULL any contained interfaces initially.
    m_pIPersistStorage=NULL;
    m_pIStorage=NULL;
    m_pIStream=NULL;
    m_pIDataObject=NULL;
    m_pIDataAdviseHolder=NULL;
    m_pIOleObject=NULL;
    m_pIOleAdviseHolder=NULL;
    m_pIOleClientSite=NULL;

    m_cf=pDoc->m_cf;

    //These are for IDataObject::QueryGetData
    m_cfeGet=CFORMATETCGET;

    SETDefFormatEtc(m_rgfeGet[0], pDoc->m_cf, TYMED_HGLOBAL);
    SETDefFormatEtc(m_rgfeGet[1], pDoc->m_cfEmbedSource
        , TYMED_ISTORAGE);
    SETDefFormatEtc(m_rgfeGet[2], pDoc->m_cfObjectDescriptor
        , TYMED_HGLOBAL);
    SETDefFormatEtc(m_rgfeGet[3], CF_METAFILEPICT, TYMED_MFPICT);
    SETDefFormatEtc(m_rgfeGet[4], CF_BITMAP, TYMED_GDI);

    m_pST=NULL;
    m_pIPersistFile=NULL;

    //We live in the document's lifetime, so no need to AddRef here.
    m_pMoniker=m_pDoc->m_pMoniker;
    m_dwRegROT=0L;

    //CHAPTER16MOD
    m_pIOleIPSite        =NULL;
    m_pIOleIPFrame       =NULL;
    m_pIOleIPUIWindow    =NULL;
    m_pIOleIPObject      =NULL;
    m_pIOleIPActiveObject=NULL;
    m_hMenuShared        =NULL;
    m_hOLEMenu           =NULL;
    m_hAccel             =NULL;
    m_pHW                =NULL;
    m_pGB                =NULL;
    m_cyBar              =0;
    m_fUndoDeactivates   =FALSE;
    m_fAllowInPlace      =TRUE;
    m_fForceSave         =FALSE;
    m_pST                =NULL;
    //End CHAPTER16MOD

    return;
    }


CFigure::~CFigure(void)
    {
    if (NULL!=m_pIOleClientSite)
        {
        m_pIOleClientSite->Release();
        m_pIOleClientSite=NULL;
        }

    if (NULL!=m_pIDataAdviseHolder)
        {
        m_pIDataAdviseHolder->Release();
        m_pIDataAdviseHolder=NULL;
        }

    if (NULL!=m_pIOleAdviseHolder)
        {
        m_pIOleAdviseHolder->Release();
        m_pIOleAdviseHolder=NULL;
        }

    //CHAPTER16MOD
    if (NULL!=m_pHW)
        delete m_pHW;

    if (NULL!=m_pST)
        delete m_pST;

    /*
     * Free contained interfaces.
     * Container in-place interfaces released during deactivation.
     */

    if (NULL!=m_pGB)    //Safety-net
        delete m_pGB;

    if (NULL!=m_pIOleIPObject)
        delete m_pIOleIPObject;

    if (NULL!=m_pIOleIPActiveObject)
        delete m_pIOleIPActiveObject;
    //End CHAPTER16MOD

    //Make sure no one thinks we're still running
    if (0L!=m_dwRegROT)
        OleStdRevokeAsRunning(&m_dwRegROT);

    if (NULL!=m_pIPersistFile)
        delete m_pIPersistFile;

    if (NULL!=m_pIOleObject)
        delete m_pIOleObject;

    if (NULL!=m_pIDataObject)
        delete m_pIDataObject;

    if (NULL!=m_pIStorage)
        m_pIStorage->Release();

    if (NULL!=m_pIStream)
        m_pIStream->Release();

    if (NULL!=m_pIPersistStorage)
        delete m_pIPersistStorage;

    //Free strings.
    if (NULL!=m_pST)
        delete m_pST;

    return;
    }





/*
 * CFigure::QueryInterface
 * CFigure::AddRef
 * CFigure::Release
 *
 * Purpose:
 *  IUnknown members for CFigure object.
 */

STDMETHODIMP CFigure::QueryInterface(REFIID riid, PPVOID ppv)
    {
    *ppv=NULL;

    if (IID_IUnknown==riid)
        *ppv=this;

    if (IID_IPersist==riid || IID_IPersistStorage==riid)
        *ppv=m_pIPersistStorage;

    if (IID_IDataObject==riid)
        *ppv=m_pIDataObject;

    if (IID_IOleObject==riid)
        *ppv=m_pIOleObject;

    if (IID_IPersistFile==riid)
        *ppv=m_pIPersistFile;

    //CHAPTER16MOD
    //IOleWindow will be the InPlaceObject
    if (IID_IOleWindow==riid || IID_IOleInPlaceObject==riid)
        *ppv=m_pIOleIPObject;
    //End CHAPTER16MOD

    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) CFigure::AddRef(void)
    {
    return ++m_cRef;
    }


STDMETHODIMP_(ULONG) CFigure::Release(void)
    {
    ULONG       cRefT;

    cRefT=--m_cRef;

    if (0L==m_cRef)
        {
        /*
         * Tell the housing that an object is going away so it can
         * shut down if appropriate.
         */
        if (NULL!=m_pfnDestroy)
            (*m_pfnDestroy)();
        }

    return cRefT;
    }






/*
 * CFigure::FInit
 *
 * Purpose:
 *  Performs any initialization of a CFigure that's prone to failure
 *  that we also use internally before exposing the object outside.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if the function is successful,
 *                  FALSE otherwise.
 */

BOOL CFigure::FInit(void)
    {
    m_pST=new CStringTable(m_pDoc->m_hInst);

    if (NULL==m_pST)
        return FALSE;

    if (!m_pST->FInit(IDS_FIGUREMIN, IDS_FIGUREMAX))
        return FALSE;

    //Allocate contained interfaces.
    m_pIPersistStorage=new CImpIPersistStorage(this, this);

    if (NULL==m_pIPersistStorage)
        return FALSE;

    m_pIDataObject=new CImpIDataObject(this, this);

    if (NULL==m_pIDataObject)
        return FALSE;

    m_pIOleObject=new CImpIOleObject(this, this);

    if (NULL==m_pIOleObject)
        return FALSE;

    m_pIPersistFile=new CImpIPersistFile(this, this);

    if (NULL==m_pIPersistFile)
        return FALSE;

    //CHAPTER16MOD
    m_pIOleIPObject=new CImpIOleInPlaceObject(this, this);

    if (NULL==m_pIOleIPObject)
        return FALSE;

    m_pIOleIPActiveObject=new CImpIOleInPlaceActiveObject(this
        , this);

    if (NULL==m_pIOleIPActiveObject)
        return FALSE;

    m_pHW=new CHatchWin(m_pDoc->m_hInst);

    if (NULL==m_pHW)
        return FALSE;

    //We don't have m_pFR yet from which to get the frame window.
    if (!m_pHW->FInit(m_pDoc->Window(), ID_HATCHWINDOW, NULL))
        return FALSE;

    /*
     * Load the stringtable with the only instance handle readily
     * available.
     */
    m_pST=new CStringTable(m_pDoc->m_hInst);

    if (NULL==m_pST)
        return FALSE;

    if (!m_pST->FInit(IDS_FIGUREMIN, IDS_FIGUREMAX))
        return FALSE;
    //End CHAPTER16MOD

    return TRUE;
    }



/*
 * CFigure::FrameSet
 *
 * Purpose:
 *  Provides the compound document object with access to the frame
 *  of this application for UI purposes.
 *
 * Parameters:
 *  pFR             PCCosmoFrame of the frame window.
 *
 * Return Value:
 *  None
 */

void CFigure::FrameSet(PCCosmoFrame pFR)
    {
    m_pFR=pFR;

    //CHAPTER16MOD
    //We need this in IOleInPlaceActiveObject::ResizeBorder
    m_cyBar=m_pFR->m_cyBar;

    //We need this in IOleInPlaceActiveObject::TranslateAccelerator
    m_hAccel=m_pFR->m_hAccel;
    //End CHAPTER16MOD
    return;
    }




/*
 * CFigure::FIsDirty
 *
 * Purpose:
 *  Checks if the document is dirty.  This can be called from
 *  IPersistStorage::IsDirty which doesn't have access to CCosmoDoc.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if dirty, FALSE if clean.
 */

BOOL CFigure::FIsDirty(void)
    {
    //CHAPTER16MOD
    //Force a save if we opened after being in-place.
    return m_pDoc->m_fDirty || m_fForceSave;
    //End CHAPTER16MOD
    }




/*
 * CFigure::FIsEmbedded
 *
 * Purpose:
 *  Answers if the object is embedded or not.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if the object is embedded, FALSE otherwise.
 */

BOOL CFigure::FIsEmbedded(void)
    {
    return m_fEmbedded;
    }




/*
 * CFigure::SendAdvise
 *
 * Purpose:
 *  Calls the appropriate IOleClientSite or IAdviseSink member
 *  function for various events such as closure, saving, etc.
 *
 * Parameters:
 *  uCode           UINT OBJECTCODE_* identifying the notification.
 *
 * Return Value:
 *  None
 */

void CFigure::SendAdvise(UINT uCode)
    {
    switch (uCode)
        {
        case OBJECTCODE_SAVED:
            if (NULL!=m_pIOleAdviseHolder)
                m_pIOleAdviseHolder->SendOnSave();

                        break;

        case OBJECTCODE_CLOSED:
            if (NULL!=m_pIOleAdviseHolder)
                m_pIOleAdviseHolder->SendOnClose();

                        break;

        case OBJECTCODE_RENAMED:
            //Update the moniker copy we have.
            m_pMoniker=m_pDoc->m_pMoniker;

            if (NULL!=m_pIOleAdviseHolder)
                m_pIOleAdviseHolder->SendOnRename(m_pMoniker);

            break;

        case OBJECTCODE_SAVEOBJECT:
            if (FIsDirty() && NULL!=m_pIOleClientSite)
                m_pIOleClientSite->SaveObject();

            break;

        case OBJECTCODE_DATACHANGED:
            //No flags are necessary here.
            if (NULL!=m_pIDataAdviseHolder)
                {
                m_pIDataAdviseHolder->SendOnDataChange
                    (m_pIDataObject, 0, 0);
                }

            //CHAPTER16MOD
            /*
             * If this is the first change after activation, tell the
             * container to free any undo information it's holding.
             */
            if (NULL!=m_pIOleIPSite && m_fUndoDeactivates)
                m_pIOleIPSite->DiscardUndoState();

            m_fUndoDeactivates=FALSE;
            //End CHAPTER16MOD

            break;

        case OBJECTCODE_SHOWWINDOW:
            if (NULL!=m_pIOleClientSite)
                m_pIOleClientSite->OnShowWindow(TRUE);

            break;

        case OBJECTCODE_HIDEWINDOW:
            if (NULL!=m_pIOleClientSite)
                m_pIOleClientSite->OnShowWindow(FALSE);

            break;

        case OBJECTCODE_SHOWOBJECT:
            if (NULL!=m_pIOleClientSite)
                m_pIOleClientSite->ShowObject();

            break;
        }

    return;
    }




//CHAPTER16MOD
/*
 * CFigure::InPlaceActivate
 *
 * Purpose:
 *  Goes through all the steps of activating the Figure as an
 *  in-place object.
 *
 * Parameters:
 *  pActiveSite     LPOLECLIENTSITE of the active site we show in.
 *  fIncludeUI      BOOL indicating if we should do UI as well.
 *
 * Return Value:
 *  HRESULT         Whatever error code is appropriate.
 */

HRESULT CFigure::InPlaceActivate(LPOLECLIENTSITE pActiveSite
    , BOOL fIncludeUI)
    {
    HRESULT                 hr;
    HWND                    hWnd, hWndHW;
    RECT                    rcPos;
    RECT                    rcClip;

    if (NULL==pActiveSite)
        return ResultFromScode(E_INVALIDARG);

    //If we're already active, do the UI and we're done.
    if (NULL!=m_pIOleIPSite)
        {
        if (fIncludeUI)
            UIActivate();

        return NOERROR;
        }

    /*
     * 1.  Initialization, obtaining interfaces, calling
     *     OnInPlaceActivate.
     */
    hr=pActiveSite->QueryInterface(IID_IOleInPlaceSite
        , (PPVOID)&m_pIOleIPSite);

    if (FAILED(hr))
        return hr;

    hr=m_pIOleIPSite->CanInPlaceActivate();

    if (NOERROR!=hr)
        {
        m_pIOleIPSite->Release();
        m_pIOleIPSite=NULL;
        return ResultFromScode(E_FAIL);
        }

    m_pIOleIPSite->OnInPlaceActivate();
    m_fUndoDeactivates=TRUE;

    /*
     * 2.  Get the window context and create a window or change the
     *     parent and position of an existing window.  Servers are
     *     required to full cb in the OLEINPLACEFRAMEINFO structure.
     */
    m_pIOleIPSite->GetWindow(&hWnd);
    m_pFR->m_frameInfo.cb=sizeof(OLEINPLACEFRAMEINFO);

    m_pIOleIPSite->GetWindowContext(&m_pIOleIPFrame
        , &m_pIOleIPUIWindow, &rcPos, &rcClip, &m_pFR->m_frameInfo);


    /*
     * Copy container frame pointer to CCosmoFrame for accelerators.
     * No AddRef because frame never messes with it.  Note also that
     * we don't do anything special with our own accelerators here
     * because we just use the same ones as always.
     */
    m_pFR->m_pIOleIPFrame=m_pIOleIPFrame;

    /*
     * We'll use a hatch window as the child of the container and the
     * editing window as a child of the hatch window.  We already
     * created the hatch window, so now all we have to do is put it
     * in the right place and stick the Polyline in it.
     */

    m_pHW->HwndAssociateSet(m_pFR->Window());

    m_pHW->ChildSet(m_pPL->Window());   //Calls SetParent
    m_pHW->RectsSet(&rcPos, &rcClip);   //Positions polyline

    hWndHW=m_pHW->Window();
    SetParent(hWndHW, hWnd);            //Move the hatch window
    ShowWindow(hWndHW, SW_SHOW);        //Make us visible.
    SendAdvise(OBJECTCODE_SHOWOBJECT);

    //Critical for accelerators to work initially.
    SetFocus(hWndHW);

    //3.  Set the active object
    if (NULL!=m_pIOleIPFrame)
        {
#if defined(WIN32) && !defined(UNICODE)
        OLECHAR pwcsTemp[256];
        mbstowcs(pwcsTemp, PSZ(IDS_INPLACETITLE), 256);
        m_pIOleIPFrame->SetActiveObject(m_pIOleIPActiveObject, pwcsTemp);
#else
        m_pIOleIPFrame->SetActiveObject(m_pIOleIPActiveObject
            , PSZ(IDS_INPLACETITLE));
#endif
        }

    if (NULL!=m_pIOleIPUIWindow)
        {
#if defined(WIN32) && !defined(UNICODE)
        OLECHAR pwcsTemp[256];
        mbstowcs(pwcsTemp, PSZ(IDS_INPLACETITLE), 256);
        m_pIOleIPUIWindow->SetActiveObject(m_pIOleIPActiveObject, pwcsTemp);
#else
        m_pIOleIPUIWindow->SetActiveObject(m_pIOleIPActiveObject
            , PSZ(IDS_INPLACETITLE));
#endif
        }

    /*
     * These steps are handled in UIActivate:
     *  4.  Create and install the shared menu
     *  5.  Create and install any in-place tools.
     */
    if (fIncludeUI)
        return UIActivate();

    return NOERROR;
    }




/*
 * CFigure::InPlaceDeactivate
 *
 * Purpose:
 *  Reverses all the activation steps from InPlaceActivate.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

void CFigure::InPlaceDeactivate(void)
    {
    RECT        rc;

    UIDeactivate();

    /*
     * When setting the parent back to the normal document,
     * reposition the Polyline to be at (8,8) instead of at wherever
     * it was in the container's window or our hatch window.  This
     * is so if we are deactivating to open in our own window the
     * Polyline appears in the proper place in the document window.
     */

    SetParent(m_pPL->Window(), m_pDoc->m_hWnd);
    m_pHW->ChildSet(NULL);

    //Make sure the hatch window is invisible and owned by Cosmo
    ShowWindow(m_pHW->Window(), SW_HIDE);
    SetParent(m_pHW->Window(), m_pDoc->m_hWnd);

    GetClientRect(m_pDoc->m_hWnd, &rc);
    InflateRect(&rc, -8, -8);

    SetWindowPos(m_pPL->Window(), NULL, rc.left, rc.top
        , rc.right-rc.left, rc.bottom-rc.top
        , SWP_NOZORDER | SWP_NOACTIVATE);

    if (NULL!=m_pIOleIPFrame)
        {
        m_pIOleIPFrame->SetActiveObject(NULL, NULL);
        m_pIOleIPFrame->Release();
        m_pIOleIPFrame=NULL;
        m_pFR->m_pIOleIPFrame=NULL;
        }

    if (NULL!=m_pIOleIPUIWindow)
        {
        m_pIOleIPUIWindow->SetActiveObject(NULL, NULL);
        m_pIOleIPUIWindow->Release();
        m_pIOleIPUIWindow=NULL;
        }

    if (NULL!=m_pIOleIPSite)
        {
        m_pIOleIPSite->OnInPlaceDeactivate();
        m_pIOleIPSite->Release();
        m_pIOleIPSite=NULL;
        }

    return;
    }



/*
 * CFigure::UIActivate
 *
 * Purpose:
 *  Goes through all the steps of activating the user interface of
 *  the Figure as an in-place object.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         NOERROR or error code.
 */

HRESULT CFigure::UIActivate(void)
    {
    //1.  Create frame tools
    FInPlaceToolsCreate();

    //2.  Create the shared menu.
    FInPlaceMenuCreate();

    //3.  Call IOleInPlaceSite::UIActivate
    if (NULL!=m_pIOleIPSite)
        m_pIOleIPSite->OnUIActivate();

    return NOERROR;
    }





/*
 * CFigure::UIDeactivate
 *
 * Purpose:
 *  Reverses all the user interface activation steps from
 *  UIActivate.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

void CFigure::UIDeactivate(void)
    {
    //1.  Call IOleInPlaceSite::OnUIDeactivate
    if (NULL!=m_pIOleIPSite)
        m_pIOleIPSite->OnUIDeactivate(FALSE);

    //2.  Remove the in-place tools
    FInPlaceToolsDestroy();

    //3.  Remove the shared menu.
    FInPlaceMenuDestroy();

    return;
    }



/*
 * CFigure::FInPlaceMenuCreate
 *
 * Purpose:
 *  Creates and sets a menu for an in-place embedded object.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if everything is well, FALSE on error.
 */

BOOL CFigure::FInPlaceMenuCreate(void)
    {
    HMENU               hMenu, hMenuT;
    UINT                uTemp=MF_BYPOSITION | MF_POPUP;
    UINT                i;
    OLEMENUGROUPWIDTHS  mgw;

    for (i=0; i<6; i++)
        mgw.width[i]=0;

    //We already have popup menu handles in m_pFR->m_phMenu[]

    //Create the new shared menu and let container do its thing
    hMenu=CreateMenu();
    m_pIOleIPFrame->InsertMenus(hMenu, &mgw);

    /*
     * Add our menus remembering that the container has added its
     * already.
     */
    InsertMenu(hMenu, (UINT)mgw.width[0]
       , uTemp, (UINT)m_pFR->m_phMenu[1], PSZ(IDS_MENUEDIT));

    /*
     * Add the Open item to the edit menu.
     * NOTE:  If you are a multiple-use server, this sort of code
     * will also modify the menu on the server window as well as
     * this shared menu in which case you need a separate popup menu
     * altogether.
     */
    AppendMenu(m_pFR->m_phMenu[1], MF_SEPARATOR, 0, NULL);
    AppendMenu(m_pFR->m_phMenu[1], MF_STRING, IDM_EDITOPEN
        , PSZ(IDS_MENUOPEN));

    InsertMenu(hMenu, (UINT)mgw.width[0]+1+(UINT)mgw.width[2]
       , uTemp, (UINT)m_pFR->m_phMenu[2], PSZ(IDS_MENUCOLOR));

    InsertMenu(hMenu, (UINT)mgw.width[0]+1+(UINT)mgw.width[2]+1
       , uTemp, (UINT)m_pFR->m_phMenu[3], PSZ(IDS_MENULINE));

   #ifdef MDI
    hMenuT=m_pFR->m_phMenu[5];
   #else
    hMenuT=m_pFR->m_phMenu[4];
   #endif

    InsertMenu(hMenu, (UINT)mgw.width[0]+1+(UINT)mgw.width[2]+2
        + (UINT)mgw.width[4], uTemp, (UINT)hMenuT
        , PSZ(IDS_MENUHELP));

    //Tell OLE how many items in each group are ours.
    mgw.width[1]=1;
    mgw.width[3]=2;
    mgw.width[5]=1;

    m_hMenuShared=hMenu;
    m_hOLEMenu=OleCreateMenuDescriptor(m_hMenuShared, &mgw);

    m_pIOleIPFrame->SetMenu(m_hMenuShared, m_hOLEMenu
        , m_pFR->Window());
    return TRUE;
    }




/*
 * CFigure::FInPlaceMenuDestroy
 *
 * Purpose:
 *  Performs opposite actions from FInPlaceMenuCreate
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if all is well, FALSE otherwise.
 */

BOOL CFigure::FInPlaceMenuDestroy(void)
    {
    int         cItems, i, j;
    HMENU       hMenuT;

    //If we don't have a shared menu, nothing to do.
    if (NULL==m_hMenuShared)
        return TRUE;

    //Stop the container frame from using this menu.
    m_pIOleIPFrame->SetMenu(NULL, NULL, NULL);

    //Clean up what we got from OleCreateMenuDescriptor.
    OleDestroyMenuDescriptor(m_hOLEMenu);
    m_hOLEMenu=NULL;

    cItems=GetMenuItemCount(m_hMenuShared);

    /*
     * Walk backwards down the menu.  For each popup, see if it
     * matches any other popup we know about, and if so, remove
     * it from the shared menu.
     */
    for (i=cItems; i >=0; i--)
        {
        hMenuT=GetSubMenu(m_hMenuShared, i);

        for (j=0; j <= CMENUS; j++)
            {
            /*
             * If the submenu matches any we have, remove, don't
             * delete. Since we're walking backwards this only
             * affects the positions of those menus after us so the
             * GetSubMenu call above is not affected.
             */
            if (hMenuT==m_pFR->m_phMenu[j])
                RemoveMenu(m_hMenuShared, i, MF_BYPOSITION);
            }
        }

    /*
     * Remove the Open item and separator from the Edit menu.
     * NOTE:  If you are a multiple-user server, this affects the
     * menu on the server window as well as the shared menu in which
     * case you need to use a separate popup menu altogether.
     */
    RemoveMenu(m_pFR->m_phMenu[1], 6, MF_BYPOSITION);
    RemoveMenu(m_pFR->m_phMenu[1], 5, MF_BYPOSITION);

    if (NULL!=m_pIOleIPFrame)
        m_pIOleIPFrame->RemoveMenus(m_hMenuShared);

    DestroyMenu(m_hMenuShared);
    m_hMenuShared=NULL;
    return TRUE;
    }






/*
 * CFigure::FInPlaceToolsCreate
 *
 * Purpose:
 *  Creates a gizmobar for in-place activation and negotiates the
 *  border space for the gizmobar.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if we could allocate the space and create
 *                  the gizmos.  FALSE if we fail, meaning that we
 *                  just do in-place with menus only, since we can
 *                  do without the tools.  We could fail and attempt
 *                  to reverse what we might have done already, but
 *                  since we don't really *need* the gizmobar, we
 *                  keep going.
 */

BOOL CFigure::FInPlaceToolsCreate(void)
    {
    BORDERWIDTHS    bw;
    HWND            hWnd;
    UINT            uState=GIZMO_NORMAL;
    UINT            utCmd =GIZMOTYPE_BUTTONCOMMAND;
    UINT            utEx  =GIZMOTYPE_BUTTONATTRIBUTEEX;
    UINT            i;
    HBITMAP         hBmp;
    RECT            rc;

    //0.  We don't need anything on the document, so send zeros.
    SetRectEmpty((LPRECT)&bw);

    if (NULL!=m_pIOleIPUIWindow)
        m_pIOleIPUIWindow->SetBorderSpace(&bw);

    if (NULL==m_pIOleIPFrame)
        return FALSE;

    /*
     * 1.  Make sure we can reserve space for what we need.  If this
     *     fails then we just do without because the menu has
     *     everything we really need.  A more willing server could
     *     put tools in popup windows as well.
     */

    if (!FInPlaceToolsRenegotiate())
        {
        //If the container doesn't allow us any, don't ask for any.
        m_pIOleIPFrame->SetBorderSpace(&bw);
        return FALSE;
        }

    /*
     * 2.  Create the gizmobar window using the container window as
     *     the parent.  In order to get messages from it, we use
     *     it's AssociateSet ability directly after creation and
     *     before we add any gizmos to the bar.
     */

    m_pIOleIPFrame->GetWindow(&hWnd);

    //If we already have a gizmobar, just show it again.
    if (NULL!=m_pGB)
        {
        ShowWindow(m_pGB->Window(), SW_SHOW);
        return TRUE;
        }

    m_pGB=new CGizmoBar(m_pFR->m_hInst);

    if (NULL==m_pGB)
        {
        SetRectEmpty((LPRECT)&bw);
        m_pIOleIPFrame->SetBorderSpace(&bw);
        return FALSE;
        }

    m_pGB->FInit(hWnd, ID_GIZMOBAR, m_cyBar);
    g_pInPlaceGB=m_pGB;

    //Insure the tools are initially invisible
    ShowWindow(m_pGB->Window(), SW_HIDE);

    //Tell the gizmobar who to send messages to.
    m_pGB->HwndAssociateSet(m_pFR->m_hWnd);

    /*
     * Add gizmos to the bar, setting CFrame::fInit to avoid
     * command processing
     */
    m_pFR->m_fInit=TRUE;

    hBmp=m_pFR->m_hBmp;

    //Edit Undo, Cut, Copy, Paste
    m_pGB->Add(utCmd, 1, IDM_EDITUNDO,  24, 22, NULL, hBmp, 1, uState);
    m_pGB->Add(utCmd, 2, IDM_EDITCUT,   24, 22, NULL, NULL, 0, uState);
    m_pGB->Add(utCmd, 3, IDM_EDITCOPY,  24, 22, NULL, NULL, 1, uState);
    m_pGB->Add(utCmd, 4, IDM_EDITPASTE, 24, 22, NULL, NULL, 2, uState);

    //Separator
    m_pGB->Add(GIZMOTYPE_SEPARATOR, 5, 0, 6, 22, NULL, NULL, 0, uState);

    //Color Background and Color Line
    m_pGB->Add(utCmd, 6, IDM_COLORBACKGROUND, 24, 22, NULL, hBmp, 3
               , GIZMO_NORMAL | PRESERVE_BLACK);

    m_pGB->Add(utCmd, 7, IDM_COLORLINE, 24, 22, NULL, hBmp, 4, uState);

    //Separator
    m_pGB->Add(GIZMOTYPE_SEPARATOR, 8, 0, 6, 22, NULL, NULL, 0, uState);

    //Line styles.
    m_pGB->Add(utEx, 19, IDM_LINESOLID, 24, 22, NULL, hBmp, 5, uState);
    m_pGB->Add(utEx, 10, IDM_LINEDASH, 24, 22, NULL, hBmp, 6, uState);
    m_pGB->Add(utEx, 11, IDM_LINEDOT, 24, 22, NULL, hBmp, 7, uState);
    m_pGB->Add(utEx, 12, IDM_LINEDASHDOT, 24, 22, NULL, hBmp, 8
        , uState);
    m_pGB->Add(utEx, 13, IDM_LINEDASHDOTDOT, 24, 22, NULL, hBmp, 9
        , uState);

    //Check the current line style.
    i=m_pPL->LineStyleGet()+IDM_LINEMIN;
    m_pGB->Check(i, TRUE);

    m_pFR->m_fInit=FALSE;

    /*
     * Before making the GizmoBar visible, resize it to the
     * container's GetBorder rectangle.  By default the GizmoBar
     * sizes itself to the client area of the parent, but we can't
     * assume that's the same as GetBorder returns, so we do the
     * extra work here.
     */

    m_pIOleIPFrame->GetBorder(&rc);
    SetWindowPos(m_pGB->Window(), NULL, rc.left, rc.top
        , rc.right-rc.left, rc.top+m_cyBar, SWP_NOZORDER);


    //3.  Make the tools visible.
    ShowWindow(m_pGB->Window(), SW_SHOW);

    return TRUE;
    }






/*
 * CFigure::FInPlaceToolsDestroy
 *
 * Purpose:
 *  Reverses the process of FInPlaceGizmosCreate
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if successful, FALSE otherwise.
 */


BOOL CFigure::FInPlaceToolsDestroy(void)
    {
    //Nothing to do if we never created anything.
    if (NULL==m_pGB)
        return TRUE;

    /*
     * No reason to call SetBorderSpace with an empty rectangle
     * since you call IOleInPlaceSite::OnUIDeactivate.  The
     * container will restore its own tools appropriately.
     */

    //Destroy the gizmobar.
    if (NULL!=m_pGB)
        {
        delete m_pGB;
        m_pGB=NULL;
        g_pInPlaceGB=NULL;
        }

    return TRUE;
    }



/*
 * CFigure::FInPlaceToolsRenegotiate
 *
 * Purpose:
 *  Calls IOleInPlaceFrame::RequestBorderSpace and SetBorderSpace
 *  to reserve space for our toolbar.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if all is well, FALSE otherwise.
 */

BOOL CFigure::FInPlaceToolsRenegotiate(void)
    {
    HRESULT         hr;
    BORDERWIDTHS    bw;

    SetRect((LPRECT)&bw, 0, m_pFR->m_cyBar, 0, 0);

    hr=m_pIOleIPFrame->RequestBorderSpace(&bw);

    if (NOERROR!=hr)
        return FALSE;

    //Safety net:  RequestBorderSpace may modify values in bw
    SetRect((LPRECT)&bw, 0, m_pFR->m_cyBar, 0, 0);
    m_pIOleIPFrame->SetBorderSpace(&bw);
    return TRUE;
    }



/*
 * CFigure::OpenIntoWindow
 *
 * Purpose:
 *  If we're current open in-place, send ourselves the OPEN verb to
 *  show into a full window.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

void CFigure::OpenIntoWindow(void)
    {
    if (NULL!=m_pIOleIPSite)
        {
        //Make sure we don't try to do this.
        m_fUndoDeactivates=FALSE;

        /*
         * We can get away with passing a lot of NULLs since we know
         * how we implemented DoVerb.
         */
        m_pIOleObject->DoVerb(OLEIVERB_OPEN, NULL, m_pIOleClientSite
            , -1, NULL, NULL);

        //This makes sure we save ourselves when closing.
        m_fForceSave=TRUE;

        //Repaint the container immediately
        SendAdvise(OBJECTCODE_DATACHANGED);
        }

    return;
    }



/*
 * FUndo
 *
 * Purpose:
 *  If we have not done anything else in this object then call
 *  IOleInPlaceSite::DeactivateAndUndo and return TRUE, otherwise
 *  just return FALSE.  Note that the m_fUndoDeactivates is set
 *  to FALSE in CFigure::SendAdvise for OBJECTCODE_DATACHANGED.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if we deactivated, FALSE otherwise.
 */

BOOL CFigure::FUndo(void)
    {
    if (!m_fUndoDeactivates)
        return FALSE;

    m_fUndoDeactivates=FALSE;
    m_pIOleIPSite->DeactivateAndUndo();
    return TRUE;
    }




//End CHAPTER16MOD
