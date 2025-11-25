////////////////////////////////////////////////////////////////////////////
//  SENDRTF.CPP
//
//  Command line mail program to send rtf files in a message body.
//
//  Copyright 1986-1996, Microsoft Corporation. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <mapiutil.h>
#include <mapix.h>
#include <mapidbg.h>
#include <cindex.h>

#ifdef DEBUG
#define TraceFnResult(f, hr)    \
{ (hr) ?   \
printf(#f " returns 0x%08lX %s\n", GetScode(hr), SzDecodeScode(GetScode(hr))) : 0;\
}
#else
#define TraceFnResult(f, hr)
#endif  //DEBUG

// Globals
LPSTR szProfile = NULL;     //profile name
LPSTR szPassword = NULL;    //profile password
LPSTR szSubject = NULL;     //message subject
LPSTR szRTFFile = NULL;     //rtf file name
LPSTR szToRecips = NULL;    //recipients

BOOL fMAPIInited = FALSE;

LPMAPISESSION pses = NULL;              //MAPI session
LPADRBOOK pabAddrB = NULL;              //Address Book
LPMDB pmdb = NULL;                      //default message store           
LPMAPIFOLDER pfldOutBox = NULL;         // Out folder
LPSPropValue pvalSentMailEID = NULL;    //Entry ID of the "Sent Mail" folder


//Function Prototypes
BOOL fParseCommandLine(int argc, char * argv[]);
HRESULT InitMAPI(void);
void DeInitMAPI(void);
HRESULT HrOpenDefaultStore(LPMDB * ppmdb);
HRESULT HrOpenAddressBook(LPADRBOOK * ppAdrBook);
HRESULT HrOpenOutFolder(LPMAPIFOLDER FAR * lppF);
HRESULT HrCreateOutMessage(LPMESSAGE FAR * ppmM);
HRESULT HrInitMsg(LPMESSAGE pmsg);
HRESULT HrCreateAndSend(void);
HRESULT HrCreateAddrList(LPADRLIST * ppal);
HRESULT HrSetRTF(LPMESSAGE pmsg);



int main(int argc, char * argv[])
{
    HRESULT hr;

    if(!fParseCommandLine(argc, argv))
        return 1;
    
    hr = InitMAPI();
    if(HR_FAILED(hr))
        return 1;
    
    hr = HrCreateAndSend();
        
    DeInitMAPI();

    return 0;
}


//
//  Init MAPI. Open address book, default message store,  outbox 
//
HRESULT InitMAPI(void)
{

    HRESULT hr;
    ULONG ulLogonFlag = 0;

    hr = MAPIInitialize(NULL);
    if(hr)
    {
        return hr;
    }
    
    fMAPIInited = TRUE;

    ulLogonFlag = szProfile ? 0 : MAPI_USE_DEFAULT;

    hr = MAPILogonEx(NULL, szProfile, szPassword,
                    MAPI_EXTENDED | MAPI_NEW_SESSION | ulLogonFlag,
                    &pses);
    if(hr)
    {
        goto err;
    }

    hr = HrOpenDefaultStore(&pmdb);
    if(HR_FAILED(hr))
        goto err;

    hr = HrOpenAddressBook(&pabAddrB);
    if(HR_FAILED(hr))
        goto err;

    hr = HrOpenOutFolder(&pfldOutBox);
    if(HR_FAILED(hr))
        goto err;
    
    /* retrieve the EntryID of the sentmail folder and change the property tag
        so that it is ready to use on a message*/
    hr = HrGetOneProp((LPMAPIPROP)pmdb, PR_IPM_SENTMAIL_ENTRYID, &pvalSentMailEID);
    if(hr)
    {
        goto err;
    }
    pvalSentMailEID->ulPropTag = PR_SENTMAIL_ENTRYID;
    
    return hrSuccess;

err:
    DeInitMAPI();
    
    DebugTraceResult(InitMAPI, hr);

    return hr;
}

//
//  Release MAPI interfaces and logoff
//
void DeInitMAPI(void)
{

    UlRelease(pfldOutBox);
    pfldOutBox = NULL;

    if(pmdb)
    {   
        //get our message out of the outbox
        ULONG ulFlags = LOGOFF_PURGE;
        HRESULT hr;
        
        hr = pmdb->StoreLogoff(&ulFlags);

        TraceFnResult(StoreLogoff, hr);
#ifdef DEBUG
        printf("StoreLogoff output falgs: %lx\n", ulFlags);
#endif
        UlRelease(pmdb);
        pmdb = NULL;
    }

    UlRelease(pabAddrB);
    pabAddrB = NULL;

    MAPIFreeBuffer(pvalSentMailEID);
    pvalSentMailEID = NULL;

    if(pses)
    {
        pses->Logoff(0, 0, 0);
        UlRelease(pses);
        pses = NULL;
    }

    if(fMAPIInited)
    {
        MAPIUninitialize();
        fMAPIInited = FALSE;
    }

}

//
//  Open the default message store. (The one that has PR_DEFAULT_STORE set to
//  TRUE in the message store table.
//
HRESULT HrOpenDefaultStore(LPMDB * ppmdb)
{
    HRESULT hr;
    LPMDB lpmdb = NULL;
    LPMAPITABLE ptable = NULL;
    LPSRowSet prows = NULL;
    LPSPropValue pvalProp = NULL;
    static SizedSPropTagArray(2, columns) =
                { 2, { PR_DEFAULT_STORE, PR_ENTRYID} };
    SPropValue valDefStore;
    SRestriction restDefStore;

    
    valDefStore.ulPropTag = PR_DEFAULT_STORE;
    valDefStore.dwAlignPad = 0;
    valDefStore.Value.b = TRUE;

    restDefStore.rt = RES_PROPERTY;
    restDefStore.res.resProperty.relop = RELOP_EQ;
    restDefStore.res.resProperty.ulPropTag = PR_DEFAULT_STORE;
    restDefStore.res.resProperty.lpProp = &valDefStore;
    
    Assert(pses);

    hr = pses->GetMsgStoresTable(0, &ptable);
    if (HR_FAILED(hr))
    {
        TraceFnResult(GetMsgStoresTable, hr);
        goto ret;
    }

    
    hr = HrQueryAllRows(ptable, (LPSPropTagArray) &columns, &restDefStore, NULL, 0, &prows);
    if (HR_FAILED(hr))
    {
        TraceFnResult(HrQyeryAllRows, hr);
        goto ret;
    }

    if (prows == NULL || prows->cRows == 0
        || prows->aRow[0].lpProps[1].ulPropTag != PR_ENTRYID)
    {
        printf("No default store\n");
        goto ret;
    }
    
    Assert(prows->cRows == 1);

    hr = pses->OpenMsgStore(0,
                        prows->aRow[0].lpProps[1].Value.bin.cb,
                        (LPENTRYID)prows->aRow[0].lpProps[1].Value.bin.lpb,
                        NULL, MDB_WRITE | MAPI_DEFERRED_ERRORS, &lpmdb);
    if (HR_FAILED(hr))
    {
        if (GetScode(hr) != MAPI_E_USER_CANCEL)
            TraceFnResult(OpenMsgStore, hr);
        goto ret;
    }

#if 0
    if(hr) /*if we have a warning, display it and succeed */
    {
        LPMAPIERROR perr = NULL;

        pses->lpVtbl->GetLastError(pses, hr, 0, &perr);
        MakeMessageBox(hWnd, GetScode(hr), IDS_OPENSTOREWARN, perr, MBS_ERROR);
        MAPIFreeBuffer(perr);
    }

#endif

    Assert(lpmdb != NULL);

    *ppmdb = lpmdb;


                    
ret:
    FreeProws(prows);
    UlRelease(ptable);

    DebugTraceResult(OpenDefaultStore, hr);
    return hr;
}

//
//  Open MAPI address book
//
HRESULT HrOpenAddressBook(LPADRBOOK * ppAddrBook)
{
    HRESULT hr;
    LPADRBOOK pabAddrBook = NULL;

    Assert(pses);
    
    hr = pses->OpenAddressBook(0, NULL, 0, &pabAddrBook);
    if(HR_FAILED(hr))
    {
        TraceFnResult(OpenAddressBook, hr);
        return hr;
    }
#if 0
    if(hr) /*if we have a warning*/
    {
        LPMAPIERROR perr = NULL;

        pses->lpVtbl->GetLastError(pses, hr, 0, &perr);
        MakeMessageBox(hwnd, GetScode(hr), IDS_OPENABWARN, perr, MBS_ERROR);
        MAPIFreeBuffer(perr);
    }
#endif

    *ppAddrBook = pabAddrBook;
    
    DebugTraceResult(HrOpenAddressBook, hr);
    return hrSuccess;
}

//
//  Open the outbox of the default store.
//  Assumes the default message store has been opened.
//
HRESULT HrOpenOutFolder(LPMAPIFOLDER FAR * lppF)
{
    LPMAPIFOLDER lpfOutF = NULL;
    HRESULT hr;
    LPSPropValue lpspvFEID = NULL;
    ULONG  ulObjType = 0;

    Assert(pmdb);

    *lppF = NULL;
    hr = HrGetOneProp((LPMAPIPROP) pmdb, PR_IPM_OUTBOX_ENTRYID, &lpspvFEID);
    if(hr)
    {
        TraceFnResult(HrGetOneProp, hr);
        goto err;
    }

    Assert(lpspvFEID->ulPropTag == PR_IPM_OUTBOX_ENTRYID);

    hr = pmdb->OpenEntry(lpspvFEID->Value.bin.cb,
                        (LPENTRYID)lpspvFEID->Value.bin.lpb, NULL,
                        MAPI_MODIFY | MAPI_DEFERRED_ERRORS,
                        &ulObjType, (LPUNKNOWN FAR *) &lpfOutF);
    if(HR_FAILED(hr))
    {
        TraceFnResult(OpenEntry, hr);
        goto err;
    }

    *lppF = lpfOutF;


err:
    MAPIFreeBuffer(lpspvFEID);

    DebugTraceResult(HrOpenOutFolder, hr);
    return hr;

}


//
//  Create an outbound message, put data in it and submit.
//
HRESULT HrCreateAndSend()
{
    HRESULT hr;
    LPMESSAGE pmsg = NULL;

    hr = HrCreateOutMessage(&pmsg);
    if(hr)
        return hr;

    hr = HrInitMsg(pmsg);
    if(HR_FAILED(hr))
        goto err;

    hr = pmsg->SubmitMessage(0);
    TraceFnResult(SubmitMessage, hr);

err:

    UlRelease(pmsg);

    DebugTraceResult(HrCreateAndSend, hr);
    return hr;

}

//
//  Create a message in the outbox
//
HRESULT HrCreateOutMessage(LPMESSAGE FAR * ppmM)
{

    LPMESSAGE lpmResM = NULL;
    HRESULT hr;

    Assert(pfldOutBox);

    hr = pfldOutBox->CreateMessage(NULL, MAPI_DEFERRED_ERRORS, &lpmResM);
    if(HR_FAILED(hr))
    {
        TraceFnResult(CreateMessage, hr);
        return hr;
    }

    *ppmM = lpmResM;

    DebugTraceResult(HrCreateOutMessage, hr);
    return hrSuccess;
}

//
//  Put the data from globals in the message
//
HRESULT HrInitMsg(LPMESSAGE pmsg)
{
    HRESULT hr;
    LPADRLIST pal = NULL;
    enum {iSubj, iSentMail, iConvTopic, iConvIdx, iMsgClass, cProps};
    // PR_SUBJECT, PR_SENTMAIL_ENTRYID,
    // PR_CONVERSATION_TOPIC, PR_COVERSATION_INDEX
    
    SPropValue props[cProps];
    ULONG cbConvIdx = 0;
    LPBYTE lpbConvIdx = NULL;
    
    //recipients
    hr = HrCreateAddrList(&pal);
    if(HR_FAILED(hr))
        goto err;
    
    Assert(pal);

    hr = pmsg->ModifyRecipients(0, pal);
    if(HR_FAILED(hr))
    {
        goto err;
    }

    //subject  and conversation topic
    if(szSubject)
    {
        props[iSubj].ulPropTag = PR_SUBJECT;
        props[iSubj].Value.LPSZ = szSubject;

        props[iConvTopic].ulPropTag = PR_CONVERSATION_TOPIC;
        props[iConvTopic].Value.LPSZ = szSubject;
    }
    else
    {
        props[iSubj].ulPropTag = PR_NULL;
        props[iConvTopic].ulPropTag = PR_NULL;
    }

    // conversaton index
    if(!ScAddConversationIndex(0, NULL, &cbConvIdx, &lpbConvIdx))
    {
        props[iConvIdx].ulPropTag = PR_CONVERSATION_INDEX;
        props[iConvIdx].Value.bin.lpb = lpbConvIdx;
        props[iConvIdx].Value.bin.cb = cbConvIdx;
    }
    else
    {
        props[iConvIdx].ulPropTag = PR_NULL;
    }

    // sent mail entry id (we want to leave a copy in the "Sent Items" folder)
    props[iSentMail] = *pvalSentMailEID;
    
    props[iMsgClass].ulPropTag = PR_MESSAGE_CLASS;
    props[iMsgClass].Value.lpszA = "IPM.Note";

    hr = pmsg->SetProps(cProps, (LPSPropValue)&props, NULL);
    if(HR_FAILED(hr))
    {
        TraceFnResult(SetProps, hr);
        goto err;
    }

    //body (PR_RTF_COMPRESSED)

    hr = HrSetRTF(pmsg);
    if(HR_FAILED(hr))
        goto err;

err:
    FreePadrlist(pal);
    MAPIFreeBuffer(lpbConvIdx);

    DebugTraceResult(HrInitMsg, hr);
    return hr;

}

//
//  Put the context of the input file in PR_RTF_COMPRESSED
//
HRESULT HrSetRTF(LPMESSAGE pmsg)
{
    if(!szRTFFile)
        return hrSuccess;

    Assert(pmsg);

    HRESULT hr;
    LPSTREAM lpstrRTFComp = NULL;
    LPSTREAM lpstrRTF = NULL;
    LPSTREAM lpstrRTFFile = NULL;
    STATSTG statstg;
    BOOL fUpdated;
    LPSPropValue pvalSupMask = NULL;

    hr = OpenStreamOnFile(MAPIAllocateBuffer, MAPIFreeBuffer,
                            STGM_READ | STGM_SHARE_DENY_WRITE,
                            szRTFFile,  NULL, &lpstrRTFFile);
    if(HR_FAILED(hr))
    {
        TraceFnResult(OpenStreamOnFile, hr);
        printf("Can't open file %s \n", szRTFFile);
        goto err;
    }

    hr = pmsg->OpenProperty (PR_RTF_COMPRESSED, &IID_IStream,
                        0, MAPI_CREATE | MAPI_MODIFY | MAPI_DEFERRED_ERRORS,
                            (LPUNKNOWN FAR *)&lpstrRTFComp);
    if(HR_FAILED(hr))
    {
        TraceFnResult(OpenProperty, hr);
        goto err;
    }

    hr = HrGetOneProp(pmsg, PR_STORE_SUPPORT_MASK, &pvalSupMask);
    if(HR_FAILED(hr))
    {
        TraceFnResult(HrGetOneProp, hr);
        goto err;
    }

    hr = WrapCompressedRTFStream(lpstrRTFComp,
                MAPI_MODIFY | (pvalSupMask->Value.l & STORE_UNCOMPRESSED_RTF),
                &lpstrRTF);
    if(HR_FAILED(hr))
    {
        TraceFnResult(WrapCompressedRTFStream, hr);
        goto err;
    }

    hr = lpstrRTFFile->Stat(&statstg, STATFLAG_NONAME);
    if(HR_FAILED(hr))
    {
        TraceFnResult(Stat, hr);
        goto err;
    }

    hr = lpstrRTFFile->CopyTo(lpstrRTF, statstg.cbSize, NULL, NULL);
    if(HR_FAILED(hr))
    {
        TraceFnResult(CopyTo, hr);
        goto err;
    }

    hr = lpstrRTF->Commit(STGC_OVERWRITE);
    if(HR_FAILED(hr))
    {
        TraceFnResult(Commit, hr);
        goto err;
    }

    if( !(pvalSupMask->Value.l & STORE_RTF_OK))
    {
#ifdef DEBUG
        printf("RTFSync will be called\n");
#endif //DEBUG
        
        hr = RTFSync(pmsg, RTF_SYNC_RTF_CHANGED, &fUpdated);
        if(HR_FAILED(hr))
        {
            TraceFnResult(RTFSync, hr);
            goto err;
        }
    }

err:

    UlRelease(lpstrRTF);
    UlRelease(lpstrRTFComp);
    UlRelease(lpstrRTFFile);

    MAPIFreeBuffer(pvalSupMask);

    DebugTraceResult(HrSetRTF, hr);

    return hr;
}

//
//  Create an adrlist with resolved recipients
//
HRESULT HrCreateAddrList(LPADRLIST * ppal)
{
    
    HRESULT hr;
    LPADRLIST pal = NULL;
    int ind;
    #define chDELIMITER ';'


    int nToRecips = 1;
    LPSTR strToken = szToRecips;
    while (strToken = strchr(strToken, chDELIMITER))
    {      
        ++strToken;
        ++nToRecips;
    } 

    int cb = CbNewADRLIST(nToRecips);

    hr = MAPIAllocateBuffer(cb, (LPVOID FAR *) &pal);
    if(hr) 
    {
        TraceFnResult(MAPIAllocateBuffer, hr);
        goto err;
    }
    ZeroMemory(pal, cb);

    hr = MAPIAllocateBuffer(2 * sizeof(SPropValue),
                            (LPVOID FAR *)&pal->aEntries[0].rgPropVals);
    if(hr) 
    {
        TraceFnResult(MAPIAllocateBuffer, hr);
        goto err;
    }
    
    pal->aEntries[0].rgPropVals[0].ulPropTag = PR_DISPLAY_NAME;
    pal->aEntries[0].rgPropVals[0].Value.LPSZ = szToRecips;
    pal->aEntries[0].rgPropVals[1].ulPropTag = PR_RECIPIENT_TYPE;
    pal->aEntries[0].rgPropVals[1].Value.l= MAPI_TO;
    pal->aEntries[0].cValues = 2;
    
    strToken = szToRecips;

    for(ind = 1; ind < nToRecips; ++ind)
    {
        LPADRENTRY pae = &pal->aEntries[ind];
        
        hr = MAPIAllocateBuffer(2 * sizeof(SPropValue),
                            (LPVOID FAR *)&pae->rgPropVals);
        if(hr) 
        {
            TraceFnResult(MAPIAllocateBuffer, hr);
            goto err;
        }

        strToken = strchr(strToken, chDELIMITER);
        Assert(strToken);

        *strToken++ = '\0';

        pae->rgPropVals[0].ulPropTag = PR_DISPLAY_NAME;
        pae->rgPropVals[0].Value.LPSZ = strToken;
        pae->rgPropVals[1].ulPropTag = PR_RECIPIENT_TYPE;
        pae->rgPropVals[1].Value.l = MAPI_TO;
        pae->cValues = 2;
    }
    
    pal->cEntries = nToRecips;
    hr = pabAddrB->ResolveName(0, 0, NULL, pal);
    if(HR_FAILED(hr))
    {
        TraceFnResult(ResolveName, hr);
        goto err;
    }

    *ppal = pal;

    return hrSuccess;

err:

    FreePadrlist(pal);

    DebugTraceResult(HrCreateAddrList, hr);
    return hr;
}
    

//======================================================================
//  ProcessCommandLineArguments - show usage to the user
//======================================================================

BOOL fParseCommandLine(int argc,char * argv[])
{
    BOOL fResult;
    int iCount;

    fResult = FALSE;
    _try
    {
      if (argc < 2)                                // Must be at least 1 argument
      {
         return fResult;                           // Show Usage to user.
      }
        
      for (iCount = 1; iCount < argc; iCount++)
      {
        if ((*argv[iCount] == '-') || (*argv[iCount] == '/'))
        {
            switch (tolower(*(argv[iCount]+1)))            // Process switches
            {
            case 'u':
                szProfile = argv[++iCount];
                break;
                
            case 'p':
                szPassword = argv[++iCount];
                break;

            case 't':
                szToRecips = argv[++iCount];
                break;

            case 's':
                szSubject = argv[++iCount];
                break;

            case 'f':
                szRTFFile = argv[++iCount];
                break;

            case 'h':
            case '?':                   // User asked for help '-?'
            default:
                return fResult;
            } // Switch
        }   // If 
      } // For Argument Count

      
      // Check for required command line arguments  

      if (szToRecips)
      {
         fResult = TRUE;
      } 
      else
      {
         printf("ERROR: Recipient(s) required.\n");

      }
      return fResult;

    }
    _finally 
    {
        if (!fResult)
        {
            printf( "\nSend MAPI messages from Win32 command line.\n" );
            printf( "Usage: sendrtf [-u] [-p] -t [-s] [-f][-v][-?]\n" );
            printf( "Where...\n\n" );
            printf( "-u  profile name \n" );
            printf( "-p  profile password \n" );
            printf( "-t  recipient(s) (multiples must be separated by ';' and "
                    "enclosed in double\n");
            printf( "    quotes. recipients must not be ambiguous in default "
                    "address book.)\n" );
            //printf( "-c  specifies mail copy list (cc: list) \n" );
            printf( "-s  subject line \n" );
            printf( "-f  file (in rtf format) to be used as a message body\n" );
            //printf( "-v  generates verbose output\n" );
            printf( "-?  prints this message\n" );
         }
    } // Finally
} // End of Function

