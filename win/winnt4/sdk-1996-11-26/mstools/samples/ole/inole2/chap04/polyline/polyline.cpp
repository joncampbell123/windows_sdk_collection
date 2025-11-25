/*
 * POLYLINE.CPP
 * Polyline Component Object Chapter 4
 *
 * Implementation of the CPolyline class that we expose as a
 * component object.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Rights Reserved
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

    //NULL any contained interfaces initially.
    m_pIPolyline=NULL;
    m_pAdv      =NULL;

    return;
    }


CPolyline::~CPolyline(void)
    {
    //Free our contained interfaces
    if (NULL!=m_pIPolyline)
        delete m_pIPolyline;

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

    //Allocate contained interfaces.
    m_pIPolyline=new CImpIPolyline(this, pIUnknown);

    if (NULL==m_pIPolyline)
        return FALSE;

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

    //For anything else we return our contained interface
    if (IID_IPolyline4==riid)
        *ppv=m_pIPolyline;

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
