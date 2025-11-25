/****************************************************************************
                       Microsoft RPC
                Copyright Microsoft Corp. 1992-1996
                  object call_as Example

    FILE:       server.cxx

    USAGE:      server  - note that this is auto-loaded by the client
                          requesting this server, and exits at completion

    PURPOSE:    Server side of RPC distributed application

    FUNCTIONS:  main() - binds to server and calls remote procedure

    COMMENTS:   This version of the Ole2 distributed application that
                prints "hello, world" (or other string) on the server
                features a non-rpcable custom interface being remoted with
                [call_as].

                It provides a class factory to manufacture a server object,
                and the implementation of the callas server object

****************************************************************************/

#define INC_OLE2
#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include "callas.h"

unsigned long   ObjectCount         = 0;
BOOL            fClassRegistered    = FALSE;
DWORD           dwHandle            = 0;

// this is the CLSID for the class factory for the server

const CLSID CLSID_CallAs
    = {0xf9246030,0x9f33,0x11cd,{0xb2,0x3f,0x00,0xaa,0x00,0x33,0x9c,0xce}};

IClassFactory *GetCallAsFactory();




// here are some convenient inline functions to reduce the casting involved
// with having the RefCount be unsigned instead of signed, and pass by
// reference

inline
unsigned long
IncrementRefCount( unsigned long & RefCount )
    {
    return InterlockedIncrement( (long *) &RefCount );
    }

inline
unsigned long
DecrementRefCount( unsigned long & RefCount )
    {
    return InterlockedDecrement( (long *) &RefCount );
    }







// this is the class factory for the CCallAs server
// it manufactures CCallAs server objects in response to a CreateInstance
// call

class CCallAsFactory : public IClassFactory

{
public:
    // note that the virtual methods must exactly match the set for
    // the IClassFactory in the idl (and callas.h)
    unsigned long    RefCount;

                            CCallAsFactory()
                                {
                                RefCount    = 0;
                                }

        HRESULT STDMETHODCALLTYPE QueryInterface(
                REFIID iid,
            void **ppv);

        ULONG STDMETHODCALLTYPE AddRef();

        ULONG STDMETHODCALLTYPE Release();

        HRESULT STDMETHODCALLTYPE CreateInstance(
            IUnknown *punkOuter,
            REFIID riid,
            void **ppv);

   HRESULT STDMETHODCALLTYPE LockServer(
        BOOL fLock );
};

// allocate a static classfactory to avoid construction races if
// we were multithreaded.
CCallAsFactory       gCallAsClassFactory;





class CCallAs   : public IHelloCallAs

{

    unsigned long    RefCount;

public:

    // remoted functions (virtual)

        HRESULT STDMETHODCALLTYPE QueryInterface(
                REFIID iid,
            void **ppv);

        ULONG STDMETHODCALLTYPE AddRef();

        ULONG STDMETHODCALLTYPE Release();

        MyBoolean __stdcall HelloProc(
            unsigned char *pszString);

        // local functions
        // constructor
                            CCallAs()
                                {
                                RefCount    = 0;
                                }

    void    *   operator new( size_t s )
                                {
                                return CoTaskMemAlloc( s );
                                }

    void        operator delete( void * pv )
                                {
                                CoTaskMemFree( pv );
                                }

};


//+---------------------------------------------------------------------------
//
//  Function:   main
//
//  Synopsis:
//
//  Arguments:
//
//  Returns:    void
//
//  Modifies:   nothing
//
//  Notes:      none
//
//----------------------------------------------------------------------------

void _cdecl main()
{
    HRESULT hr;
    MSG msg;
        IClassFactory *pClassFactory;

    //
    // Initialize OLE before calling any other OLE functions.
    //

    hr = CoInitialize(NULL);

    if( FAILED(hr))
    {
            printf("Server: CoInitialize failed(%x)\n",hr);
        return;
    }

        pClassFactory = GetCallAsFactory();

        hr = CoRegisterClassObject(CLSID_CallAs,
                                               (IUnknown *) pClassFactory,
                                               CLSCTX_LOCAL_SERVER,
                                               REGCLS_SINGLEUSE,
                                               &dwHandle);

        pClassFactory->Release();

        if (FAILED(hr))
            printf("Server: CoRegisterClassObject failed %x\n", hr);
        else
        {
                fClassRegistered = TRUE;

            // message loop
        while (GetMessage(&msg, NULL, 0, 0))
        {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }
        }
    CoUninitialize();

    return;
}

//+-------------------------------------------------------------------------
//
//  Method:     GetCallAsFactory
//
//  Synopsis:   Get an IClassFactory * for the CallAs class.
//
//--------------------------------------------------------------------------
IClassFactory *GetCallAsFactory()
{
    // increment the file object count the first time it is created
        if(gCallAsClassFactory.RefCount == 0)
        {
                IncrementRefCount(gCallAsClassFactory.RefCount);
                IncrementRefCount(ObjectCount);
        }
        else
        {
                IncrementRefCount(gCallAsClassFactory.RefCount);
        }
        return &gCallAsClassFactory;
}

//+-------------------------------------------------------------------------
//
//  Method:     CCallAsFactory::QueryInterface, public
//
//  Synopsis:   Query for an interface on the class factory.
//
//  Derivation: IUnknown
//
//--------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE
CCallAsFactory::QueryInterface (
    REFIID iid,
    void **ppv )
{
    HRESULT hr = E_NOINTERFACE;

        assert(ppv);

    *ppv = 0;
    if ( IsEqualGUID( iid, IID_IUnknown) ||
         IsEqualGUID( iid, IID_IClassFactory ) )
    {
        *ppv = this;
                AddRef();
            hr = S_OK;
    }

    return hr;
}
//+-------------------------------------------------------------------------
//
//  Method:     CCallAsFactory::AddRef, public
//
//  Synopsis:   Increment DLL reference counts
//
//  Derivation: IUnknown
//
//      Notes: We have a single instance of the CCallAsFactory.
//
//--------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE
CCallAsFactory::AddRef()
{
    IncrementRefCount(RefCount);
        return RefCount;
}

//+-------------------------------------------------------------------------
//
//  Method:     CCallAsFactory::Release, public
//
//  Synopsis:   Decrement DLL reference count
//
//  Derivation: IUnknown
//
//--------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE
CCallAsFactory::Release()
{
        long t;
        unsigned long count;

    t = DecrementRefCount(RefCount);

        if(t == 0)
        {
                count = 0;
                //decrement the object count
                if(DecrementRefCount(ObjectCount) == 0)
                {
                        //The last object has been destroyed.
                        PostQuitMessage(0);
                }
        }
        else
                count = RefCount;

    return count;
}

//+-------------------------------------------------------------------------
//
//  Method:     CCallAsFactory::CreateInstance, public
//
//  Synopsis:   Create an instance of CCallAs.
//
//--------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE
CCallAsFactory::CreateInstance
(
    IUnknown *punkOuter,
    REFIID riid,
    void **ppv
)
{
    HRESULT hr = E_OUTOFMEMORY;
        CCallAs *pCallAs;

        assert(ppv);

        *ppv = 0;

        if(fClassRegistered == TRUE)
        {
                fClassRegistered = FALSE;
                CoRevokeClassObject(dwHandle);
        }

        pCallAs = new CCallAs();
        if(pCallAs)
        {
            // invoke the constructor
                *pCallAs = CCallAs();

                //increment the object count.
                //The object count will keep this process alive until all
                //objects are released.
                IncrementRefCount(ObjectCount);
                hr = pCallAs->QueryInterface( riid, ppv);
        }

    return hr;
}

//+-------------------------------------------------------------------------
//
//  Method:     CCallAsFactory::LockServer, public
//
//  Synopsis:   Lock the server in memory (by adding an extra reference)
//
//--------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE
CCallAsFactory::LockServer(
        BOOL fLock )
{
    if ( fLock )
            IncrementRefCount( ObjectCount );
        else
            DecrementRefCount( ObjectCount );

        return S_OK;

}

//+-------------------------------------------------------------------------
//
//  Method:     CCallAs::QueryInterface, public
//
//  Synopsis:   Query for an interface on the class factory.
//
//  Derivation: IUnknown
//
//--------------------------------------------------------------------------
HRESULT STDMETHODCALLTYPE
CCallAs::QueryInterface (
    REFIID iid,
    void **ppv )
{
    HRESULT hr = E_NOINTERFACE;

        assert(ppv);

    *ppv = 0;
    if ( IsEqualGUID( iid, IID_IUnknown ) ||
         IsEqualGUID( iid, IID_IHelloCallAs ) )
    {
        *ppv = this;
                AddRef();
            hr = S_OK;
    }

    return hr;
}
//+-------------------------------------------------------------------------
//
//  Method:     CCallAs::AddRef, public
//
//  Synopsis:   Increment reference count
//
//  Derivation: IUnknown
//
//--------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE
CCallAs::AddRef()
{
    IncrementRefCount(RefCount);
        return RefCount;
}

//+-------------------------------------------------------------------------
//
//  Method:     CCallAs::Release, public
//
//  Synopsis:   Decrement DLL reference count
//
//  Derivation: IUnknown
//
//--------------------------------------------------------------------------
ULONG STDMETHODCALLTYPE
CCallAs::Release()
{
        long t;
        unsigned long count;

    t = DecrementRefCount(RefCount);

        if(t == 0)
        {
                count = 0;

                //decrement the object count
                if(DecrementRefCount(ObjectCount) == 0)
                {
                        //The last object has been destroyed.
                        PostQuitMessage(0);
                }
                delete this;
        }
        else
                count = RefCount;

    return count;
}

//+---------------------------------------------------------------------------
//
//  Function:   CCallAs::HelloProc()
//
//  Synopsis:   Displays the specified string
//
//  Arguments:  void
//
//  Returns:    Success.
//
//  Modifies:   nothing
//
//  Notes:      none
//
//----------------------------------------------------------------------------
MyBoolean __stdcall
CCallAs::HelloProc(unsigned char *pszString)
{
    printf("HelloProc: %s \n", pszString);
    printf(" <pausing for 5 sec>\n");
    // pretend to be busy computing for a bit
    Sleep( 5000 ); // 5000 msec = 5 sec

    return TRUE;
}
