/*
 -  S M P L M A P I . C P P
 -  Copyright (C) 1994 Microsoft Corporation
 -
 *  Purpose:
 *      Simple MAPI stub function pointers
 *
 */


#ifdef WIN32
#ifdef CHICAGO
#define _INC_OLE
#endif
#define INC_OLE2
#define INC_RPC
#endif

#include <afxwin.h>     
#include <windowsx.h>
#include <string.h>

#ifdef WIN16
#include <compobj.h>
#endif

#ifdef WIN32
#include <objbase.h>
#include <objerror.h>
#ifdef CHICAGO
#include <ole2.h>
#endif
#endif


#include <mapidefs.h>
#include <mapi.h>
#include <mapiwin.h>
#include "smplmapi.h"

HINSTANCE hlibMAPI;

BOOL InitMapiDll()
{             
    
#ifdef  WIN32
    if (!(hlibMAPI = LoadLibrary ("MAPI" szMAPIDLLSuffix ".DLL")))
#else
    if ((UINT)(hlibMAPI = LoadLibrary ("MAPI.DLL")) < 32)
#endif
        return (FALSE);

    if (!(lpfnMAPILogon = (LPFNMAPILOGON) GetProcAddress (hlibMAPI, "MAPILogon")))
        return (FALSE);
    if (!(lpfnMAPILogoff = (LPFNMAPILOGOFF) GetProcAddress (hlibMAPI, "MAPILogoff")))
        return (FALSE);
    if (!(lpfnMAPISendMail = (LPFNMAPISENDMAIL) GetProcAddress (hlibMAPI, "MAPISendMail")))
        return (FALSE);
    if (!(lpfnMAPISendDocuments = (LPFNMAPISENDDOCUMENTS) GetProcAddress (hlibMAPI, "MAPISendDocuments")))
        return (FALSE);
    if (!(lpfnMAPIFindNext = (LPFNMAPIFINDNEXT) GetProcAddress (hlibMAPI, "MAPIFindNext")))
        return (FALSE);
    if (!(lpfnMAPIReadMail = (LPFNMAPIREADMAIL) GetProcAddress (hlibMAPI, "MAPIReadMail")))
        return (FALSE);
    if (!(lpfnMAPISaveMail = (LPFNMAPISAVEMAIL) GetProcAddress (hlibMAPI, "MAPISaveMail")))
        return (FALSE);
    if (!(lpfnMAPIDeleteMail = (LPFNMAPIDELETEMAIL) GetProcAddress (hlibMAPI, "MAPIDeleteMail")))
        return (FALSE);
    if (!(lpfnMAPIFreeBuffer = (LPFNMAPIFREEBUFFER) GetProcAddress (hlibMAPI, "MAPIFreeBuffer")))
        return (FALSE);
    if (!(lpfnMAPIAddress = (LPFNMAPIADDRESS) GetProcAddress (hlibMAPI, "MAPIAddress")))
        return (FALSE);
    if (!(lpfnMAPIDetails = (LPFNMAPIDETAILS) GetProcAddress (hlibMAPI, "MAPIDetails")))
        return (FALSE);
    if (!(lpfnMAPIResolveName = (LPFNMAPIRESOLVENAME) GetProcAddress (hlibMAPI, "MAPIResolveName")))
        return (FALSE);

    return (TRUE);
}

void DeinitSimpleMAPI ()
{
    if (hlibMAPI)
    {
        FreeLibrary (hlibMAPI);
        hlibMAPI = 0;
    }
}
