// ConnMgmt.cpp
//
// Copyright (c)1996 Microsoft Corporation, All Right Reserved
//
// Module Description:
//
// This module includes functions which perform client connection 
// context management.  Double linked list coupled with the 
// CRITICAL_SECTION object is used to implement very simple client
// context connection management. The CRITICAL_SECTION object is required 
// to make these functions thread safe.  While one can make this 
// list more feature rich and easier to use, it gets a point across.
//
// Written by: ZorG			1/12/96
//

#include <windows.h>
#include "Echo.h"

static LPCONNCONTEXT	s_lpConnectedClientsList;

extern CRITICAL_SECTION g_csClientList;



LPCONNCONTEXT AddConnectionContext(EXTENSION_CONTROL_BLOCK *pEcb, SOCKET s)

/*++

Routine Description:

    This routine creates and adds Client Context to the list of active 
	client connections

Arguments:

    pEcb		- pointer to the Extension block. I currently use only
				ConnID in the Client Context structure.  

	s			- pointer to the socket connected to a server
Return Value:

    LPCONNCONTEXT	- a pointer.to a newly created ConnContext

--*/

{
LPCONNCONTEXT	lpConnContext;

	EnterCriticalSection(&g_csClientList);
	lpConnContext = (LPCONNCONTEXT)LocalAlloc( 0, sizeof(*lpConnContext) );
	if (lpConnContext == NULL)
		goto end;
	// Initialize Client Context
	lpConnContext->pEcb				= pEcb;
	lpConnContext->Socket			= s;
	lpConnContext->dwTimeAccessed	= GetTickCount();
	lpConnContext->Next = NULL;
	lpConnContext->Prev = NULL;
	if (NULL ==s_lpConnectedClientsList) {
		// First element on the list
		s_lpConnectedClientsList = lpConnContext;
	}
	else {
		// Put new element on a top of the list
		lpConnContext->Next				= s_lpConnectedClientsList;
		s_lpConnectedClientsList->Prev	= lpConnContext;
		s_lpConnectedClientsList		= lpConnContext;
	}	
end:
	LeaveCriticalSection(&g_csClientList);
	return lpConnContext;
}



void	RemoveConnectionContext(LPCONNCONTEXT lpConnContext)

/*++

Routine Description:

    This routine removes Client Context from the list of active 
	client connections

Arguments:

    LPCONNCONTEXT	- a pointer.to the Client Context to be removed from the list

Return Value:

    void
--*/

{
LPCONNCONTEXT	lpTempCntx;

	EnterCriticalSection(&g_csClientList);

	lpTempCntx = s_lpConnectedClientsList;

	while(lpTempCntx) {
		if ( lpConnContext != lpTempCntx)
			lpTempCntx = lpTempCntx->Next;
		else {

			if (lpConnContext->Prev == NULL) {
				// removing from the top of the list
				s_lpConnectedClientsList = s_lpConnectedClientsList->Next;
			}
			else if (lpConnContext->Next == NULL) {
				// removing from the bottom of the list
				lpTempCntx->Prev->Next = NULL;
			}
			else {
				// removing from the middle of the list
				lpConnContext->Prev->Next = lpConnContext->Next;
				lpConnContext->Next->Prev = lpConnContext->Prev;
			}

			LocalFree(lpConnContext);
			break;
		}
	}

	LeaveCriticalSection(&g_csClientList);
}



BOOL FindConnectionContext(LPCONNCONTEXT lpConnContext, EXTENSION_CONTROL_BLOCK *pNewEcb)

/*++

Routine Description:

    This routine searches list	of active client connections

Arguments:

 lpConnContext - a pointer to a ConnContext to search for
 pNewEcb	- a pointer to new EXTENSION_CONTROL_BLOCK. We use to 
			  set lpClient pEcb data member if a connection has 
			  already been establsihed by the client
Return Value:

  BOOL		- TRUE if match is found, FALSE otherwise
   
--*/

{
LPCONNCONTEXT	lpTempCntx;
BOOL		bFound = FALSE;

	EnterCriticalSection(&g_csClientList);

	lpTempCntx = s_lpConnectedClientsList;

	while(lpTempCntx) {
		if ( lpConnContext != lpTempCntx)
			lpTempCntx = lpTempCntx->Next;
		else {
			// If active client os found
			// then save new pointer to Extension 
			// block and update a time stamp
			lpConnContext->pEcb				= pNewEcb;
			lpConnContext->dwTimeAccessed	= GetTickCount();
			bFound = TRUE;
			goto end;
		}
	}
end:
	LeaveCriticalSection(&g_csClientList);
	return bFound;
}


void	RemoveInactiveConnections(void)

/*++

Routine Description:

    This routine removes Inactive Clientsthe list of active 
	client connections

Arguments:

	void

Return Value:

    void
--*/

{
DWORD			dwCurrentTick;
LPCONNCONTEXT	lpConnContext, lpTempCntx;

	dwCurrentTick = GetTickCount();

	EnterCriticalSection(&g_csClientList);

	lpTempCntx = s_lpConnectedClientsList;

	while(lpTempCntx) {
		// Remove the connection
		// It will automatically expire cookie
		// and the new one will be assigned

		if (dwCurrentTick - lpTempCntx->dwTimeAccessed >= INACTIVE_CONNECTION) {

			closesocket(lpTempCntx->Socket);

			lpConnContext = lpTempCntx; 

			if (lpConnContext->Prev == NULL) {
				// removing from the top of the list
				lpTempCntx = s_lpConnectedClientsList = s_lpConnectedClientsList->Next;
			}
			else if (lpConnContext->Next == NULL) {
				// removing from the bottom of the list
				lpTempCntx->Prev->Next = NULL;
				lpTempCntx = NULL;
			}
			else {
				// removing from the middle of the list
				lpConnContext->Prev->Next = lpConnContext->Next;
				lpConnContext->Next->Prev = lpConnContext->Prev;
				lpTempCntx = lpTempCntx->Next;
			}

			LocalFree(lpConnContext);
		}
		else
			lpTempCntx = lpTempCntx->Next;
	}

	LeaveCriticalSection(&g_csClientList);

	return;
}