/*
 * IENUMFE.H
 *
 * Definitions of a FORMATETC enumerator.
 *
 * Copyright (c)1993-1996 Microsoft Corporation, All Right Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _IENUMFE_H_
#define _IENUMFE_H_

/*
 * IEnumFORMATETC object that is created from
 * IDataObject::EnumFormatEtc.  This object lives on its own,
 * that is, QueryInterface only knows IUnknown and IEnumFormatEtc,
 * nothing more.  We still use an outer unknown for reference
 * counting, because as long as this enumerator lives, the data
 * object should live, thereby keeping the application up.
 */

class CEnumFormatEtc;
typedef class CEnumFormatEtc *PCEnumFormatEtc;

class CEnumFormatEtc : public IEnumFORMATETC
    {
    private:
        ULONG           m_cRef;     //Object reference count
        LPUNKNOWN       m_pUnkRef;  //IUnknown for ref counting
        ULONG           m_iCur;     //Current element
        ULONG           m_cfe;      //Number of FORMATETCs in us
        LPFORMATETC     m_prgfe;    //Source of FORMATETCs

    public:
        CEnumFormatEtc(LPUNKNOWN, ULONG, LPFORMATETC);
        ~CEnumFormatEtc(void);

        //IUnknown members that delegate to m_pUnkRef.
        STDMETHODIMP         QueryInterface(REFIID, LPVOID *);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IEnumFORMATETC members
        STDMETHODIMP Next(ULONG, LPFORMATETC, ULONG *);
        STDMETHODIMP Skip(ULONG);
        STDMETHODIMP Reset(void);
        STDMETHODIMP Clone(IEnumFORMATETC **);
    };


#endif //_IENUMFE_H_
