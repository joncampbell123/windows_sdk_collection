// STimeObj.cpp : Implementation of CMySettimeObject
//
//THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
//EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
//MERCHANTIBILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//
//Copyright (C) 1996 Microsoft Corporation. All Rights Reserved.

#include "stdafx.h"
#include "settime.h"
#include "STimeObj.h"

/////////////////////////////////////////////////////////////////////////////
//

// REVIEW: I currently succeed in this call because
// a) I have all machines logged in with administrator privilege
// b) I use DCOMCNFG.EXE, Object Identity set to "The Interactive User"
 
STDMETHODIMP CMySettimeObject::Set(SYSTEMTIME t) 
{
        // REVIEW: in case of failure, refer to the SetSystemTime()
        //  in the Win32 SDK documentation. The call might fail
        // because this process might not have SE_SYSTEMTIME_NAME privilege
        return SetSystemTime(&t) ? NOERROR : E_ACCESSDENIED; 
}
