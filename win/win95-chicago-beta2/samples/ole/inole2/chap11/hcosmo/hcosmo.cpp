/*
 * HCOSMO.CPP
 * Cosmo Handler Chapter 11
 *
 * DLL exports for an object handler as well as class factory.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#define INITGUIDS
#include "hcosmo.h"


//Count number of objects and number of locks.
ULONG       g_cObj=0;
ULONG       g_cLock=0;

//DLL Instance handle
HINSTANCE   g_hInst=0;


/*
 * LibMain(32)
 *
 * Purpose:
 *  Entry point for Windows NT or Windows 3.1 DLLs.
 */

#ifdef WIN32
BOOL WINAPI DllMain(HINSTANCE hInstance, ULONG ulReason
    , LPVOID pvReserved)
    {
    if (DLL_PROCESS_ATTACH==ulReason)
        g_hInst=hInstance;

    return TRUE;
    }

#else

/*
 * LibMain (also called from Win32 LibMain32)
 *
 * Purpose:
 *  DLL-specific entry point called from LibEntry.
 */

int PASCAL LibMain(HINSTANCE hInst, WORD wDataSeg
    , WORD cbHeapSize, LPSTR lpCmdLine)
    {
    if (0!=cbHeapSize)
        UnlockData(0);

    g_hInst=hInst;
    return (int)hInst;
    }

#endif




/*
 * DllGetClassObject
 *
 * Purpose:
 *  Standard export for DLL component objects.
 */

HRESULT APIENTRY DllGetClassObject(REFCLSID rclsid
    , REFIID riid, PPVOID ppv)
    {
    if (CLSID_Cosmo2Figure!=rclsid)
        return ResultFromScode(E_FAIL);

    //Check that we can provide the interface
    if (IID_IUnknown!=riid && IID_IClassFactory!=riid)
        return ResultFromScode(E_NOINTERFACE);

    //Return our IClassFactory for Figure objects
    *ppv=new CFigureClassFactory;

    if (NULL==*ppv)
        return ResultFromScode(E_OUTOFMEMORY);

    ((LPUNKNOWN)*ppv)->AddRef();

    return NOERROR;
    }





/*
 * DllCanUnloadNow
 *
 * Purpose:
 *  Answers if the DLL can be freed, that is, if there are no
 *  references to anything this DLL provides.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  BOOL            TRUE if nothing is using us, FALSE otherwise.
 */

STDAPI DllCanUnloadNow(void)
    {
    SCODE   sc;

    //Our answer is whether there are any object or locks
    sc=(0L==g_cObj && 0==g_cLock) ? S_OK : S_FALSE;
    return ResultFromScode(sc);
    }




/*
 * ObjectDestroyed
 *
 * Purpose:
 *  Function for the Figure object to call when it gets destroyed.
 *  Since we're in a DLL we only track the number of objects here
 *  letting DllCanUnloadNow take care of the rest.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

void PASCAL ObjectDestroyed(void)
    {
    g_cObj--;
    return;
    }






/*
 * CFigureClassFactory::CFigureClassFactory
 *
 * Purpose:
 *  Constructor for an object supporting an IClassFactory that
 *  instantiates Figure objects.
 *
 * Parameters:
 *  None
 */

CFigureClassFactory::CFigureClassFactory(void)
    {
    m_cRef =0L;
    return;
    }





/*
 * CFigureClassFactory::~CFigureClassFactory
 *
 * Purpose:
 *  Destructor for a CFigureClassFactory object.  This will be
 *  called when we Release the object to a zero reference count.
 */

CFigureClassFactory::~CFigureClassFactory(void)
    {
    return;
    }






/*
 * CFigureClassFactory::QueryInterface
 * CFigureClassFactory::AddRef
 * CFigureClassFactory::Release
 *
 * Purpose:
 *  IUnknown implementations for this object.
 */

STDMETHODIMP CFigureClassFactory::QueryInterface(REFIID riid
    , PPVOID ppv)
    {
    *ppv=NULL;

    //Any interface on this object is the object pointer.
    if (IID_IUnknown==riid || IID_IClassFactory==riid)
        *ppv=this;

    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) CFigureClassFactory::AddRef(void)
    {
    return ++m_cRef;
    }


STDMETHODIMP_(ULONG) CFigureClassFactory::Release(void)
    {
    ULONG           cRefT;

    cRefT=--m_cRef;

    if (0L==m_cRef)
        delete this;

    return cRefT;
    }







/*
 * CFigureClassFactory::CreateInstance
 *
 * Purpose:
 *  Instantiates a Figure object that supports the IFigure
 *  and IUnknown interfaces.  If the caller asks for a different
 *  interface than these two then we fail.
 *
 * Parameters:
 *  pUnkOuter       LPUNKNOWN to the controlling IUnknown if we are
 *                  being used in an aggregation.
 *  riid            REFIID identifying the interface the caller
 *                  desires to have for the new object.
 *  ppvObj          PPVOID in which to store the desired interface
 *                  pointer for the new object.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, otherwise contains
 *                  E_NOINTERFACE if we cannot support the requested
 *                  interface.
 */

STDMETHODIMP CFigureClassFactory::CreateInstance(LPUNKNOWN pUnkOuter
    , REFIID riid, PPVOID ppvObj)
    {
    PCFigure            pObj;
    HRESULT             hr;

    *ppvObj=NULL;
    hr=ResultFromScode(E_OUTOFMEMORY);

    //Verify that a controlling unknown is asking for IUnknown
    if (NULL!=pUnkOuter && IID_IUnknown!=riid)
        return ResultFromScode(E_NOINTERFACE);

    //Create the object.  This also creates a window.
    pObj=new CFigure(pUnkOuter, ObjectDestroyed, g_hInst);

    if (NULL==pObj)
        return hr;

    if (pObj->FInit())
        hr=pObj->QueryInterface(riid, ppvObj);

    //Kill the object if initial creation or FInit failed.
    if (FAILED(hr))
        delete pObj;
    else
        g_cObj++;

    return hr;
    }






/*
 * CFigureClassFactory::LockServer
 *
 * Purpose:
 *  Increments or decrements the lock count of the DLL.  If the lock
 *  count goes to zero and there are no objects, the DLL is allowed
 *  to unload.  See DllCanUnloadNow.
 *
 * Parameters:
 *  fLock           BOOL specifying whether to increment or
 *                  decrement the lock count.
 *
 * Return Value:
 *  HRESULT         NOERROR always.
 */

STDMETHODIMP CFigureClassFactory::LockServer(BOOL fLock)
    {
    if (fLock)
        g_cLock++;
    else
        g_cLock--;

    return NOERROR;
    }
