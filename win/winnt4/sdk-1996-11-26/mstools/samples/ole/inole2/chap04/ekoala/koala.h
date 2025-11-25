/*
 * KOALA.H
 * Koala Object DLL/EXE Chapter 4
 *
 * Classes that implement the Koala object independent of
 * whether we live in a DLL or EXE.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Right Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _KOALA_H_
#define _KOALA_H_

#define INC_OLE2
#include <windows.h>
#include <ole2.h>       //ole2.h has IPersist, compobj.h doesn't

#include <bookguid.h>

//Type for an object-destroyed callback
typedef void (PASCAL *PFNDESTROYED)(void);


//Forward class references
class CImpIPersist;
typedef class CImpIPersist *PIMPIPERSIST;

class CImpIExternalConnection;
typedef class CImpIExternalConnection *PIMPIEXTERNALCONNECTION;


/*
 * The Koala object is implemented in its own class with its own
 * IUnknown to support aggregation.  It contains one CImpIPersist
 * object that we use to implement the externally exposed interfaces.
 */

class CKoala : public IUnknown
    {
    //Make any contained interfaces friends
    friend class CImpIPersist;
    friend class CImpIExternalConnection;

    protected:
        ULONG           m_cRef;         //Object reference count
        LPUNKNOWN       m_pUnkOuter;    //Controlling unknown

        PFNDESTROYED    m_pfnDestroy;   //To call on closure
        PIMPIPERSIST    m_pIPersist;    //IPersist

        ULONG                   m_cStrong;      //Connection count
        PIMPIEXTERNALCONNECTION m_pIExtConn;    //IExternalConnection

    public:
        CKoala(LPUNKNOWN, PFNDESTROYED);
        ~CKoala(void);

        BOOL FInit(void);

        //Non-delegating object IUnknown
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);
    };

typedef CKoala *PCKoala;


/*
 * Interface implementations for the CKoala object.
 */

class CImpIPersist : public IPersist
    {
    private:
        ULONG           m_cRef;
        PCKoala         m_pObj;         //Back pointer to object
        LPUNKNOWN       m_pUnkOuter;    //Controlling unknown

    public:
        CImpIPersist(PCKoala, LPUNKNOWN);
        ~CImpIPersist(void);

        //IUnknown members that delegate to m_pUnkOuter.
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IPersist members
        STDMETHODIMP         GetClassID(LPCLSID);
    };


class CImpIExternalConnection : public IExternalConnection
    {
    protected:
        ULONG           m_cRef;      //Interface reference count
        PCKoala         m_pObj;      //Back pointer to the object
        LPUNKNOWN       m_pUnkOuter; //For delegation

    public:
        CImpIExternalConnection(PCKoala, LPUNKNOWN);
        ~CImpIExternalConnection(void);

        STDMETHODIMP QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        STDMETHODIMP_(DWORD) AddConnection(DWORD, DWORD);
        STDMETHODIMP_(DWORD) ReleaseConnection(DWORD, DWORD, BOOL);
    };




#endif //_KOALA_H_
