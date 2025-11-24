/*
 * POLYLINE.CPP
 * Polyline Component Object Chapter 6
 *
 * Implementation of the CPolyline class that we expose as a
 * component object.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#include "polyline.h"


/*
 * CPolyline:CPolyline
 * CPolyline::~CPolyline
 *
 * Constructor Parameters:
 *  pUnkOuter       LPUNKNOWN of the controlling unknown.
 *  pfnDestroy      PFNDESTROYED to call when an object is
 *                  destroyed.
 *  hInst           HINSTANCE of the application we're in.
 */

CPolyline::CPolyline(LPUNKNOWN pUnkOuter, PFNDESTROYED pfnDestroy
    , HINSTANCE hInst)
    {
    m_hWnd=NULL;
    m_hInst=hInst;

    m_cRef=0;
    m_pUnkOuter=pUnkOuter;
    m_pfnDestroy=pfnDestroy;
    m_fDirty=FALSE;

    m_pST  =NULL;
    m_cf   =0;

    m_pIStorage=NULL;
    m_pIStream =NULL;

    //CHAPTER6MOD
    m_clsID=CLSID_Polyline6;

    //NULL any contained interfaces initially.
    m_pIPolyline        =NULL;
    m_pIPersistStorage  =NULL;
    m_pIDataObject      =NULL;
    m_pIDataAdviseHolder=NULL;
    m_pAdv              =NULL;

    //End CHAPTER6MOD
    return;
    }


CPolyline::~CPolyline(void)
    {
    if (NULL!=m_pST)
        delete m_pST;

    //CHAPTER6MOD
    if (NULL!=m_pIDataAdviseHolder)
        m_pIDataAdviseHolder->Release();

    if (NULL!=m_pIDataObject)
        delete m_pIDataObject;
    //End CHAPTER6MOD

    if (NULL!=m_pIPersistStorage)
        delete m_pIPersistStorage;

    if (NULL!=m_pIStream)
        m_pIStream->Release();

    if (NULL!=m_pIStorage)
        m_pIStorage->Release();

    if (NULL!=m_pIPolyline)
        delete m_pIPolyline;    //Interface does not free itself.

    //Reverses the AddRef in IPolyline::SetAdvise
    if (NULL!=m_pAdv)
        m_pAdv->Release();

    return;
    }




/*
 * CPolyline::FInit
 *
 * Purpose:
 *  Performs any intiailization of a CPolyline that's prone to
 *  failure that we also use internally before exposing the
 *  object outside this DLL.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if the function is successful,
 *                  FALSE otherwise.
 */

BOOL CPolyline::FInit(void)
    {
    LPUNKNOWN       pIUnknown=this;

    if (NULL!=m_pUnkOuter)
        pIUnknown=m_pUnkOuter;

    m_pST=new CStringTable(m_hInst);

    if (!m_pST->FInit(IDS_POLYLINEMIN, IDS_POLYLINEMAX))
        return FALSE;

    m_cf=RegisterClipboardFormat(SZPOLYLINECLIPFORMAT);

    //Allocate contained interfaces.
    m_pIPolyline=new CImpIPolyline(this, pIUnknown);

    if (NULL==m_pIPolyline)
        return FALSE;

    m_pIPersistStorage=new CImpIPersistStorage(this, pIUnknown);

    if (NULL==m_pIPersistStorage)
        return FALSE;

    //CHAPTER6MOD
    m_pIDataObject=new CImpIDataObject(this, pIUnknown);

    if (NULL==m_pIDataObject)
        return FALSE;

    //Initialize IDataObject::EnumFormatEtc arrays
    m_cfeGet=CFORMATETCGET;
    m_cfeSet=CFORMATETCSET;

    SETDefFormatEtc(m_rgfeGet[0], m_cf, TYMED_HGLOBAL);
    SETDefFormatEtc(m_rgfeGet[1], CF_METAFILEPICT, TYMED_MFPICT);
    SETDefFormatEtc(m_rgfeGet[2], CF_BITMAP, TYMED_GDI);

    SETDefFormatEtc(m_rgfeSet[0], m_cf, TYMED_HGLOBAL);
    //End CHAPTER6MOD

    return TRUE;
    }







/*
 * CPolyline::QueryInterface
 * CPolyline::AddRef
 * CPolyline::Release
 *
 * Purpose:
 *  IUnknown members for CPolyline object.
 */

STDMETHODIMP CPolyline::QueryInterface(REFIID riid, PPVOID ppv)
    {
    *ppv=NULL;

    /*
     * The only calls we get here for IUnknown are either in a
     * non-aggregated case or when we're created in an
     * aggregation, so in either we always return our IUnknown
     * for IID_IUnknown.
     */
    if (IID_IUnknown==riid)
        *ppv=this;

    //For anything else we return contained interfaces.
    //CHAPTER6MOD
    if (IID_IPolyline6==riid)
        *ppv=m_pIPolyline;
    //End CHAPTER6MOD

    if (IID_IPersist==riid || IID_IPersistStorage==riid)
        *ppv=m_pIPersistStorage;

    //CHAPTER6MOD
    if (IID_IDataObject==riid)
        *ppv=m_pIDataObject;
    //End CHAPTER6MOD

    //AddRef any interface we'll return.
    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) CPolyline::AddRef(void)
    {
    return ++m_cRef;
    }


STDMETHODIMP_(ULONG) CPolyline::Release(void)
    {
    ULONG       cRefT;

    cRefT=--m_cRef;

    if (0L==m_cRef)
        {
        if (NULL!=m_pfnDestroy)
            (*m_pfnDestroy)();

        delete this;
        }

    return cRefT;
    }







/*
 * CPolyline::RectConvertMappings
 *
 * Purpose:
 *  Converts the contents of a rectangle from device (MM_TEXT) or
 *  HIMETRIC to the other.
 *
 * Parameters:
 *  pRect           LPRECT containing the rectangle to convert.
 *  fToDevice       BOOL TRUE to convert from HIMETRIC to device,
 *                  FALSE to convert device to HIMETRIC.
 *
 * Return Value:
 *  None
 */

void CPolyline::RectConvertMappings(LPRECT pRect, BOOL fToDevice)
    {
    HDC      hDC;
    int      iLpx, iLpy;

    if (NULL==pRect)
        return;

    hDC=GetDC(NULL);
    iLpx=GetDeviceCaps(hDC, LOGPIXELSX);
    iLpy=GetDeviceCaps(hDC, LOGPIXELSY);
    ReleaseDC(NULL, hDC);

    if (fToDevice)
        {
        pRect->left=MulDiv(iLpx, pRect->left, HIMETRIC_PER_INCH);
        pRect->top =MulDiv(iLpy, pRect->top , HIMETRIC_PER_INCH);

        pRect->right =MulDiv(iLpx, pRect->right, HIMETRIC_PER_INCH);
        pRect->bottom=MulDiv(iLpy, pRect->bottom,HIMETRIC_PER_INCH);
        }
    else
        {
        pRect->left=MulDiv(pRect->left, HIMETRIC_PER_INCH, iLpx);
        pRect->top =MulDiv(pRect->top , HIMETRIC_PER_INCH, iLpy);

        pRect->right =MulDiv(pRect->right, HIMETRIC_PER_INCH, iLpx);
        pRect->bottom=MulDiv(pRect->bottom,HIMETRIC_PER_INCH, iLpy);
        }

    return;
    }



//CHAPTER6MOD
//Functions now internal due to IDataObject

/*
 * CPolyline::DataSet
 *
 * Purpose:
 *  Sets the current data in this Polyline to a given structure.
 *
 * Parameters:
 *  pplIn           PPOLYLINEDATA to initialize to.
 *  fSizeToData     BOOL indicating if we're to size to the data
 *                  or scale it.
 *  fNotify         BOOL indicating if we're to send an advise
 *                  on this change.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, otherwise a
 *                  POLYLINE_E_ value.
 */

STDMETHODIMP CPolyline::DataSet(PPOLYLINEDATA pplIn
    , BOOL fSizeToData, BOOL fNotify)
    {
    RECT            rc;

    /*
     * Copy the structure in pplIn and repaint to reflect the
     * new point set.  Note that unlike the RectSet message, we
     * do no scaling, assuming that the rect in the structure
     * is appropriate for the data.
     */

    if (NULL==pplIn)
        return ResultFromScode(POLYLINE_E_INVALIDPOINTER);

    m_pl=*pplIn;

    //CHAPTER6MOD
    m_fDirty=TRUE;

    //Inform our parent of the data change
    if (NULL!=m_pIDataAdviseHolder)
        {
        m_pIDataAdviseHolder->SendOnDataChange(m_pIDataObject
            , DVASPECT_CONTENT, ADVF_NODATA);
        }
    //End CHAPTER6MOD

    /*
     * If we're scaling the window to fit the data, then use
     * RectSet passing our current rectangle as the new one.
     * That makes sure that the data won't change but that the
     * window is resized.
     */

    if (fSizeToData)
        {
        POINT       pt;

        /*
         * Get our offset in the parent window so we can RectSet
         * to the right place since RectSet expects rectangle in
         * parent coordinates and we get it in client coordinates.
         */
        GetWindowRect(m_hWnd, &rc);
        pt.x=rc.left;
        pt.y=rc.top;
        ScreenToClient(GetParent(m_hWnd), &pt);
        RECTSTORECT(m_pl.rc, rc);
        OffsetRect(&rc, pt.x, pt.y);

        //This will also cause a repaint.
        m_pIPolyline->RectSet(&rc, fNotify);
        }
    else
        {
        //Make sure we're updated.
        InvalidateRect(m_hWnd, NULL, TRUE);
        UpdateWindow(m_hWnd);
        }

    return NOERROR;
    }







/*
 * CPolyline::DataGet
 *
 * Purpose:
 *  Retrieves the Polyline's current data.
 *
 * Parameters:
 *  pplIn           PPOLYLINEDATA into which we copy the data.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, otherwise a
 *                  POLYLINE_E_ value.
 */

STDMETHODIMP CPolyline::DataGet(PPOLYLINEDATA pplIn)
    {
    if (NULL==pplIn)
        return ResultFromScode(POLYLINE_E_INVALIDPOINTER);

    *pplIn=m_pl;
    return NOERROR;
    }




/*
 * DataGet/SetMem eliminated:  no need with IDataObject.
 * DataGetMem is somewhat like RenderNative below.
 */



/*
 * CPolyline::RenderNative
 *
 * Purpose:
 *  Retrieves the Polyline's data in a global memory handle.
 *
 * Parameters:
 *  phMem           HGLOBAL * in which to store the handle.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, otherwise a
 *                  POLYLINE_E_ value.
 */

STDMETHODIMP CPolyline::RenderNative(HGLOBAL *phMem)
    {
    HGLOBAL         hMem;
    PPOLYLINEDATA   ppl;
    HRESULT         hr=ResultFromScode(POLYLINE_E_INVALIDPOINTER);

    if (NULL==phMem)
        return ResultFromScode(POLYLINE_E_INVALIDPOINTER);

    hMem=GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, CBPOLYLINEDATA);

    if (NULL!=hMem)
        {
        ppl=(PPOLYLINEDATA)GlobalLock(hMem);
        hr=DataGet(ppl);

        GlobalUnlock(hMem);

        if (FAILED(hr))
            {
            GlobalFree(hMem);
            hMem=NULL;
            }
        }

    *phMem=hMem;
    return hr;
    }







/*
 * CPolyline::RenderBitmap
 *
 * Purpose:
 *  Creates a bitmap image of the current Polyline.
 *
 * Parameters:
 *  phBmp           HBITMAP * in which to return the bitmap.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, otherwise a
 *                  POLYLINE_E_ value.
 */

STDMETHODIMP CPolyline::RenderBitmap(HBITMAP *phBmp)
    {
    HDC             hDC;
    HDC             hMemDC;
    HBITMAP         hBmp;
    RECT            rc;
    HGDIOBJ         hObj;

    if (NULL==phBmp)
        return ResultFromScode(POLYLINE_E_INVALIDPOINTER);

    //Render a bitmap the size of the current rectangle.
    hDC=GetDC(m_hWnd);
    hMemDC=CreateCompatibleDC(hDC);

    GetClientRect(m_hWnd, &rc);
    hBmp=CreateCompatibleBitmap(hDC, rc.right, rc.bottom);

    if (NULL!=hBmp)
        {
        //Draw the POLYLINEDATA into the bitmap.
        hObj=SelectObject(hMemDC, hBmp);
        Draw(hMemDC, FALSE, TRUE);
        SelectObject(hMemDC, hObj);
        }

    DeleteDC(hMemDC);
    ReleaseDC(m_hWnd, hDC);

    *phBmp=hBmp;
    return NOERROR;
    }



//RenderMetafile not necessary--now part of RenderMetafilePict.



/*
 * CPolyline::RenderMetafilePict
 *
 * Purpose:
 *  Renders the current Polyline into a METAFILEPICT structure in
 *  global memory.
 *
 * Parameters:
 *  phMem           HGLOBAL * in which to return the
 *                  METAFILEPICT.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, otherwise a
 *                  POLYLINE_E_ value.
 */

STDMETHODIMP CPolyline::RenderMetafilePict(HGLOBAL *phMem)
    {
    HGLOBAL         hMem;
    HMETAFILE       hMF;
    LPMETAFILEPICT  pMF;
    RECT            rc;
    HDC             hDC;

    if (NULL==phMem)
        return ResultFromScode(POLYLINE_E_INVALIDPOINTER);

    //Create a memory metafile and return its handle.
    hDC=(HDC)CreateMetaFile(NULL);

    if (NULL==hDC)
        return ResultFromScode(STG_E_MEDIUMFULL);

    SetMapMode(hDC, MM_ANISOTROPIC);
    GetClientRect(m_hWnd, &rc);
    SetWindowOrgEx(hDC, 0, 0, NULL);
    SetWindowExtEx(hDC, rc.right, rc.bottom, NULL);

    Draw(hDC, TRUE, TRUE);
    hMF=CloseMetaFile(hDC);

    if (NULL==hMF)
        return ResultFromScode(STG_E_MEDIUMFULL);

    //Allocate the METAFILEPICT structure.
    hMem=GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE
        , sizeof(METAFILEPICT));

    if (NULL==hMem)
        {
        DeleteMetaFile(hMF);
        return ResultFromScode(E_FAIL);
        }

    /*
     * Global lock only fails in PMODE if the selector is invalid
     * (like it was discarded) or references a 0 length segment,
     * neither of which can happen here.
     */
    pMF=(LPMETAFILEPICT)GlobalLock(hMem);

    pMF->hMF=hMF;
    pMF->mm=MM_ANISOTROPIC;

    //Insert the extents in MM_HIMETRIC units.
    GetClientRect(m_hWnd, &rc);
    RectConvertMappings(&rc, FALSE);
    pMF->xExt=rc.right;
    pMF->yExt=rc.bottom;

    GlobalUnlock(hMem);

    *phMem=hMem;
    return NOERROR;
    }


//End CHAPTER6MOD
