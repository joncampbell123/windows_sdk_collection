/*
 -  S H O O F . C
 -
 *  Purpose:
 *      This is a sample SpoolerMsgHook Provider for generating
 *      Out of OFfice messages on every message received.
 *
 *  Copyright 1992-94 Microsoft Corporation.  All Rights Reserved.
 *
 */

#include <windows.h>
#include <string.h>
#include <mapiwin.h>
#include <mapix.h>
#include <mapispi.h>
#include <mapicode.h>
#include <mapitags.h>
#include <mapiguid.h>
#include <mapihook.h>
#include "shoof.h"

static char szOOF[] = TEXT ("OOF: ");

#define NUM_MSG_PROPS   3

/* OOF Hook Provider Jump Table */

SH_Vtbl vtblSH =
{
    SH_QueryInterface,
    SH_AddRef,
    SH_Release,
    SH_InboundMsgHook,
    NULL
};


/*
 -  IBProviderInit
 -
 *  Purpose:
 *      This is the Providers initialization entry point.  This gets
 *      called when the provider is loaded.  Here, we create an
 *      ISpoolerHook object to be passed back to the Spooler.
 *
 */

STDINITMETHODIMP
HPProviderInit (LPMAPISESSION lpSessObj,
    HINSTANCE hInstance,
    LPALLOCATEBUFFER lpAllocateBuffer,
    LPALLOCATEMORE lpAllocateMore,
    LPFREEBUFFER lpFreeBuffer,
    LPMAPIUID lpSectionUID,
    ULONG ulFlags,
    LPSPOOLERHOOK FAR * lppSpoolerHook)
{
    SCODE sc;
    LPSH lpsh;
    LPUNKNOWN lpunk = NULL;

    *lppSpoolerHook = NULL;

    /*  Allocate a OOF Hook Provider object */

    sc = (*lpAllocateBuffer) (sizeof (SH), (LPVOID *) &lpsh);
    if (FAILED (sc))
        return ResultFromScode (sc);

    /* Check the Spooler's Session Interface */

    sc = GetScode (lpSessObj->lpVtbl->QueryInterface (lpSessObj,
        (REFIID)&IID_IMAPISession, &lpunk));
    if (FAILED (sc))
        return ResultFromScode (sc);

    /* Fill in all fields of the object */

    lpsh->lpVtbl = &vtblSH;
    lpsh->ulcRef = 1;
    lpsh->hInstance = hInstance;
    lpsh->lpSession = lpSessObj;
    lpsh->lpfAllocateBuffer = lpAllocateBuffer;
    lpsh->lpfAllocateMore = lpAllocateMore;
    lpsh->lpfFreeBuffer = lpFreeBuffer;
    lpsh->UIDSection = *lpSectionUID;

    *lppSpoolerHook = (LPSPOOLERHOOK) lpsh;

    return hrSuccess;
}


/*
 -  ISpoolerHook::InboundMsgHook
 -
 *  Purpose:
 *      This is the ISpoolerHook method that gets called during
 *      InBound message processing in the spooler.  What we will
 *      do is check to see if the person receiving this message
 *      was on the To: or Cc: line (and not just a member of a
 *      DistList).  If so, then we will create an OOF message
 *      that we will send to the Sender of the original message
 *      to let them know we are out of the office and have received
 *      their message.
 */

STDMETHODIMP
SH_InboundMsgHook (LPSH lpsh,
    LPMESSAGE lpMessage,
    LPMAPIFOLDER lpDefaultFolder,
    LPMDB lpDefaultMDB,
    ULONG FAR * lpulFlags,
    ULONG FAR * lpcbEntryID,
    LPBYTE FAR * lppEntryID)
{
    HRESULT hResult = hrSuccess;
    ULONG cValues;
    LPSPropValue lpProps = NULL;
    ULONG ulObjType = 0;
    LPMAPIFOLDER lpFolder = NULL;
    LPMESSAGE lpOOFMsg = NULL;

    static SizedSPropTagArray (3, SPTA_MsgProps) =
    {
        3,
        {
            PR_MESSAGE_RECIP_ME,
            PR_SUBJECT,
            PR_MESSAGE_FLAGS
        }
    };

    hResult = lpMessage->lpVtbl->GetProps (lpMessage,
        (LPSPropTagArray) &SPTA_MsgProps, 0, /* ansi */
        &cValues, &lpProps);

    if (HR_FAILED(hResult))
        goto ret;

    /*  Look for OOF: in the subject.  If found, abort! */
    /*  It is not cool to OOF an OOF message.           */

    if ((lpProps[1].ulPropTag == PR_SUBJECT) &&
        (lpProps[1].Value.LPSZ != NULL) &&
        strstr (lpProps[1].Value.LPSZ, szOOF))
        goto ret;

    /*  Test to see if I sent this message to myself via the   */
    /*  PR_MESSAGE_FLAGS property containing MSGFLAG_FROMME.   */
    /*  If so, then don't OOF myself!                          */
    
    if ((lpProps[2].ulPropTag == PR_MESSAGE_FLAGS) &&
        (lpProps[2].Value.l == MSGFLAG_FROMME))
        goto ret;
        
    /*  Test to see if I was explicitly listed on the To:      */
    /*  or Cc: field (and not in Bcc: or part of a DistList).  */

    if ((lpProps->ulPropTag == PR_MESSAGE_RECIP_ME) && (lpProps->Value.b))
    {
        /*  We will create the OOF message in the Root Folder  */
        /*  So, we need to open it and pass this to the Create */

        hResult = lpDefaultMDB->lpVtbl->OpenEntry (lpDefaultMDB, 0, NULL,
            NULL, MAPI_MODIFY, &ulObjType, (LPUNKNOWN FAR *) &lpFolder);

        if (hResult || (ulObjType != MAPI_FOLDER))
        {
            hResult = ResultFromScode(MAPI_E_NOT_FOUND);
            goto ret;
        }

        /*  Create the OOF message in lpFolder */

        hResult = HrCreateOOFMessage (lpsh, lpFolder, lpMessage, &lpOOFMsg);

        if (hResult)
            goto ret;

        /*  It's ready for sending! */

        hResult = lpOOFMsg->lpVtbl->SubmitMessage (lpOOFMsg, FORCE_SUBMIT);
    }

ret:
    lpsh->lpfFreeBuffer(lpProps);

    if (lpFolder)
        lpFolder->lpVtbl->Release (lpFolder);
    if (lpOOFMsg)
        lpOOFMsg->lpVtbl->Release (lpOOFMsg);

    return hResult;
}


/*
 -  HrCreateOOFMessage
 *
 *  Purpose:
 *      Creates an OOF message in the specified folder with
 *      all the correct properties set and ready for submission.
 */

STDMETHODIMP
HrCreateOOFMessage (LPSH lpsh,
    LPMAPIFOLDER lpFolder,
    LPMESSAGE lpMsgOrig,
    LPMESSAGE FAR * lppMsg)
{
    SCODE sc;
    HRESULT hResult;
    LPTSTR lpszOOFSubject = NULL;
    LPTSTR lpszOOFBody = TEXT ("I am currently out of the office.  See Ya!");
    SPropValue rgMsgProps[NUM_MSG_PROPS];
    LPSPropValue rgAdrProps = NULL;
    ULONG ulObjType;
    ULONG cb;
    ULONG cVals;
    LPSPropValue lpMsgProps = NULL;
    LPSPropValue lpAdrProps = NULL;
    LPADRLIST lpMods = NULL;
    LPMESSAGE lpMsg = NULL;
    LPMAPIPROP lpUser = NULL;

    /*  Properties we'll be requesting of the message and sender */

    static SizedSPropTagArray (NUM_MSG_PROPS, SPTA_MsgProps) =
    {
        NUM_MSG_PROPS,
        {
            PR_NORMALIZED_SUBJECT,
            PR_SENDER_NAME,
            PR_SENDER_ENTRYID
        }
    };

    static SizedSPropTagArray (2, SPTA_AdrProps) =
    {
        2,
        {
            PR_ADDRTYPE,
            PR_EMAIL_ADDRESS
        }
    };

    *lppMsg = NULL;

    /*  Get a few properties from the original message */
    /*  If there are any errors, then we must bail!    */

    hResult = lpMsgOrig->lpVtbl->GetProps (lpMsgOrig,
        (LPSPropTagArray) &SPTA_MsgProps, 0, /* ansi */
        &cVals, &lpMsgProps);

    if (HR_FAILED(hResult) || lpMsgProps[2].ulPropTag != PR_SENDER_ENTRYID)
        goto ret;

    /*  Now, we open the PR_SENDER_ENTRYID and get an IMailUser object. */
    /*  It is from here that we get the Email Address and Address Type  */
    /*  of the person we are sending this OOF to.                       */

    hResult = lpsh->lpSession->lpVtbl->OpenEntry (lpsh->lpSession,
        lpMsgProps[2].Value.bin.cb, (LPENTRYID) lpMsgProps[2].Value.bin.lpb,
        NULL, 0, &ulObjType, (LPUNKNOWN *) &lpUser);

    if (hResult)
        goto ret;

    hResult = lpUser->lpVtbl->GetProps (lpUser,
        (LPSPropTagArray) &SPTA_AdrProps, 0, /* ansi */
        &cVals, &lpAdrProps);

    if (hResult)
        goto ret;

    /*  Create the core message that we will be sending.  In a second */
    /*  we'll populate this message with important properties.        */

    hResult = lpFolder->lpVtbl->CreateMessage (lpFolder, NULL, 0, &lpMsg);

    if (hResult || !lpMsg)
        goto ret;

    /*  Build our new Subject */

    cb = lstrlen(szOOF) + 1;
    cb += (lpMsgProps[0].Value.LPSZ ? lstrlen(lpMsgProps[0].Value.LPSZ) : 0);
    cb *= sizeof(TCHAR);

    sc = lpsh->lpfAllocateBuffer(cb, (LPVOID FAR *) &lpszOOFSubject);

    if (sc != SUCCESS_SUCCESS)
    {
        hResult = ResultFromScode (sc);
        goto ret;
    }

    lstrcpy(lpszOOFSubject, szOOF);
    if (lpMsgProps[0].Value.LPSZ)
        lstrcat(lpszOOFSubject, lpMsgProps[0].Value.LPSZ);

    /*  Create a PropVal array of Subject & Body; add them to the message */

    rgMsgProps[0].ulPropTag = PR_SUBJECT;
    rgMsgProps[0].Value.LPSZ = lpszOOFSubject;
    rgMsgProps[1].ulPropTag = PR_BODY;
    rgMsgProps[1].Value.LPSZ = lpszOOFBody;
    rgMsgProps[2].ulPropTag = PR_DELETE_AFTER_SUBMIT;
    rgMsgProps[2].Value.ul = TRUE;

    hResult = lpMsg->lpVtbl->SetProps (lpMsg, 3, rgMsgProps, 0);

    if (HR_FAILED (hResult))
        goto ret;

    /*  Now we build the ADRLIST that contains our one recipient.    */
    /*  We pass this ADRLIST to ModifyRecipients() on the message    */
    /*  and then commit this change with another SaveChanges() call. */

    sc = (*(lpsh->lpfAllocateBuffer)) (5 * sizeof (SPropValue), &rgAdrProps);

    if (sc != SUCCESS_SUCCESS)
    {
        hResult = ResultFromScode (sc);
        goto ret;
    }

    rgAdrProps[0].ulPropTag = PR_DISPLAY_NAME;
    rgAdrProps[0].Value.LPSZ = lpMsgProps[1].Value.LPSZ;
    rgAdrProps[1].ulPropTag = PR_ADDRTYPE;
    rgAdrProps[1].Value.LPSZ = lpAdrProps[0].Value.LPSZ;
    rgAdrProps[2].ulPropTag = PR_ENTRYID;
    rgAdrProps[2].Value.bin.cb = lpMsgProps[2].Value.bin.cb;
    rgAdrProps[2].Value.bin.lpb = lpMsgProps[2].Value.bin.lpb;
    rgAdrProps[3].ulPropTag = PR_EMAIL_ADDRESS;
    rgAdrProps[3].Value.LPSZ = lpAdrProps[1].Value.LPSZ;
    rgAdrProps[4].ulPropTag = PR_RECIPIENT_TYPE;
    rgAdrProps[4].Value.l = MAPI_TO;

    sc = (*(lpsh->lpfAllocateBuffer)) (CbNewADRLIST (1), &lpMods);

    if (sc != SUCCESS_SUCCESS)
    {
        hResult = ResultFromScode (sc);
        goto ret;
    }

    lpMods->cEntries = 1;
    lpMods->aEntries[0].cValues = 5;
    lpMods->aEntries[0].rgPropVals = rgAdrProps;

    /*  Add the AdrList */

    hResult = lpMsg->lpVtbl->ModifyRecipients (lpMsg, MODRECIP_ADD, lpMods);

    if (HR_FAILED (hResult))
        goto ret;

    /*  Commit the change; keep open so we can SubmitMessage() on it! */

    hResult = lpMsg->lpVtbl->SaveChanges (lpMsg, KEEP_OPEN_READWRITE);

ret:
    lpsh->lpfFreeBuffer(lpMsgProps);
    lpsh->lpfFreeBuffer(lpAdrProps);
    lpsh->lpfFreeBuffer(lpMods->aEntries[0].rgPropVals);
    lpsh->lpfFreeBuffer(lpMods);
    lpsh->lpfFreeBuffer(lpszOOFSubject);

    if (lpUser)
        lpUser->lpVtbl->Release (lpUser);

    /*  If an error occurred, we'll bail without a message.   */
    /*  Else, if all went well, we'll pass this message back. */

    if (hResult)
        if (lpMsg)
            lpMsg->lpVtbl->Release (lpMsg);
    else
        *lppMsg = lpMsg;

    return hResult;
}


/*
 -  ISpoolerHook::
 -  QueryInterface
 *
 *  Purpose:
 *      Validate our parameters and then AddRef the current object and 
 *      pass a copy back to the caller.
 */

STDMETHODIMP
SH_QueryInterface (LPSH lpsh, REFIID lpiid, LPVOID FAR * lppv)
{
    if ((IsBadWritePtr(lpsh, sizeof(SH))) ||
        (lpsh->ulcRef == 0) ||
        (IsBadReadPtr(lpiid, sizeof(IID))) ||
        (IsBadWritePtr(lppv, sizeof(LPSH))))
    {
        return ResultFromScode(E_INVALIDARG);
    }

    /*  See if the requested interface is one we support */

    if (memcmp(lpiid, &IID_ISpoolerHook, sizeof(IID)))
    {
        *lppv = NULL;
        return ResultFromScode(E_NOINTERFACE);
    }

    lpsh->ulcRef++;

    *lppv = (LPVOID) lpsh;

    return hrSuccess;
}


/*
 -  ISpoolerHook::
 -  AddRef
 *
 *  Purpose:
 *      Increment reference count.
 *
 */

STDMETHODIMP_ (ULONG)
SH_AddRef (LPSH lpsh)
{
    return ++lpsh->ulcRef;
}


/*
 -  ISpoolerHook::
 -  Release
 -
 *  Purpose:
 *  Decrement ulcRef. If it's zero, free the object.
 *
 */

STDMETHODIMP_ (ULONG)
SH_Release (LPSH lpsh)
{
    ULONG cRef;

    cRef = (--lpsh->ulcRef);

    if (cRef == 0)
    {
        /* Release the session object */

        lpsh->lpSession->lpVtbl->Release (lpsh->lpSession);

        lpsh->lpVtbl = NULL;
        (*(lpsh->lpfFreeBuffer)) (lpsh);
    }

    return cRef;
}
