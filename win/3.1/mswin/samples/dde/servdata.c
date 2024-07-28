/****************************************************************************

    MODULE: SERVDATA.C

    PURPOSE: Maintains conversation information.


****************************************************************************/

#include "windows.h"

#include "dde.h"
#include "server.h"
#include <string.h>

typedef struct CONV
{
    HWND hwndServerDDE;
    HWND hwndClientDDE;
    BOOL bInClientRequestedTerminate;
};

typedef struct ADVISE
{
    HWND	hwndServerDDE;
    HWND	hwndClientDDE;
    int         nItem;
    ATOM        atomItem;
    BOOL        bAckRequest;
    BOOL        bDeferUpdate;
    BOOL        bAwaitingAck;
    HANDLE      hData;
};

struct CONV Conv[CONV_MAX_COUNT];
static int nConvCount = 0;
static struct ADVISE Advise[ADVISE_MAX_COUNT];
static int nAdviseCount = 0;

struct ADVISE * NEAR FindAdvise(HWND, int);
struct CONV *	NEAR FindConv(HWND);


/****************************************************************************

    FUNCTION: AddAdvise

    PURPOSE:  Register a new hot or warm link to a specified client, for
	      a specified server application item (1, 2, or 3).

****************************************************************************/
BOOL AddAdvise(hwndServerDDE, hDDEAdviseOptions, atomItem, nItem)
    HWND   hwndServerDDE;
    HANDLE hDDEAdviseOptions;
    ATOM   atomItem;
    int    nItem;
{
    struct ADVISE     * pAdvise;
    DDEADVISE FAR     * lpDDEAdviseOptions;
    int 		nAdviseIndex;

    if (nAdviseCount >= ADVISE_MAX_COUNT)
    {
	MessageBox(hwndMain,
            "Maximum advisories exceeded",
            "Server",
            MB_ICONEXCLAMATION | MB_OK);
        return (FALSE);
    }
    if ((lpDDEAdviseOptions
          = (DDEADVISE FAR *)GlobalLock(hDDEAdviseOptions)) == NULL)
        return (FALSE);

    for (pAdvise = Advise, nAdviseIndex = 0;
	 nAdviseIndex < nAdviseCount;
	 pAdvise++, nAdviseIndex++)
    {
	if (pAdvise->hwndServerDDE == hwndServerDDE
	    && pAdvise->nItem == nItem)
	{
	    MessageBox(hwndMain,
		"Advisory (paste link) already established",
		"Server",
		MB_ICONEXCLAMATION | MB_OK);
	    GlobalUnlock(hDDEAdviseOptions);
	    return (FALSE);
	}
    }

    pAdvise = Advise + nAdviseCount++;

    pAdvise->hwndServerDDE = hwndServerDDE;
    pAdvise->hwndClientDDE = GetHwndClientDDE(hwndServerDDE);
    pAdvise->nItem = nItem;
    pAdvise->atomItem = atomItem;
    pAdvise->bAckRequest = lpDDEAdviseOptions->fAckReq;
    pAdvise->bDeferUpdate = lpDDEAdviseOptions->fDeferUpd;
    pAdvise->bAwaitingAck = FALSE;

    GlobalUnlock(hDDEAdviseOptions);
    return (TRUE);
}



/****************************************************************************

    FUNCTION: AddConv

    PURPOSE:  Register a new conversation with a client window

****************************************************************************/
BOOL AddConv(hwndServerDDE, hwndClientDDE)
    HWND  hwndServerDDE;
    HWND  hwndClientDDE;
{
    struct CONV * pConv;

    if (nConvCount >= CONV_MAX_COUNT)
    {
        return (FALSE);
    }

    if (FindConv(hwndServerDDE) != NULL)
    {
	return (FALSE); /* conversation already added */
    }

    pConv = Conv + nConvCount++;
    pConv->hwndServerDDE = hwndServerDDE;
    pConv->hwndClientDDE = hwndClientDDE;
    pConv->bInClientRequestedTerminate = FALSE;
    return (TRUE);
}



/***********************************************************************

    FUNCTION:  AtLeastOneConvActive

    PURPOSE:   Used during termination of application, to
	       determine whether any conversations are still active
	       while the conversations are being terminated.

***********************************************************************/
BOOL AtLeastOneConvActive()
{
    return (nConvCount? TRUE: FALSE);
}



/****************************************************************************

    FUNCTION: CheckOutSentData

    PURPOSE:  Set Awaiting Ack state to true for specified item.

****************************************************************************/
void CheckOutSentData(hwndServerDDE, nItem, atomItem, hData)
    HWND    hwndServerDDE;
    int     nItem;
    ATOM    atomItem;
    HANDLE  hData;
{
    struct ADVISE * pAdvise;

    if (!(pAdvise = FindAdvise(hwndServerDDE, nItem)))
        return;
    pAdvise->bAwaitingAck = TRUE;
    pAdvise->atomItem = atomItem;
    pAdvise->hData = hData;
    return;
}



/****************************************************************************

    FUNCTION: DoEditCopy

    PURPOSE:  Copy selected data to clipboard in two formats:
	      CF_TEXT and (registered) "Link" format.

****************************************************************************/
void DoEditCopy(nItem)
    int   nItem;
{

    /* Format string and set clipboard data here */
    HANDLE  hTextData;
    HANDLE  hConvData;
    char  szItemValue[ITEM_VALUE_MAX_SIZE+1];
    LPSTR  lpTextData, lpConvData;

    if (!GetDlgItemText(hwndMain, nItem, szItemValue, ITEM_VALUE_MAX_SIZE))
	strcpy(szItemValue, " ");

    /* Copy item value in CF_TEXT format to global memory object */

    if (!(hTextData
	  = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, (DWORD)lstrlen(szItemValue)+1)))
	return;
    if (!(lpTextData = GlobalLock(hTextData)))
    {
	GlobalFree(hTextData);
	return;
    }
    lstrcpy(lpTextData, szItemValue);
    GlobalUnlock(hTextData);

    /* Copy item value in "Link" format to global memory object */

    if (!(hConvData
	  = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
	    (DWORD)APP_MAX_SIZE+TOPIC_MAX_SIZE+ITEM_NAME_MAX_SIZE+4)))
       return;
    if (!(lpConvData = GlobalLock(hConvData)))
    {
       GlobalFree(hConvData);
       return;
    }

    /* Compose paste link string, each sub-string is null terminated */
    /* Format is <app> <null> <topic> <null> <item> <null> <null>    */
    lstrcpy(lpConvData, "Server");
    lpConvData = lpConvData + lstrlen(lpConvData) + 1;	/* '+1' adds null */
    lstrcpy(lpConvData, szDocName);   /* topic */
    lpConvData += (lstrlen(lpConvData) + 1);
    lstrcpy(lpConvData, "item#");
    lpConvData[4] = (char)('1' + nItem - 1);
    *(lpConvData + lstrlen(lpConvData) + 1) = 0;  /* final null */

    GlobalUnlock(hConvData);

    if (OpenClipboard(hwndMain))
    {
       EmptyClipboard();

       SetClipboardData(CF_TEXT, hTextData);
       SetClipboardData(cfLink, hConvData);

       CloseClipboard();
    }
    else
    {
       GlobalFree(hTextData);
       GlobalFree(hConvData);
    }

    return;
}



/****************************************************************************

    FUNCTION: FindAdvise

    PURPOSE:  Find advisory data for a specific conversation item.

****************************************************************************/
struct ADVISE * NEAR FindAdvise(hwndServerDDE, nItem)
    HWND hwndServerDDE;
    int  nItem;
{
    struct ADVISE * pAdvise;
    int             nAdviseIndex;

    for (nAdviseIndex = 0, pAdvise = Advise;
         nAdviseIndex < nAdviseCount;
         nAdviseIndex++, pAdvise++)
    {
	if (pAdvise->hwndServerDDE == hwndServerDDE
            && pAdvise->nItem == nItem)
        {
            return (pAdvise);
        }
    }
    return (NULL);
}





/****************************************************************************

    FUNCTION: FindConv

    PURPOSE:  Find the conversation for a specified server DDE window.

****************************************************************************/
struct CONV * NEAR FindConv(hwndServerDDE)
    HWND  hwndServerDDE;
{
    struct CONV * pConv;
    int 	  nConvIndex;

    for (nConvIndex = 0, pConv = Conv;
	 nConvIndex < nConvCount;
	 nConvIndex++, pConv++)
    {
	if (pConv->hwndServerDDE == hwndServerDDE)
	    return (pConv);
    }
    return (NULL);
}



/****************************************************************************

    FUNCTION: GetAdviseData

    PURPOSE:  Get advisory data for a specified conversation item.

****************************************************************************/
BOOL GetAdviseData(hwndServerDDE, nItem, szItemName, szItemValue,
                   pbDeferUpdate, pbAckRequest)
    HWND   hwndServerDDE;
    int    nItem;
    char * szItemName;
    char * szItemValue;
    BOOL * pbDeferUpdate;
    BOOL * pbAckRequest;
{
    struct ADVISE * pAdvise;

    if (!(pAdvise = FindAdvise(hwndServerDDE, nItem)))
        return (FALSE);
    strcpy(szItemName, "Item#");
    szItemName[4] = (char)(pAdvise->nItem==1? '1': pAdvise->nItem==2? '2': '3');
    *pbDeferUpdate = pAdvise->bDeferUpdate;
    *pbAckRequest = pAdvise->bAckRequest;
    if (!GetDlgItemText(hwndMain, nItem, szItemValue, ITEM_VALUE_MAX_SIZE))
        strcpy(szItemValue," ");

    return (TRUE);
}



/****************************************************************************

    FUNCTION: GetHwndClientDDE

    PURPOSE:  Get the hwnd of the client in conversation with a specified
	      server DDE window.

****************************************************************************/
HWND GetHwndClientDDE(hwndServerDDE)
    HWND  hwndServerDDE;
{
    struct CONV * pConv;

    if (!(pConv = FindConv(hwndServerDDE)))
	return (NULL);
    return (pConv->hwndClientDDE);
}



/****************************************************************************

    FUNCTION: GetNextAdvise

    PURPOSE:  Find a client that needs to be notified that a value of
	      a specified item (a hot or warm link item) has changed.
	      Since a hot or warm link to the item may have been established
	      for multiple conversations (multiple clients), this function
	      is set up to step through the list of such conversations.
	      Start the list by passing a NULL hwndClientDDE.  To get the
	      next conversation in the list, pass the previous client returned
	      by this function.

****************************************************************************/
HWND GetNextAdvise(hwndServerDDE, nItem)
    HWND hwndServerDDE;
    int  nItem;
{
    struct ADVISE * pAdvise;
    int             nAdviseIndex;

    if (hwndServerDDE)
    {
        for (nAdviseIndex = 0, pAdvise = Advise;
             nAdviseIndex < nAdviseCount;
             nAdviseIndex++, pAdvise++)
        {
	    if (pAdvise->hwndServerDDE == hwndServerDDE
                && pAdvise->nItem == nItem)
            {
                pAdvise++;
                break;
            }
        }
        if (nAdviseIndex >= nAdviseCount)
            return (NULL);
    }
    else
    {
        pAdvise = Advise;
        nAdviseIndex = 0;
    }
    for ( ; nAdviseIndex < nAdviseCount; nAdviseIndex++, pAdvise++)
    {
        if (pAdvise->nItem == nItem)
        {
	    return (pAdvise->hwndServerDDE);
        }
    }
    return (NULL);
}



/****************************************************************************

    FUNCTION: GetNextConv

    PURPOSE:  Get next client in list of conversations.  To get the
	      first hwndServerDDE in the conversation list, pass in a NULL
	      value for hwndServerDDE.

****************************************************************************/
HWND GetNextConv(hwndServerDDE)
    HWND hwndServerDDE;
{
    struct CONV * pConv;
    int 	  nConvIndex;

    if (hwndServerDDE)
    {
	for (nConvIndex = 0, pConv = Conv;
	     nConvIndex < nConvCount;
	     nConvIndex++, pConv++)
        {
	    if (pConv->hwndServerDDE == hwndServerDDE)
            {
		if (++nConvIndex < nConvCount)
		    return (++pConv)->hwndServerDDE;
                else
                    return (NULL);
            }
        }
        return (NULL);
    }
    if (nConvCount > 0)
	return (Conv[0].hwndServerDDE);
    else
        return (NULL);
}



/****************************************************************************

    FUNCTION: GlobalFreeSentData

    PURPOSE:  A global memory object (hData) was sent in a WM_DDE_DATA
	      message, but the client never used it.  So, the server is
	      responsible for freeing the object.

****************************************************************************/
void GlobalFreeSentData(hwndServerDDE, nItem)
    HWND  hwndServerDDE;
    int   nItem;
{
    struct ADVISE * pAdvise;

    if (!(pAdvise = FindAdvise(hwndServerDDE, nItem)))
        return;
    pAdvise->bAwaitingAck = FALSE;
    GlobalDeleteAtom(pAdvise->atomItem);
    GlobalFree(pAdvise->hData);
    return;
}



/****************************************************************************

    FUNCTION: IsConvInTerminateState

    PURPOSE:  Terminate whether conversation with specified client is
	      in process of being terminated.

****************************************************************************/
BOOL IsConvInTerminateState(hwndServerDDE)
    HWND  hwndServerDDE;
{
    struct CONV * pConv;

    if (pConv = FindConv(hwndServerDDE))
	return (pConv->bInClientRequestedTerminate);
    else
        return (FALSE);
}


/****************************************************************************

    FUNCTION: RemoveAdvise

    PURPOSE:  Cancel a hot or warm link for a specified conversation item.
	      If a 0 value is specified for nItem, then all hot/warm links
              for the specified client are removed.

****************************************************************************/
BOOL RemoveAdvise(hwndServerDDE, nItem)
    HWND  hwndServerDDE;
    int   nItem;
{
    struct ADVISE * pAdvise;
    int             nAdviseIndex;
    int             nRemoveCount;

    nRemoveCount = 0;
    for (nAdviseIndex = 0, pAdvise = Advise;
         nAdviseIndex < nAdviseCount;
         nAdviseIndex++, pAdvise++)
    {
        if (nRemoveCount)
        {
            *(pAdvise-nRemoveCount) = *pAdvise;
        }
	if (pAdvise->hwndServerDDE == hwndServerDDE
            && (!nItem || pAdvise->nItem == nItem))
        {
            nRemoveCount++;
        }

    }
    if (nRemoveCount)
    {
        nAdviseCount -= nRemoveCount;
        return (TRUE);
    }
    else
    {
        return (FALSE);
    }
}



/****************************************************************************

    FUNCTION: RemoveConv

    PURPOSE:  Remove conversation from conversation list, and remove
	      all hot/warm links associated with that conversation
	      from the advisory list.

****************************************************************************/
void RemoveConv(hwndServerDDE)
    HWND hwndServerDDE;
{
    struct CONV   * pConv;
    int 	    nConvIndex;
    struct ADVISE * pAdvise;
    struct ADVISE * pAdviseShift;
    int             nAdviseIndex;
    int             nAdviseDecrement;

    for (nConvIndex = 0, pConv = Conv;
	 nConvIndex < nConvCount;
	 nConvIndex++, pConv++)
    {
	if (pConv->hwndServerDDE == hwndServerDDE)
            break;
    }
    nConvCount--;
    while (nConvIndex < nConvCount)
    {
	*pConv = *(pConv+1);
	nConvIndex++;
	pConv++;
    }

    /* Remove each hot/warm link */

    pAdviseShift = Advise;
    nAdviseDecrement = 0;
    for (nAdviseIndex = 0, pAdvise = Advise;
         nAdviseIndex < nAdviseCount;
         nAdviseIndex++, pAdvise++)
    {
	if (pAdvise->hwndServerDDE == hwndServerDDE)
        {
            nAdviseDecrement++;
            if (pAdvise->bAwaitingAck)
            { /* Destroy objects perhaps not destroyed by client */
		GlobalDeleteAtom(pAdvise->atomItem);
                GlobalFree(pAdvise->hData);
            }
        }
        else
        {
            *(pAdviseShift++) = *pAdvise;
        }
    }
    nAdviseCount -= nAdviseDecrement;
    return;
}



/****************************************************************************

    FUNCTION: SetConvInTerminateState

    PURPOSE:  Set conversations's terminate state to TRUE.

****************************************************************************/
void SetConvInTerminateState(hwndServerDDE)
    HWND  hwndServerDDE;
{
    struct CONV * pConv;

    if (pConv = FindConv(hwndServerDDE))
	pConv->bInClientRequestedTerminate = TRUE;
    return;
}
