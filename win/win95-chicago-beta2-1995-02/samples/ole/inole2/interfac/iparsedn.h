/*
 * IPARSEDN.H
 *
 * Definitions of a template IParseDisplayName interface
 * implementation.
 *
 * Copyright (c)1993-1994 Microsoft Corporation, All Right Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _IPARSEDN_H_
#define _IPARSEDN_H_

class CImpIParseDisplayName;
typedef class CImpIParseDisplayName *PIMPIPARSEDISPLAYNAME;

class CImpIParseDisplayName : public IParseDisplayName
    {
    private:
        ULONG           m_cRef;      //Interface reference count
        LPVOID          m_pObj;      //Back pointer to the object
        LPUNKNOWN       m_pUnkOuter; //For delegation

    public:
        CImpIParseDisplayName(LPVOID, LPUNKNOWN);
        ~CImpIParseDisplayName(void);

        //IUnknown members that delegate to m_pUnkOuter.
        STDMETHODIMP         QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IParseDisplayName members
        STDMETHODIMP ParseDisplayName(LPBC, LPTSTR
            , ULONG *, LPMONIKER *);
    };


#endif //_IPARSEDN_H_
