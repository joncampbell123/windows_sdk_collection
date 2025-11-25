// STimeObj.h : Declaration of the CMySettimeObject
//
//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
//EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
//MERCHANTIBILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//
//Copyright (C) 1996 Microsoft Corporation. All Rights Reserved.

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// settime

class CMySettimeObject : 
        public IMySettime,
        public CComObjectBase<&CLSID_MySettime>
{
public:
        CMySettimeObject() {}
BEGIN_COM_MAP(CMySettimeObject)
        COM_INTERFACE_ENTRY(IMySettime)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CMySettimeObject)

// IMySettime
public:
        STDMETHOD(Set)(SYSTEMTIME t);

};
