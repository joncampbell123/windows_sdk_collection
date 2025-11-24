/*
 * DATATRAN.H
 * Data Transfer Object
 *
 * Definitions, classes, and prototypes for a DLL that provides
 * data transfer objects.
 *
 * Copyright (c)1993-1995 Microsoft Corporation, All Rights Reserved
 *
 * Kraig Brockschmidt, Software Design Engineer
 * Microsoft Systems Developer Relations
 *
 * Internet  :  kraigb@microsoft.com
 * Compuserve:  >INTERNET:kraigb@microsoft.com
 */


#ifndef _DATATRAN_H_
#define _DATATRAN_H_


#include "dataobj.h"


//DATATRAN.CPP
void PASCAL ObjectDestroyed(void);


//This class factory object creates Data Transfer Objects.

class CDataTransferClassFactory : public IClassFactory
    {
    protected:
        ULONG           m_cRef;

    public:
        CDataTransferClassFactory(void);
        ~CDataTransferClassFactory(void);

        //IUnknown members
        STDMETHODIMP         QueryInterface(REFIID, PPVOID);
        STDMETHODIMP_(ULONG) AddRef(void);
        STDMETHODIMP_(ULONG) Release(void);

        //IClassFactory members
        STDMETHODIMP         CreateInstance(LPUNKNOWN, REFIID
                                 , PPVOID);
        STDMETHODIMP         LockServer(BOOL);
    };

typedef CDataTransferClassFactory *PCDataTransferClassFactory;

#endif //_DATATRAN_H_
