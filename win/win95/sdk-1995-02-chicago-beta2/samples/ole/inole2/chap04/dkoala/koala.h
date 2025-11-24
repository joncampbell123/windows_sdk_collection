/*
 * KOALA.H
 * Koala Object DLL/EXE Chapter 4
 *
 * Classes that implement the Koala object independent of
 * whether we live in a DLL or EXE.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Right Reserved
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



/*
 * The Koala object is implemented in its own class with its own
 * IUnknown to support aggregation.  It contains one CImpIPersist
 * object that we use to implement the externally exposed interfaces.
 */

class CKoala : public IUnknown
    {
    //Make any contained interfaces friends
    friend class CImpIPersist;

    protected:
        ULONG           m_cRef;         //Object reference count
        LPUNKNOWN       m_pUnkOuter;    //Controlling unknown

        PFNDESTROYED    m_pfnDestroy;   //To call on closure
        PIMPIPERSIST    m_pIPersist;    //Our interface impl.

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


#endif //_KOALA_H_
