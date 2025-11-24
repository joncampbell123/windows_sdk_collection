/*** 
*cenumpt.h
*
*  Copyright (C) 1992, Microsoft Corporation.  All Rights Reserved.
*  Information Contained Herein Is Proprietary and Confidential.
*
*Purpose:
*  Definition of the CEnumPoint class.
*
*Implementation Notes:
*
*****************************************************************************/

class CEnumPoint : public IEnumVARIANT 
{
public:
    static HRESULT Create(SAFEARRAY FAR* psa, CEnumPoint FAR* FAR* ppenum);

    /* IUnknown methods */
    STDMETHOD(QueryInterface)(REFIID riid, void FAR* FAR* ppv);
    STDMETHOD_(ULONG, AddRef)(void);
    STDMETHOD_(ULONG, Release)(void);

    /* IEnumVARIANT methods */
    STDMETHOD(Next)(ULONG celt, VARIANT FAR* rgvar, ULONG FAR* pceltFetched);
    STDMETHOD(Skip)(ULONG celt);
    STDMETHOD(Reset)(void);
    STDMETHOD(Clone)(IEnumVARIANT FAR* FAR* ppenum);

    CEnumPoint();

private:

    ULONG m_refs;

    ULONG m_celts;
    ULONG m_iCurrent;
    SAFEARRAY FAR* m_psa;
};
