/*++

     Copyright (c) 1996 Intel Corporation
     Copyright (c) 1996 Microsoft Corporation
     All Rights Reserved
     
     Permission is granted to use, copy and distribute this software and 
     its documentation for any purpose and without fee, provided, that 
     the above copyright notice and this statement appear in all copies. 
     Intel makes no representations about the suitability of this 
     software for any purpose.  This software is provided "AS IS."  
     
     Intel specifically disclaims all warranties, express or implied, 
     and all liability, including consequential and other indirect 
     damages, for the use of this software, including liability for 
     infringement of any proprietary rights, and including the 
     warranties of merchantability and fitness for a particular purpose. 
     Intel does not assume any responsibility for any errors which may 
     appear in this software nor any responsibility to update it.

Module Name:

    SPI.CPP : 

Abstract:

    Contains all the entry points for the WS2SPI.  This module implements a
    sample WinSock2 layered service provider.
    
Author:

        
Revision History:

   
--*/

#include "precomp.h"


// The WinSock2 UpCallTable.
WSPUPCALLTABLE gUpCallTable;

// Variables to track Startup/Cleanup Pairing.
CRITICAL_SECTION  gInitCriticalSection;
DWORD             gStartupCount=0;

// Variables to keep track of the sockets we have open
LIST_ENTRY         gSocketList;
CRITICAL_SECTION  gSocketListLock;

// The catalog of providers
PDCATALOG gProviderCatalog;

// The worker thread for async and overlapped functions
PDWORKERTHREAD gWorkerThread;

// The buffer manager for providers that modify the data stream
PDBUFFERMANAGER gBufferManager;

#if defined(DEBUG_TRACING)
char    gLibraryName[MAX_PATH] = "LSP.DLL";
#endif
    
SOCKET
WSPAPI
WSPAccept(
    IN SOCKET s,
    OUT struct sockaddr FAR *addr,
    OUT INT FAR *addrlen,
    IN LPCONDITIONPROC lpfnCondition,
    IN DWORD dwCallbackData,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Conditionally  accept a connection based on the return value of a condition
    function, and optionally create and/or join a socket group.

Arguments:

    s              - A  descriptor  identiying  a socket which is listening for
                     connections after a WSPListen().

    addr           - An optional pointer to a buffer which receives the address
                     of   the  connecting  entity,  as  known  to  the  service
                     provider.   The  exact  format  of  the  addr arguement is
                     determined  by  the  address  family  established when the
                     socket was created.

    addrlen        - An  optional  pointer  to  an  integer  which contains the
                     length of the address addr.

    lpfnCondition  - The  procedure  instance address of an optional, WinSock 2
                     client  supplied  condition  function  which  will make an
                     accept/reject  decision  based  on  the caller information
                     passed  in  as  parameters,  and optionally creaetd and/or
                     join  a  socket group by assigning an appropriate value to
                     the result parameter of this function.

    dwCallbackData - Callback data to be passed back to the WinSock 2 client as
                     a  condition  function  parameter.   This parameter is not
                     interpreted by the service provider.

    lpErrno        - A pointer to the error code.

Return Value:

    If  no  error occurs, WSPAccept() returns a value of type SOCKET which is a
    descriptor  for  the accepted socket.  Otherwise, a value of INVALID_SOCKET
    is returned, and a specific error code is available in lpErrno.

--*/
{
    INT          ReturnValue;
    PDPROVIDER   Provider;
    PDSOCKET     Socket;
    PDSOCKET     NewSocket;
    SOCKET       ProviderSocket;
    SOCKET       NewProviderSocket;
    
    
    // Debug/Trace stuff
    
    if (PREAPINOTIFY(( DTCODE_WSPAccept,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &addr,
                       &addrlen,
                       &lpfnCondition,
                       &dwCallbackData,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);

    if (SOCKET_ERROR != ReturnValue){

        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        NewProviderSocket = Provider->WSPAccept(
            ProviderSocket,
            addr,
            addrlen,
            lpfnCondition,
            dwCallbackData,
            lpErrno);
        if (INVALID_SOCKET != NewProviderSocket){

            //
            // Create a new socket object and initialize it.
            NewSocket = new DSOCKET;
            if (NewSocket){
                
                ReturnValue = gUpCallTable.lpWPUCreateSocketHandle(
                    Socket->GetCatalogEntryId(),
                    (DWORD) NewSocket,
                    lpErrno);
                DEBUGF( DBG_TRACE,
                        ("Accept Returning Socket %X\n", ReturnValue));
                  
                if (INVALID_SOCKET != ReturnValue){
                    NewSocket->Initialize(
                        Provider,
                        NewProviderSocket,
                        Socket->GetCatalogEntryId(),
                        ReturnValue);

                    // Add this socket to the list of sockets.
                    EnterCriticalSection(&gSocketListLock);
                    InsertHeadList(
                        &gSocketList,
                        &NewSocket->m_list_linkage);
                    LeaveCriticalSection(&gSocketListLock);
                } //if
                else{
                    delete NewSocket;
                } //else
            } //if
        } //if
    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPAccept,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &addr,
                    &addrlen,
                    &lpfnCondition,
                    &dwCallbackData,
                    &lpErrno));
   
    return(ReturnValue);
}




INT
WSPAPI
WSPAddressToString(
    IN     LPSOCKADDR lpsaAddress,
    IN     DWORD dwAddressLength,
    IN     LPWSAPROTOCOL_INFOW lpProtocolInfo,
    OUT    LPWSTR lpszAddressString,
    IN OUT LPDWORD lpdwAddressStringLength,
    OUT    LPINT lpErrno )
/*++

Routine Description:

    WSPAddressToString() converts a SOCKADDR structure into a human-readable
    string representation of the address.  This is intended to be used mainly
    for display purposes. If the caller wishes the translation to be done by a
    particular provider, it should supply the corresponding WSAPROTOCOL_INFO
    struct in the lpProtocolInfo parameter.

Arguments:

    lpsaAddress - points to a SOCKADDR structure to translate into a string.

    dwAddressLength - the length of the Address SOCKADDR.

    lpProtocolInfo - (optional) the WSAPROTOCOL_INFO struct for a particular
                     provider. 

    lpszAddressString - a buffer which receives the human-readable address
                        string. 

    lpdwAddressStringLength - on input, the length of the AddressString buffer.
                              On output, returns the length of  the string
                              actually copied into the buffer.

Return Value:

    The return value is 0 if the operation was successful.  Otherwise the value
    SOCKET_ERROR is returned 
--*/
{
    INT                 ReturnValue;
    DWORD               NextProviderCatalogEntryId;
    DWORD               ThisProviderCatalogEntryId;
    PPROTO_CATALOG_ITEM CatalogItem;
    PDPROVIDER          Provider;

    if (PREAPINOTIFY(( DTCODE_WSPAddressToString,
                       &ReturnValue,
                       gLibraryName,
                       &lpsaAddress,
                       &dwAddressLength,
                       &lpProtocolInfo,
                       &lpszAddressString,
                       &lpdwAddressStringLength,
                       &lpErrno)) ){
        return(ReturnValue);
    } //if

    //
    // Get the catlog entry for the next provider in the chain
    //
    ReturnValue = gProviderCatalog->FindNextProviderInChain(
        lpProtocolInfo,
        &ThisProviderCatalogEntryId,
        &NextProviderCatalogEntryId);

    if (NO_ERROR == ReturnValue){

        //
        // Get the provider for the catlog entry
        //
        ReturnValue = gProviderCatalog->GetCatalogItemFromCatalogEntryId(
            NextProviderCatalogEntryId,
            &CatalogItem);
        if (NO_ERROR == ReturnValue){

            //
            // Get the provider for the catlog entry
            //
            ReturnValue = gProviderCatalog->GetCatalogItemFromCatalogEntryId(
                NextProviderCatalogEntryId,
                &CatalogItem);
            if (NO_ERROR == ReturnValue){

                //
                // The the next providers WSPSocket
                //
                Provider = CatalogItem->GetProvider();
                ReturnValue = Provider->WSPAddressToString(
                    lpsaAddress,
                    dwAddressLength,
                    lpProtocolInfo,
                    lpszAddressString,
                    lpdwAddressStringLength,
                    lpErrno);
            } //if
        } //if
    } //if

    POSTAPINOTIFY(( DTCODE_WSPAddressToString,
                    &ReturnValue,
                    gLibraryName,
                    &lpsaAddress,
                    &dwAddressLength,
                    &lpProtocolInfo,
                    &lpszAddressString,
                    &lpdwAddressStringLength,
                    &lpErrno));

    return(ReturnValue);
}


 


INT
WSPAPI
WSPAsyncSelect(
    IN SOCKET s,
    IN HWND hWnd,
    IN unsigned int wMsg,
    IN long lEvent,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Request  Windows  message-based  event notification of network events for a
    socket.

Arguments:

    s       - A  descriptor identiying a socket for which event notification is
              required.

    hWnd    - A  handle  identifying  the window which should receive a message
              when a network event occurs.

    wMsg    - The message to be sent when a network event occurs.

    lEvent  - bitmask  which specifies a combination of network events in which
              the WinSock client is interested.

    lpErrno - A pointer to the error code.

Return Value:

    The  return  value  is 0 if the WinSock client's declaration of interest in
    the  netowrk event set was successful.  Otherwise the value SOCKET_ERROR is
    returned, and a specific error code is available in lpErrno.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    
    if (PREAPINOTIFY(( DTCODE_WSPAsyncSelect,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &hWnd,
                       &wMsg,
                       &lEvent,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        
        *lpErrno = NO_ERROR;
        *lpErrno = Socket->RegisterAsyncOperation(
            hWnd,
            wMsg,
            lEvent);
        if (NO_ERROR == *lpErrno){
            ReturnValue = NO_ERROR;
        } //if
        else{
            ReturnValue = SOCKET_ERROR;
        } //else
    } //if
 
    POSTAPINOTIFY(( DTCODE_WSPAsyncSelect,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &hWnd,
                    &wMsg,
                    &lEvent,
                    &lpErrno));

    return(ReturnValue);
}



INT
WSPAPI
WSPBind(
    IN SOCKET s,
    IN const struct sockaddr FAR *name,
    IN INT namelen,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Associate a local address (i.e. name) with a socket.

Arguments:

    s       - A descriptor identifying an unbound socket.

    name    - The  address  to assign to the socket.  The sockaddr structure is
              defined as follows:

              struct sockaddr {
                  u_short sa_family;
                  char    sa_data[14];
              };

              Except  for  the sa_family field,
sockaddr contents are epxressed
              in network byte order.

    namelen - The length of the name.

    lpErrno - A pointer to the error code.

Return Value:

    If   no   erro   occurs,  WSPBind()  returns  0.   Otherwise, it  returns
    SOCKET_ERROR, and a specific error code is available in lpErrno.

--*/
{
    INT ReturnValue;
    PDPROVIDER   Provider;
    PDSOCKET     Socket;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPBind,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &name,
                       &namelen,
                       &lpErrno)) ) {

        return(ReturnValue);
    }
    
    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);

    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        ReturnValue = Provider->WSPBind(
            ProviderSocket,
            name,
            namelen,
            lpErrno);

        
        POSTAPINOTIFY(( DTCODE_WSPBind,
                        &ReturnValue,
                        gLibraryName,
                        &s,
                        &name,
                        &namelen,
                        &lpErrno));
    }
    
    return(ReturnValue);

}



INT
WSPAPI
WSPCancelBlockingCall(OUT INT FAR *lpErrno)
/*++
Routine Description:

    Cancel a blocking call which is currently in progress.

Arguments:

    lpErrno - A pointer to the error code.

Return Value:

    The  value  returned  by  WSPCancelBlockingCall() is 0 if the operation was
    successfully canceled.  Otherwise the value SOCKET_ERROR is returned,
and a
    specific error code is available in lpErrno.

--*/
{
 //     INT ReturnValue;

//      if (PREAPINOTIFY(( DTCODE_WSPCancelBlockingCall,
//                         &ReturnValue,
//                         gLibraryName,
//                         &lpErrno)) ) {
//          return(ReturnValue);
//      }

//      ReturnValue = gProcTable.lpWSPCancelBlockingCall(
//          lpErrno);

//      POSTAPINOTIFY(( DTCODE_WSPCancelBlockingCall,
//                      &ReturnValue,
//                      gLibraryName,
//                      &lpErrno))
//      return(ReturnValue);
    // ****
    // Note:
    // We are failing this call right now since I am lobbing for a spec change
    // so we dont have to keep per thread info. If the spec change does not go
    // through we will have to write down the last provider called by a thread
    // in thread local storage so we can know what provider to send this call
    // to.
    return(WSAEINVAL);
    
}



INT
WSPAPI
WSPCleanup(
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Terminate use of the WinSock service provider.

Arguments:

    lpErrno - A pointer to the error code.

Return Value:

    The  return  value  is  0 if the operation has been successfully initiated.
    Otherwise  the  value SOCKET_ERROR is returned,
and a specific error number
    is available in lpErrno.

--*/

{
    INT          ReturnValue;
    INT          Errno;
    PLIST_ENTRY  ListMember;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    
    if (PREAPINOTIFY(( DTCODE_WSPCleanup,
                       &ReturnValue,
                       gLibraryName,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    EnterCriticalSection(&gInitCriticalSection);
    if (gStartupCount > 0){
        gStartupCount--;
        if (gStartupCount == 0){

            DEBUGF( DBG_TRACE,
                    ("Tearing down layered provider\n"));
            
            //Kill all the open sockets
            ListMember = &gSocketList;

            while (ListMember != &gSocketList){
                Socket = CONTAINING_RECORD(
                    ListMember,
                    DSOCKET,
                    m_list_linkage);
                ListMember = ListMember->Flink;

                Provider = Socket->GetDProvider();
                ProviderSocket = Socket->GetSocketHandle();

                
                Provider->WSPCloseSocket(
                    ProviderSocket,
                    &Errno);

                gUpCallTable.lpWPUCloseSocketHandle(
                    Socket->GetContext(),
                    &Errno);
                
                RemoveEntryList(
                    &Socket->m_list_linkage);
                delete(Socket);
            } //while

            // Kill the ProviderCatalog
            delete(gProviderCatalog);

            //Kill the worker thread
            delete(gWorkerThread);

            //Kill the buffer manager
            delete(gBufferManager);
            DEBUGF( DBG_TRACE,
                    ("Tearing down Complete\n"));
            
        } //if
    } //if
    ReturnValue = NO_ERROR;
    
    LeaveCriticalSection(&gInitCriticalSection);
    
    POSTAPINOTIFY(( DTCODE_WSPCleanup,
                    &ReturnValue,
                    gLibraryName,
                    &lpErrno));

    return(ReturnValue);

}



INT
WSPAPI
WSPCloseSocket(
    IN SOCKET s,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Close a socket.

Arguments:

    s       - A descriptor identifying a socket.

    lpErrno - A pointer to the error code.

Return Value:

    If  no  erro  occurs, WSPCloseSocket()  returns  0.  Otherwise, a value of
    SOCKET_ERROR  is  returned,  and  a  specific  error  code  is available in
    lpErrno.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;

    
    if (PREAPINOTIFY(( DTCODE_WSPCloseSocket,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();
        DEBUGF( DBG_TRACE,
                ("Closing socket %X\n",s));
                
        ReturnValue = Provider->WSPCloseSocket(
            ProviderSocket,
            lpErrno);

        gUpCallTable.lpWPUCloseSocketHandle(
            Socket->GetContext(),
            lpErrno);
             
        RemoveEntryList(
            &Socket->m_list_linkage);
        delete(Socket);

        ReturnValue = NO_ERROR;
    } //if
    

    POSTAPINOTIFY(( DTCODE_WSPCloseSocket,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &lpErrno));

    return(ReturnValue);

}



INT
WSPAPI
WSPConnect(
    IN SOCKET s,
    IN const struct sockaddr FAR *name,
    IN INT namelen,
    IN LPWSABUF lpCallerData,
    IN LPWSABUF lpCalleeData,
    IN LPQOS lpSQOS,
    IN LPQOS lpGQOS,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Establish a connection to a peer,
exchange connect data,
and specify needed
    quality of service based on the supplied flow spec.

Arguments:

    s            - A descriptor identifying an unconnected socket.

    name         - The name of the peer to which the socket is to be connected.

    namelen      - The length of the name.

    lpCallerData - A  pointer to the user data that is to be transferred to the
                   peer during connection established.

    lpCalleeData - A pointer to a buffer into which may be copied any user data
                   received from the peer during connection establishment.

    lpSQOS       - A  pointer  to  the  flow  specs  for socket s, one for each
                   direction.

    lpGQOS       - A  pointer  to  the  flow  specs  for  the  socket group (if
                   applicable).

    lpErrno      - A pointer to the error code.

Return Value:

    If  no  error  occurs, WSPConnect()  returns NO_ERROR.  Otherwise, it
    returns SOCKET_ERROR, and a specific erro rcode is available in lpErrno.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    
    if (PREAPINOTIFY(( DTCODE_WSPConnect,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &name,
                       &namelen,
                       &lpCallerData,
                       &lpCalleeData,
                       &lpSQOS,
                       &lpGQOS,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        ReturnValue = Provider->WSPConnect(
            ProviderSocket,
            name,
            namelen,
            lpCallerData,
            lpCalleeData,
            lpSQOS,
            lpGQOS,
            lpErrno);
    }//if
    
    POSTAPINOTIFY(( DTCODE_WSPConnect,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &name,
                    &namelen,
                    &lpCallerData,
                    &lpCalleeData,
                    &lpSQOS,
                    &lpGQOS,
                    &lpErrno));

    return(ReturnValue);

}



INT
WSPAPI
WSPDuplicateSocket(
    IN SOCKET s,
    IN DWORD dwProcessID,
    IN LPWSAPROTOCOL_INFOW lpProtocolInfo,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    descriptor for a shared socket.


Arguments:

    s              - Specifies the local socket descriptor.

    dwProcessID    - Specifies  the  ID  of  the  target  process for which the
                     shared socket will be used.

    lpProtocolInfo - A  pointer  to  a  buffer  allocated by the client that is
                     large enough to contain a WSAPROTOCOL_INFOA struct.  The
                     service  provider copies the protocol info struct contents
                     to this buffer.

    lpErrno        - A pointer to the error code

Return Value:

    If  no  error  occurs, WPSDuplicateSocket()  returns zero.  Otherwise, the
    value of SOCKET_ERROR is returned, and a specific error number is available
    in lpErrno.

--*/
{
    INT ReturnValue=SOCKET_ERROR;

//      if (PREAPINOTIFY(( DTCODE_WSPDuplicateSocket,
//                         &ReturnValue,
//                         gLibraryName,
//                         &s,
//                         &dwProcessID,
//                         &lpProtocolInfo,
//                         &lpErrno)) ) {
//          return(ReturnValue);
//      }

//      ReturnValue = gProcTable.lpWSPDuplicateSocket(
//          s,
//          dwProcessID,
//          lpProtocolInfo,
//          lpErrno);

//      POSTAPINOTIFY(( DTCODE_WSPDuplicateSocket,
//                      &ReturnValue,
//                      gLibraryName,
//                      &s,
//                      &dwProcessID,
//                      &lpProtocolInfo,
//                      &lpErrno));

    return(ReturnValue);

}



INT
WSPAPI
WSPEnumNetworkEvents(
    IN SOCKET s,
    OUT WSAEVENT hEventObject,
    OUT LPWSANETWORKEVENTS lpNetworkEvents,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Report occurrences of network events for the indicated socket.

Arguments:

    s               - A descriptor identifying the socket.

    hEventObject    - An optional handle identifying an associated event object
                      to be reset.

    lpNetworkEvents - A  pointer  to  a WSANETWORKEVENTS struct which is filled
                      with   a  record  of  occurred  network  events  and  any
                      associated error codes.

    lpErrno         - A pointer to the error code.

Return Value:

    The  return  value  is  NO_ERROR  if  the  operation  was  successful.
    Otherwise  the  value SOCKET_ERROR is returned, and a specific error number
    is available in lpErrno.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;

    if (PREAPINOTIFY(( DTCODE_WSPEnumNetworkEvents,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &hEventObject,
                       &lpNetworkEvents,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();
        
        ReturnValue = Provider->WSPEnumNetworkEvents(
            ProviderSocket,
            hEventObject,
            lpNetworkEvents,
            lpErrno);
    } //if

    POSTAPINOTIFY(( DTCODE_WSPEnumNetworkEvents,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &hEventObject,
                    &lpNetworkEvents,
                    &lpErrno));

    return(ReturnValue);

}



INT
WSPAPI
WSPEventSelect(
    IN SOCKET s,
    IN OUT WSAEVENT hEventObject,
    IN long lNetworkEvents,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Specify  an  event object to be associated with the supplied set of network
    events.

Arguments:

    s              - A descriptor identifying the socket.

    hEventObject   - A  handle  identifying  the  event object to be associated
                     with the supplied set of network events.

    lNetworkEvents - A  bitmask  which  specifies  the  combination  of network
                     events in which the WinSock client has interest.

    lpErrno        - A pointer to the error code.

Return Value:

    The return value is 0 if the WinSock client's specification of the network
    events and the associated event object was successful. Otherwise the value
    SOCKET_ERROR is returned, and a specific error number is available in
    lpErrno

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPEventSelect,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &hEventObject,
                       &lNetworkEvents,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        ReturnValue = Provider->WSPEventSelect(
            ProviderSocket,
            hEventObject,
            lNetworkEvents,
            lpErrno);
    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPEventSelect,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &hEventObject,
                    &lNetworkEvents,
                    &lpErrno));

    return(ReturnValue);

}



INT
WSPAPI
WSPGetOverlappedResult(
    IN SOCKET s,
    IN LPWSAOVERLAPPED lpOverlapped,
    IN LPDWORD lpcbTransfer,
    IN BOOL fWait,
    OUT LPDWORD lpdwFlags,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Returns the results of an overlapped operation on the specified socket.

Arguments:

    s            - Identifies  the  socket.   This  is the same socket that was
                   specified  when  the  overlapped  operation was started by a
                   call to WSPRecv(), WSPRecvFrom(), WSPSend(), WSPSendTo(), or
                   WSPIoctl().

    lpOverlapped - Points to a WSAOVERLAPPED structure that was specified
                   when the overlapped operation was started.

    lpcbTransfer - Points to a 32-bit variable that receives the number of
                   bytes that were actually transferred by a send or receive
                   operation, or by WSPIoctl().

    fWait        - Specifies  whether  the function should wait for the pending
                   overlapped  operation  to  complete.   If TRUE, the function
                   does  not return until the operation has been completed.  If
                   FALSE  and  the  operation  is  still  pending, the function
                   returns FALSE and lperrno is WSA_IO_INCOMPLETE.

    lpdwFlags    - Points  to  a  32-bit variable that will receive one or more
                   flags   that  supplement  the  completion  status.   If  the
                   overlapped   operation   was   initiated  via  WSPRecv()  or
                   WSPRecvFrom(), this parameter will contain the results value
                   for lpFlags parameter.

    lpErrno      - A pointer to the error code.

Return Value:

    If WSPGetOverlappedResult() succeeds,the return value is TRUE.  This means
    that the overlapped operation has completed successfully and that the value
    pointed  to  by lpcbTransfer has been updated.  If WSPGetOverlappedResult()
    returns  FALSE,  this  means  that  either the overlapped operation has not
    completed  or  the  overlapped operation completed but with errors, or that
    completion  status  could  not  be  determined due to errors in one or more
    parameters  to  WSPGetOverlappedResult().  On failure, the value pointed to
    by  lpcbTransfer  will  not be updated.  lpErrno indicates the cause of the
    failure (either of WSPGetOverlappedResult() or of the associated overlapped
    operation).

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPGetOverlappedResult,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &lpOverlapped,
                       &lpcbTransfer,
                       &fWait,
                       &lpdwFlags,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        // TODO we need to get in the way here
        ReturnValue = Provider->WSPGetOverlappedResult(
            ProviderSocket,
            lpOverlapped,
            lpcbTransfer,
            fWait,
            lpdwFlags,
            lpErrno);
    } //if
   
    POSTAPINOTIFY(( DTCODE_WSPGetOverlappedResult,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &lpOverlapped,
                    &lpcbTransfer,
                    &fWait,
                    &lpdwFlags,
                    &lpErrno));

    return(ReturnValue);
}



INT
WSPAPI
WSPGetPeerName(
    IN SOCKET s,
    OUT struct sockaddr FAR *name,
    OUT INT FAR *namelen,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Get the address of the peer to which a socket is connected.

Arguments:

    s       - A descriptor identifying a connected socket.

    name    - A  pointer  to  the structure which is to receive the name of the
              peer.

    namelen - A  pointer  to  an integer which, on input, indicates the size of
              the  structure  pointed  to  by name, and on output indicates the
              size of the returned name.

    lpErrno - A pointer to the error code.

Return Value:

    If  no  error occurs, WSPGetPeerName() returns NO_ERROR.  Otherwise, a
    value  of  SOCKET_ERROR is returned, and a specific error code is available
    in lpErrno

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPGetPeerName,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &name,
                       &namelen,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        ReturnValue = Provider->WSPGetPeerName(
        ProviderSocket,
        name,
        namelen,
        lpErrno);

    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPGetPeerName,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &name,
                    &namelen,
                    &lpErrno));

    return(ReturnValue);
}



INT
WSPAPI
WSPGetQOSByName(
    IN SOCKET s,
    IN LPWSABUF lpQOSName,
    IN LPQOS lpQOS,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Initializes a QOS structure based on a named template.

Arguments:

    s         - A descriptor identifying a socket.

    lpQOSName - Specifies the QOS template name.

    lpQOS     - A pointer to the QOS structure to be filled.

    lpErrno   - A pointer to the error code.

Return Value:

    If the function succeeds, the return value is TRUE.  If the function fails,
    the  return  value  is  FALSE, and  a  specific error code is available in
    lpErrno.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPGetQOSByName,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &lpQOSName,
                       &lpQOS,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();
        
        ReturnValue = Provider->WSPGetQOSByName(
            ProviderSocket,
            lpQOSName,
            lpQOS,
            lpErrno);
    } //if

    POSTAPINOTIFY(( DTCODE_WSPGetQOSByName,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &lpQOSName,
                    &lpQOS,
                    &lpErrno));

    return(ReturnValue);
}



INT
WSPAPI
WSPGetSockName(
    IN SOCKET s,
    OUT struct sockaddr FAR *name,
    OUT INT FAR *namelen,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Get the local name for a socket.

Arguments:

    s       - A descriptor identifying a bound socket.

    name    - A pointer to a structure used to supply the address (name) of the
              socket.

    namelen - A  pointer  to  an integer which, on input, indicates the size of
              the  structure  pointed  to  by name, and on output indicates the
              size of the returned name

    lpErrno - A Pointer to the error code.

Return Value:

    If  no  error occurs, WSPGetSockName() returns NO_ERROR.  Otherwise, a
    value  of  SOCKET_ERROR is returned, and a specific error code is available
    in lpErrno.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPGetSockName,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &name,
                       &namelen,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        ReturnValue = Provider->WSPGetSockName(
            ProviderSocket,
            name,
            namelen,
            lpErrno);
    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPGetSockName,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &name,
                    &namelen,
                    &lpErrno));

    return(ReturnValue);
}



 INT
WSPAPI
WSPGetSockOpt(
    IN SOCKET s,
    IN INT level,
    IN INT optname,
    OUT char FAR *optval,
    OUT INT FAR *optlen,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Retrieve a socket option.

Arguments:

    s       - A descriptor identifying a socket.

    level   - The  level  at  which the option is defined; the supported levels
              include SOL_SOCKET (See annex for more protocol-specific levels.)

    optname - The socket option for which the value is to be retrieved.

    optval  - A  pointer  to  the  buffer  in which the value for the requested
              option is to be returned.

    optlen  - A pointer to the size of the optval buffer.

    lpErrno - A pointer to the error code.

Return Value:

    If  no  error  occurs,  WSPGetSockOpt()  returns  0.  Otherwise, a value of
    SOCKET_ERROR  is  returned,  and  a  specific  error  code  is available in
    lpErrno.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPGetSockOpt,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &level,
                       &optname,
                       &optval,
                       &optlen,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        ReturnValue = Provider->WSPGetSockOpt(
            ProviderSocket,
            level,
            optname,
            optval,
            optlen,
            lpErrno);
    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPGetSockOpt,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &level,
                    &optname,
                    &optval,
                    &optlen,
                    &lpErrno));

    return(ReturnValue);
}


 INT
WSPAPI
WSPIoctl(
    IN SOCKET s,
    IN DWORD dwIoControlCode,
    IN LPVOID lpvInBuffer,
    IN DWORD cbInBuffer,
    IN LPVOID lpvOutBuffer,
    IN DWORD cbOutBuffer,
    IN LPDWORD lpcbBytesReturned,
    IN LPWSAOVERLAPPED lpOverlapped,
    IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
    IN LPWSATHREADID lpThreadId,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Control the mode of a socket.

Arguments:

    s                   - Handle to a socket

    dwIoControlCode     - Control code of operation to perform

    lpvInBuffer         - Address of input buffer

    cbInBuffer          - Size of input buffer

    lpvOutBuffer        - Address of output buffer

    cbOutBuffer         - Size of output buffer

    lpcbBytesReturned   - A pointer to the size of output buffer's contents.

    lpOverlapped        - Address of WSAOVERLAPPED structure

    lpCompletionRoutine - A  pointer  to the completion routine called when the
                          operation has been completed.

    lpThreadId          - A  pointer to a thread ID structure to be used by the
                          provider

    lpErrno             - A pointer to the error code.

Return Value:

    If  no error occurs and the operation has completed immediately, WSPIoctl()
    returns  0.   Note  that in this case the completion routine, if specified,
    will  have  already  been  queued.   Otherwise, a value of SOCKET_ERROR is
    returned, and  a  specific  error code is available in lpErrno.  The error
    code  WSA_IO_PENDING  indicates  that  an  overlapped  operation  has  been
    successfully  initiated  and  that  conpletion will be indicated at a later
    time.   Any  other  error  code  indicates that no overlapped operation was
    initiated and no completion indication will occur.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPIoctl,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &dwIoControlCode,
                       &lpvInBuffer,
                       &cbInBuffer,
                       &lpvOutBuffer,
                       &cbOutBuffer,
                       &lpcbBytesReturned,
                       &lpOverlapped,
                       &lpCompletionRoutine,
                       &lpThreadId,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        ReturnValue = Provider->WSPIoctl(
            ProviderSocket,
            dwIoControlCode,
            lpvInBuffer,
            cbInBuffer,
            lpvOutBuffer,
            cbOutBuffer,
            lpcbBytesReturned,
            lpOverlapped,
            lpCompletionRoutine,
            lpThreadId,
            lpErrno);
    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPIoctl,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &dwIoControlCode,
                    &lpvInBuffer,
                    &cbInBuffer,
                    &lpvOutBuffer,
                    &cbOutBuffer,
                    &lpcbBytesReturned,
                    &lpOverlapped,
                    &lpCompletionRoutine,
                    &lpThreadId,
                    &lpErrno));

    return(ReturnValue);
}


SOCKET
WSPAPI
WSPJoinLeaf(
    IN SOCKET s,
    IN const struct sockaddr FAR *name,
    IN INT namelen,
    IN LPWSABUF lpCallerData,
    IN LPWSABUF lpCalleeData,
    IN LPQOS lpSQOS,
    IN LPQOS lpGQOS,
    IN DWORD dwFlags,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Join  a  leaf  node  into  a multipoint session, exchange connect data, and
    specify needed quality of service based on the supplied flow specs.

Arguments:

    s            - A descriptor identifying a multipoint socket.

    name         - The name of the peer to which the socket is to be joined.

    namelen      - The length of the name.

    lpCallerData - A  pointer to the user data that is to be transferred to the
                   peer during multipoint session establishment.

    lpCalleeData - A  pointer  to  the user data that is to be transferred back
                   from the peer during multipoint session establishment.

    lpSQOS       - A  pointer  to  the  flow  specs  for socket s, one for each
                   direction.

    lpGQOS       - A  pointer  to  the  flow  specs  for  the  socket group (if
                   applicable).

    dwFlags      - Flags  to  indicate  that  the socket is acting as a sender,
                   receiver, or both.

    lpErrno      - A pointer to the error code.

Return Value:

    If no error occurs, WSPJoinLeaf() returns a value of type SOCKET which is a
    descriptor  for the newly created multipoint socket.  Otherwise,a value of
    INVALID_SOCKET  is  returned, and  a  specific  error code is available in
    lpErrno.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPJoinLeaf,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &name,
                       &namelen,
                       &lpCallerData,
                       &lpCalleeData,
                       &lpSQOS,
                       &lpGQOS,
                       &dwFlags,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        ReturnValue = Provider->WSPJoinLeaf(
            ProviderSocket,
            name,
            namelen,
            lpCallerData,
            lpCalleeData,
            lpSQOS,
            lpGQOS,
            dwFlags,
            lpErrno);
    } //if

    POSTAPINOTIFY(( DTCODE_WSPJoinLeaf,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &name,
                    &namelen,
                    &lpCallerData,
                    &lpCalleeData,
                    &lpSQOS,
                    &lpGQOS,
                    &dwFlags,
                    &lpErrno));

    return(ReturnValue);

}



INT
WSPAPI
WSPListen(
    IN SOCKET s,
    IN INT backlog,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Establish a socket to listen for incoming connections.

Arguments:

    s       - A descriptor identifying a bound,
unconnected socket.

    backlog - The  maximum length to which the queue of pending connections may
              grow.   If  this  value  is  SOMAXCONN,
then the service provider
              should set the backlog to a maximum "reasonable" value.

    lpErrno - A pointer to the error code.

Return Value:

    If  no  error  occurs, WSPListen()  returns  0.   Otherwise, a  value  of
    SOCKET_ERROR  is  returned, and  a  specific  error  code  is available in
    lpErrno.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPListen,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &backlog,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        ReturnValue = Provider->WSPListen(
            ProviderSocket,
            backlog,
            lpErrno);
    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPListen,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &backlog,
                    &lpErrno));
    
    return(ReturnValue);
}



INT
WSPAPI
WSPRecv(
    IN SOCKET s,
    IN LPWSABUF lpBuffers,
    IN DWORD dwBufferCount,
    IN LPDWORD lpNumberOfBytesRecvd,
    IN OUT LPDWORD lpFlags,
    IN LPWSAOVERLAPPED lpOverlapped,
    IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
    IN LPWSATHREADID lpThreadId,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Receive data on a socket.

Arguments:

    s                    - A descriptor identifying a connected socket.

    lpBuffers            - A  pointer  to  an array of WSABUF structures.  Each
                           WSABUF  structure contains a pointer to a buffer and
                           the length of the buffer.

    dwBufferCount        - The  number  of  WSABUF  structures in the lpBuffers
                           array.

    lpNumberOfBytesRecvd - A  pointer  to  the number of bytes received by this
                           call.

    lpFlags              - A pointer to flags.

    lpOverlapped         - A pointer to a WSAOVERLAPPED structure.

    lpCompletionRoutine  - A  pointer to the completion routine called when the
                           receive operation has been completed.

    lpThreadId           - A pointer to a thread ID structure to be used by the
                           provider in a subsequent call to WPUQueueApc().

    lpErrno              - A pointer to the error code.

Return Value:

    If  no  error  occurs  and the receive operation has completed immediately,
    WSPRecv() returns the number of bytes received.  If the connection has been
    closed, it  returns  0.  Note that in this case the completion routine, if
    specified,  will   have  already  been  queued.   Otherwise, a  value  of
    SOCKET_ERROR  is  returned, and  a  specific  error  code  is available in
    lpErrno.   The  error  code WSA_IO_PENDING indicates that the overlapped an
    operation  has  been  successfully  initiated  and  that completion will be
    indicated  at  a  later  time.   Any  other  error  code  indicates that no
    overlapped  operations  was  initiated  and  no  completion indication will
    occur.
--*/
{
    INT ReturnValue;
    PDSOCKET         Socket;
    PDPROVIDER       Provider;
    SOCKET           ProviderSocket;
    LPWSABUF         InternalBuffers;
    DWORD            InternalBufferCount;
    

    if (PREAPINOTIFY(( DTCODE_WSPRecv,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &lpBuffers,
                       &dwBufferCount,
                       &lpNumberOfBytesRecvd,
                       &lpFlags,
                       &lpOverlapped,
                       &lpCompletionRoutine,
                       &lpThreadId,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        // Get Internal buffers to send down to the lower provider.
        ReturnValue = gBufferManager->AllocBuffer(
            lpBuffers,
            dwBufferCount,
            &InternalBuffers,
            &InternalBufferCount);
        
        if (NO_ERROR == ReturnValue){
            //Is this a overlapped operation.
            if (lpOverlapped){
                // Setup the user overlapped struct
                lpOverlapped->Internal = WSA_IO_PENDING;
                lpOverlapped->InternalHigh = 0;
                ReturnValue = gWorkerThread->QueueOverlappedRecv(
                    Provider,
                    ProviderSocket,
                    lpBuffers,
                    dwBufferCount,
                    lpNumberOfBytesRecvd,
                    lpFlags,
                    lpOverlapped,
                    lpCompletionRoutine,
                    lpThreadId,
                    InternalBuffers,
                    InternalBufferCount,
                    lpErrno);
            } //if
            else{
                ReturnValue = Provider->WSPRecv(
                    ProviderSocket,
                    lpBuffers,
                    dwBufferCount,
                    lpNumberOfBytesRecvd,
                    lpFlags,
                    lpOverlapped,
                    lpCompletionRoutine,
                    lpThreadId,
                    lpErrno);
            } //else
        } //if
    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPRecv,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &lpBuffers,
                    &dwBufferCount,
                    &lpNumberOfBytesRecvd,
                    &lpFlags,
                    &lpOverlapped,
                    &lpCompletionRoutine,
                    &lpThreadId,
                    &lpErrno));

    return(ReturnValue);
}



 INT
WSPAPI
WSPRecvDisconnect(
    IN SOCKET s,
    IN LPWSABUF lpInboundDisconnectData,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Terminate  reception  on  a socket, and retrieve the disconnect data if the
    socket is connection-oriented.

Arguments:

    s                       - A descriptor identifying a socket.

    lpInboundDisconnectData - A  pointer to a buffer into which disconnect data
                              is to be copied.

    lpErrno                 - A pointer to the error code.

Return Value:

    If  no error occurs, WSPRecvDisconnect() returns NO_ERROR.  Otherwise,
    a value of SOCKET_ERROR is returned, and a specific error code is available
    in lpErrno.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPRecvDisconnect,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &lpInboundDisconnectData,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);

    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        ReturnValue = Provider->WSPRecvDisconnect(
            ProviderSocket,
            lpInboundDisconnectData,
            lpErrno);
    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPRecvDisconnect,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &lpInboundDisconnectData,
                    &lpErrno));

    return(ReturnValue);

}



INT
WSPAPI
WSPRecvFrom(
    IN  SOCKET s,
    IN  LPWSABUF lpBuffers,
    IN  DWORD dwBufferCount,
    IN  LPDWORD lpNumberOfBytesRecvd,
    IN  OUT LPDWORD lpFlags,
    OUT struct sockaddr FAR *  lpFrom,
    IN  LPINT lpFromlen,
    IN  LPWSAOVERLAPPED lpOverlapped,
    IN  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
    IN LPWSATHREADID lpThreadId,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Receive a datagram and store the source address.

Arguments:

    s                    - A descriptor identifying a socket.

    lpBuffers            - A  pointer  to  an array of WSABUF structures.  Each
                           WSABUF  structure contains a pointer to a buffer and
                           the length of the buffer.

    dwBufferCount        - The  number  of  WSABUF  structures in the lpBuffers
                           array.

    lpNumberOfBytesRecvd - A  pointer  to  the number of bytes received by this
                           call.

    lpFlags              - A pointer to flags.

    lpFrom               - An  optional pointer to a buffer which will hold the
                           source address upon the completion of the overlapped
                           operation.

    lpFromlen            - A  pointer  to the size of the from buffer, required
                           only if lpFrom is specified.

    lpOverlapped         - A pointer to a WSAOVERLAPPED structure.

    CompletionRoutine    - A  pointer to the completion routine called when the
                           receive operation has been completed.

    lpThreadId           - A pointer to a thread ID structure to be used by the
                           provider in a subsequent call to WPUQueueApc().

    lpErrno              - A pointer to the error code.

Return Value:

    If  no  error  occurs  and the receive operation has completed immediately,
    WSPRecvFrom()  returns the number of bytes received.  If the connection has
    been  closed, it returns 0.  Note that in this case the completion routine,
    if  specified  will  have  already  been  queued.   Otherwise,  a  value of
    SOCKET_ERROR  is  returned, and  a  specific  error  code  is available in
    lpErrno.   The  error  code  WSA_IO_PENDING  indicates  that the overlapped
    operation  has  been  successfully  initiated  and  that completion will be
    indicated  at  a  later  time.   Any  other  error  code  indicates that no
    overlapped  operations  was  initiated  and  no  completion indication will
    occur.

--*/
{
    INT              ReturnValue;
    PDSOCKET         Socket;
    PDPROVIDER       Provider;
    SOCKET           ProviderSocket;
    LPWSABUF         InternalBuffers;
    DWORD            InternalBufferCount;
    

    if (PREAPINOTIFY(( DTCODE_WSPRecvFrom,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &lpBuffers,
                       &dwBufferCount,
                       &lpNumberOfBytesRecvd,
                       &lpFlags,
                       &lpFrom,
                       &lpFromlen,
                       &lpOverlapped,
                       &lpCompletionRoutine,
                       &lpThreadId,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        // Get Internal buffers to send down to the lower provider.
        ReturnValue = gBufferManager->AllocBuffer(
            lpBuffers,
            dwBufferCount,
            &InternalBuffers,
            &InternalBufferCount);
        
        if (NO_ERROR == ReturnValue){
            //Is this a overlapped operation.
            if (lpOverlapped){

                // Setup the user overlapped struct
                lpOverlapped->Internal = WSA_IO_PENDING;
                lpOverlapped->InternalHigh = 0;
                ReturnValue = gWorkerThread->QueueOverlappedRecvFrom(
                    Provider,
                    ProviderSocket,
                    InternalBuffers,
                    InternalBufferCount,
                    lpNumberOfBytesRecvd,
                    lpFlags,
                    lpFrom,
                    lpFromlen,
                    lpOverlapped,
                    lpCompletionRoutine,
                    lpThreadId,
                   InternalBuffers,
                    InternalBufferCount,
                    lpErrno);
            } //if
            else{
                ReturnValue = Provider->WSPRecvFrom(
                    ProviderSocket,
                    lpBuffers,
                    dwBufferCount,
                    lpNumberOfBytesRecvd,
                    lpFlags,
                    lpFrom,
                    lpFromlen,
                    lpOverlapped,
                    lpCompletionRoutine,
                    lpThreadId,
                    lpErrno);
            } //else
        } //if
    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPRecvFrom,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &lpBuffers,
                    &dwBufferCount,
                    &lpNumberOfBytesRecvd,
                    &lpFlags,
                    &lpFrom,
                    &lpFromlen,
                    &lpOverlapped,
                    &lpCompletionRoutine,
                    &lpThreadId,
                    &lpErrno));

    return(ReturnValue);
}

typedef struct association
{
    SOCKET  ProviderSocket;
    SOCKET  UserSocket;
} SOCKETASSOCIATION, *PSOCKETASSOCIATION;

typedef struct
{
    UINT                AssociationCount;
    PSOCKETASSOCIATION  Associations;
} SOCKETMAP, *PSOCKETMAP;


    
INT
TransferUserFdSetToProviderFdSet(
    IN  fd_set * UserSet,
    OUT fd_set * ProviderSet,
    OUT PSOCKETMAP SocketMap,
    OUT LPINT    Errno)
{
    INT ReturnCode;
    UINT Index;
    PDSOCKET Socket;
    
    ReturnCode= NO_ERROR;
    SocketMap->AssociationCount = 0;
    SocketMap->Associations     = NULL;
    ProviderSet->fd_count       = 0;
   
    if (UserSet && (UserSet->fd_count > 0)){
        if (UserSet->fd_count > FD_SETSIZE){
            *Errno = WSAENOBUFS;
            return(SOCKET_ERROR);
        } //if
        SocketMap->Associations = (PSOCKETASSOCIATION)new BYTE[
            (sizeof(SOCKETASSOCIATION) * UserSet->fd_count)];
        if (SocketMap->Associations){

            for (Index=0;Index < UserSet->fd_count  ;Index++ ){

                ReturnCode = gUpCallTable.lpWPUQuerySocketHandleContext(
                    UserSet->fd_array[Index],
                    (DWORD*)&Socket,
                    Errno);
                if (NO_ERROR != ReturnCode){
                    delete(SocketMap->Associations);
                    SocketMap->Associations = NULL;
                    *Errno = WSAEINVAL;
                    break;
                } //if

                SocketMap->Associations[Index].ProviderSocket =
                    Socket->GetSocketHandle();
                SocketMap->Associations[Index].UserSocket =
                    UserSet->fd_array[Index];
                ProviderSet->fd_array[Index] =
                    SocketMap->Associations[Index].ProviderSocket;
               
                ProviderSet->fd_count++;
                SocketMap->AssociationCount++;            
            } //for
        } //if
        else{
             ReturnCode =SOCKET_ERROR;
            *Errno = WSAENOBUFS;
        } //else
    } //if
    return(ReturnCode);
}

INT
TransferProviderFdSetToUserFdSet(
    IN  fd_set *   UserSet,
    OUT fd_set *   ProviderSet,
    IN  PSOCKETMAP SocketMap,
    OUT LPINT      Errno)
{
    INT ReturnCode;
    UINT ProviderIndex;
    UINT AssociationIndex;
    
    ReturnCode= NO_ERROR;
   
    if (UserSet && (ProviderSet->fd_count > 0)){
        UserSet->fd_count = 0;     
        for (ProviderIndex = 0;
             ProviderIndex < ProviderSet->fd_count;
             ProviderIndex++){
            
            for (AssociationIndex =0;
                 AssociationIndex < SocketMap->AssociationCount;
                 AssociationIndex++){
                
                if (ProviderSet->fd_array[ProviderIndex] ==
                    SocketMap->Associations[AssociationIndex].ProviderSocket){
                    
                    UserSet->fd_array[ProviderIndex] =
                        SocketMap->Associations[AssociationIndex].UserSocket;
                    UserSet->fd_count++;
                } //if
            } //for
        } //for
        delete SocketMap->Associations;
    } //if
    
    return(ReturnCode);
    
}

INT
WSPAPI
WSPSelect(
    IN INT nfds,
    IN OUT fd_set FAR *readfds,
    IN OUT fd_set FAR *writefds,
    IN OUT fd_set FAR *exceptfds,
    IN const struct timeval FAR *timeout,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Determine the status of one or more sockets.

Arguments:

    nfds      - This  argument  is  ignored  and  included only for the sake of
                compatibility.

    readfds   - An  optional  pointer  to  a  set  of sockets to be checked for
                readability.

    writefds  - An  optional  pointer  to  a  set  of sockets to be checked for
                writability

    exceptfds - An  optional  pointer  to  a  set  of sockets to be checked for
                errors.

    timeout   - The  maximum  time  for  WSPSelect()  to  wait, or  NULL for a
                blocking operation.

    lpErrno   - A pointer to the error code.

Return Value:

    WSPSelect()  returns  the  total  number of descriptors which are ready and
    contained  in  the  fd_set  structures, 0  if  the  time limit expired, or
    SOCKET_ERROR  if an error occurred.  If the return value is SOCKET_ERROR, a
    specific error code is available in lpErrno.

--*/
{
    INT ReturnValue;
    PDPROVIDER   Provider;
    SOCKET       SocketHandle;
    BOOL         FoundSocket=FALSE;
    PDSOCKET     Socket;
    fd_set       InternalReadfds;
    fd_set       InternalWritefds;
    fd_set       InternalExceptfds;
    SOCKETMAP    ReadMap;
    SOCKETMAP    WriteMap;
    SOCKETMAP    ExceptMap;
    
    if (PREAPINOTIFY(( DTCODE_WSPSelect,
                       &ReturnValue,
                       gLibraryName,
                       &nfds,
                       &readfds,
                       &writefds,
                       &exceptfds,
                       &timeout,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    // Look for a socket in the three fd_sets handed in. The first
    // socket found will be used to select the service provider to
    // service this call
    if (readfds && readfds->fd_count){
        
        SocketHandle = readfds->fd_array[0];
        FoundSocket = TRUE;
    } //if
    
    if (!FoundSocket && writefds && writefds->fd_count ){
        
        SocketHandle = writefds->fd_array[0];
        FoundSocket = TRUE;
    } //if

    if (!FoundSocket && exceptfds && exceptfds->fd_count ){
        
        SocketHandle = exceptfds->fd_array[0];
        FoundSocket = TRUE;
    } //if
    if (FoundSocket){
        //
        // Get our DSOCKET object
        //
        ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
            SocketHandle,
            (DWORD*)&Socket,
            lpErrno);
        if (SOCKET_ERROR != ReturnValue){
            Provider = Socket->GetDProvider();

            TransferUserFdSetToProviderFdSet(
                readfds,
                &InternalReadfds,
                &ReadMap,
                lpErrno);
            TransferUserFdSetToProviderFdSet(
                writefds,
                &InternalWritefds,
                &WriteMap,
                lpErrno);
            TransferUserFdSetToProviderFdSet(
                exceptfds,
                &InternalExceptfds,
                &ExceptMap,
                lpErrno);
            if (NO_ERROR == *lpErrno){
                ReturnValue = Provider->WSPSelect(
                    nfds,
                    &InternalReadfds,
                    &InternalWritefds,
                    &InternalExceptfds,
                    timeout,
                    lpErrno);
                TransferProviderFdSetToUserFdSet(
                    readfds,
                    &InternalReadfds,
                    &ReadMap,
                    lpErrno);
                TransferProviderFdSetToUserFdSet(
                    writefds,
                    &InternalWritefds,
                    &WriteMap,
                    lpErrno);
                TransferProviderFdSetToUserFdSet(
                    exceptfds,
                    &InternalExceptfds,
                    &ExceptMap,
                    lpErrno);
                DEBUGF( DBG_TRACE,
                        ("Select Returns %X\n",ReturnValue));
                
            } //if
            else{
                DEBUGF( DBG_TRACE,
                        ("**Select failed**\n"));
                
                ReturnValue = SOCKET_ERROR;
            } //else
        }
    } //if
    else{
        ReturnValue = SOCKET_ERROR;
        *lpErrno    = WSAEINVAL;
    } //else/if
    
    POSTAPINOTIFY(( DTCODE_WSPSelect,
                    &ReturnValue,
                    gLibraryName,
                    &nfds,
                    &readfds,
                    &writefds,
                    &exceptfds,
                    &timeout,
                    &lpErrno));

    return(ReturnValue);
}




 INT
WSPAPI
WSPSend(
    IN SOCKET s,
    IN LPWSABUF lpBuffers,
    IN DWORD dwBufferCount,
    IN LPDWORD lpNumberOfBytesSent,
    IN DWORD dwFlags,
    IN LPWSAOVERLAPPED lpOverlapped,
    IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
    IN LPWSATHREADID lpThreadId,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Send data on a connected socket.

Arguments:

    s                   - A descriptor identifying a connected socket.

    lpBuffers           - A  pointer  to  an  array of WSABUF structures.  Each
                          WSABUF  structure  contains a pointer to a buffer and
                          the length of the buffer.

    dwBufferCount       - The  number  of  WSABUF  structures  in the lpBuffers
                          array.

    lpNumberOfBytesSent - A pointer to the number of bytes sent by this call.

    dwFlags             - Flags.

    lpOverlapped        - A pointer to a WSAOVERLAPPED structure.

    lpCompletionRoutine - A  pointer  to the completion routine called when the
                          send operation has been completed.

    lpThreadId          - A  pointer to a thread ID structure to be used by the
                          provider in a subsequent call to WPUQueueApc().

    lpErrno             - A pointer to the error code.

Return Value:

    If  no  error  occurs  and  the  send  operation has completed immediately,
    WSPSend() returns the number of bytes received.  If the connection has been
    closed,  it  returns  0.  Note that in this case the completion routine, if
    specified, will   have  already  been  queued.   Otherwise, a  value  of
    SOCKET_ERROR  is  returned, and  a  specific  error  code  is available in
    lpErrno.   The  error  code  WSA_IO_PENDING  indicates  that the overlapped
    operation  has  been  successfully  initiated  and  that completion will be
    indicated  at  a  later  time.   Any  other  error  code  indicates that no
    overlapped operation was initiated and no completion indication will occur.

--*/
{
    INT              ReturnValue;
    PDSOCKET         Socket;
    PDPROVIDER       Provider;
    SOCKET           ProviderSocket;
    LPWSABUF         InternalBuffers;
    DWORD            InternalBufferCount;
    DWORD            InternalBytesTransfered;
    
    if (PREAPINOTIFY(( DTCODE_WSPSend,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &lpBuffers,
                       &dwBufferCount,
                       &lpNumberOfBytesSent,
                       &dwFlags,
                       &lpOverlapped,
                       &lpCompletionRoutine,
                       &lpThreadId,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        // Get Internal buffers to send down to the lower provider.
        ReturnValue = gBufferManager->AllocBuffer(
            lpBuffers,
            dwBufferCount,
            &InternalBuffers,
            &InternalBufferCount);
        
        if (NO_ERROR == ReturnValue){
            // Copy the user buffers
            ReturnValue = gBufferManager->CopyBuffer(
                lpBuffers,
                dwBufferCount,
                InternalBuffers,
                InternalBufferCount,
                &InternalBytesTransfered);
        } //if
        
        if (NO_ERROR == ReturnValue){
            //Is this a overlapped operation.
            if (lpOverlapped){

                ReturnValue = gWorkerThread->QueueOverlappedSend(
                        Provider,
                        ProviderSocket,
                        InternalBuffers,
                        InternalBufferCount,
                        lpNumberOfBytesSent,
                        dwFlags,
                        lpOverlapped,
                        lpCompletionRoutine,
                        lpThreadId,
                        lpErrno);
                    
            } //if
            else{
                ReturnValue = Provider->WSPSend(
                    ProviderSocket,
                    InternalBuffers,
                    InternalBufferCount,
                    lpNumberOfBytesSent,
                    dwFlags,
                    lpOverlapped,
                    lpCompletionRoutine,
                    lpThreadId,
                    lpErrno);
            } //else
        } //if
    } //if
    POSTAPINOTIFY(( DTCODE_WSPSend,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &lpBuffers,
                    &dwBufferCount,
                    &lpNumberOfBytesSent,
                    &dwFlags,
                    &lpOverlapped,
                    &lpCompletionRoutine,
                    &lpThreadId,
                    &lpErrno));

    return(ReturnValue);
}



 INT
WSPAPI
WSPSendDisconnect(
    IN SOCKET s,
    IN LPWSABUF lpOutboundDisconnectData,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Initiate  termination  of the connection for the socket and send disconnect
    data.

Arguments:

    s                        - A descriptor identifying a socket.

    lpOutboundDisconnectData - A pointer to the outgoing disconnect data.

    lpErrno                  - A pointer to the error code.

Return Value:

    If  no  error occurs, WSPSendDisconnect() returns 0.  Otherwise, a value of
    SOCKET_ERROR  is  returned, and  a  specific  error  code  is available in
    lpErrno.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPSendDisconnect,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &lpOutboundDisconnectData,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        ReturnValue = Provider->WSPSendDisconnect(
            ProviderSocket,
            lpOutboundDisconnectData,
            lpErrno);
    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPSendDisconnect,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &lpOutboundDisconnectData,
                    &lpErrno));

    return(ReturnValue);

}



INT
WSPAPI
WSPSendTo(
    IN SOCKET s,
    IN LPWSABUF lpBuffers,
    IN DWORD dwBufferCount,
    IN LPDWORD lpNumberOfBytesSent,
    IN DWORD dwFlags,
    IN const struct sockaddr FAR *  lpTo,
    IN INT iTolen,
    IN LPWSAOVERLAPPED lpOverlapped,
    IN LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
    IN LPWSATHREADID lpThreadId,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Send data to a specific destination using overlapped I/O.

Arguments:

    s                   - A descriptor identifying a socket.

    lpBuffers           - A  pointer  to  an  array of WSABUF structures.  Each
                          WSABUF  structure  contains a pointer to a buffer and
                          the length of the buffer.

    dwBufferCount       - The  number  of  WSABUF  structures  in the lpBuffers
                          array.

    lpNumberOfBytesSent - A pointer to the number of bytes sent by this call.

    dwFlags             - Flags.

    lpTo                - An  optional  pointer  to  the  address of the target
                          socket.

    iTolen              - The size of the address in lpTo.

    lpOverlapped        - A pointer to a WSAOVERLAPPED structure.

    lpCompletionRoutine - A  pointer  to the completion routine called when the
                          send operation has been completed.

    lpThreadId          - A  pointer to a thread ID structure to be used by the
                          provider in a subsequent call to WPUQueueApc().

    lpErrno             - A pointer to the error code.

Return Value:

    If  no  error  occurs  and the receive operation has completed immediately,
    WSPSendTo()  returns  the  number of bytes received.  If the connection has
    been  closed,it returns 0.  Note that in this case the completion routine,
    if  specified, will  have  already  been  queued.   Otherwise, a value of
    SOCKET_ERROR  is  returned, and  a  specific  error  code  is available in
    lpErrno.   The  error  code  WSA_IO_PENDING  indicates  that the overlapped
    operation  has  been  successfully  initiated  and  that completion will be
    indicated  at  a  later  time.   Any  other  error  code  indicates that no
    overlapped operation was initiated and no completion indication will occur.

--*/


{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    LPWSABUF         InternalBuffers;
    DWORD            InternalBufferCount;
    DWORD            InternalBytesTransfered;
    

    if (PREAPINOTIFY(( DTCODE_WSPSendTo,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &lpBuffers,
                       &lpNumberOfBytesSent,
                       &dwFlags,
                       &lpTo,
                       &iTolen,
                       &lpOverlapped,
                       &lpCompletionRoutine,
                       &lpThreadId,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        // Get Internal buffers to send down to the lower provider.
        ReturnValue = gBufferManager->AllocBuffer(
            lpBuffers,
            dwBufferCount,
            &InternalBuffers,
            &InternalBufferCount);
        
        if (NO_ERROR == ReturnValue){
            // Copy the user buffers
            ReturnValue = gBufferManager->CopyBuffer(
                lpBuffers,
                dwBufferCount,
                InternalBuffers,
                InternalBufferCount,
                &InternalBytesTransfered);
        } //if

        if (NO_ERROR == ReturnValue){
            //Is this a overlapped operation.
            if (lpOverlapped){
                
                // Setup the user overlapped struct
                lpOverlapped->Internal = WSA_IO_PENDING;
                lpOverlapped->InternalHigh = 0;
                ReturnValue = gWorkerThread->QueueOverlappedSendTo(
                    Provider,
                    ProviderSocket,
                    InternalBuffers,
                    InternalBufferCount,
                    lpNumberOfBytesSent,
                    dwFlags,
                    lpTo,
                    iTolen,
                    lpOverlapped,
                    lpCompletionRoutine,
                    lpThreadId,
                    lpErrno);
            } //if
            else{
                ReturnValue = Provider->WSPSendTo(
                    ProviderSocket,
                    lpBuffers,
                    dwBufferCount,
                    lpNumberOfBytesSent,
                    dwFlags,
                    lpTo,
                    iTolen,
                    lpOverlapped,
                    lpCompletionRoutine,
                    lpThreadId,
                    lpErrno); 
            } //else
        } //if
    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPSendTo,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &lpBuffers,
                    &lpNumberOfBytesSent,
                    &dwFlags,
                    &lpTo,
                    &iTolen,
                    &lpOverlapped,
                    &lpCompletionRoutine,
                    &lpThreadId,
                    &lpErrno));

    return(ReturnValue);
}


 INT
WSPAPI
WSPSetSockOpt(
    IN SOCKET s,
    IN INT level,
    IN INT optname,
    IN const char FAR *optval,
    IN INT optlen,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Set a socket option.

Arguments:

    s       - A descriptor identifying a socket.

    level   - The  level  at  which the option is defined; the supported levels
              include   SOL_SOCKET.   (See  annex  for  more  protocol-specific
              levels.)

    optname - The socket option for which the value is to be set.

    optval  - A  pointer  to  the  buffer  in which the value for the requested
              option is supplied.

    optlen  - The size of the optval buffer.

    lpErrno - A pointer to the error code.

Return Value:

    If  no  error  occurs, WSPSetSockOpt()  returns  0.  Otherwise, a value of
    SOCKET_ERROR  is  returned, and  a  specific  error  code  is available in
    lpErrno.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPSetSockOpt,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &level,
                       &optname,
                       &optval,
                       &optlen,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();

        ReturnValue = Provider->WSPSetSockOpt(
            ProviderSocket,
            level,
            optname,
            optval,
            optlen,
            lpErrno);
    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPSetSockOpt,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &level,
                    &optname,
                    &optval,
                    &optlen,
                    &lpErrno));

    return(ReturnValue);

}



INT
WSPAPI
WSPShutdown(
    IN SOCKET s,
    IN INT how,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Disable sends and/or receives on a socket.

Arguments:

    s       - A descriptor identifying a socket.

    how     - A  flag  that describes what types of operation will no longer be
              allowed.

    lpErrno - A pointer to the error code.

Return Value:

    If  no  error  occurs, WSPShutdown()  returns  0.   Otherwise, a value of
    SOCKET_ERROR  is  returned, and  a  specific  error  code  is available in
    lpErrno.

--*/
{
    INT ReturnValue;
    PDSOCKET     Socket;
    PDPROVIDER   Provider;
    SOCKET       ProviderSocket;
    

    if (PREAPINOTIFY(( DTCODE_WSPShutdown,
                       &ReturnValue,
                       gLibraryName,
                       &s,
                       &how,
                       &lpErrno)) ) {
        return(ReturnValue);
    }

    //
    // Get our DSOCKET object
    //
    ReturnValue = gUpCallTable.lpWPUQuerySocketHandleContext(
        s,
        (DWORD*)&Socket,
        lpErrno);
    if (SOCKET_ERROR != ReturnValue){
        Provider = Socket->GetDProvider();
        ProviderSocket = Socket->GetSocketHandle();
        DEBUGF( DBG_TRACE,
                ("Shutdown socket %X\n",s));
                
        ReturnValue = Provider->WSPShutdown(
            ProviderSocket,
            how,
            lpErrno);
    } //if
    
    POSTAPINOTIFY(( DTCODE_WSPShutdown,
                    &ReturnValue,
                    gLibraryName,
                    &s,
                    &how,
                    &lpErrno));

    return(ReturnValue);
}



SOCKET
WSPAPI
WSPSocket(
    IN int af,
    IN int type,
    IN int protocol,
    IN LPWSAPROTOCOL_INFOW lpProtocolInfo,
    IN GROUP g,
    IN DWORD dwFlags,
    OUT INT FAR *lpErrno
    )
/*++
Routine Description:

    Initialize  internal  data  and  prepare sockets for usage.  Must be called
    before any other socket routine.

Arguments:

    lpProtocolInfo - Supplies  a pointer to a WSAPROTOCOL_INFOA struct that
                     defines  the characteristics of the socket to be created.

    g              - Supplies  the identifier of the socket group which the new
                     socket is to join.

    dwFlags        - Supplies the socket attribute specification.

    lpErrno        - Returns the error code

Return Value:

    WSPSocket() returns zero if successful.  Otherwise it returns an error code
    as outlined in the SPI.

--*/
{
    INT                 ReturnValue;
    INT                 NextProviderSocket;
    DWORD               NextProviderCatalogEntryId;
    DWORD               ThisProviderCatalogEntryId;
    PPROTO_CATALOG_ITEM CatalogItem;
    PDPROVIDER          Provider;
    PDSOCKET            Socket;

    // Debug/Trace stuff
    if (PREAPINOTIFY(( DTCODE_WSPSocket,
                       &ReturnValue,
                       gLibraryName,
                       &af,
                       &type,
                       &protocol,
                       &lpProtocolInfo,
                       &g,
                       &dwFlags,
                       &lpErrno)) ) {
        return(ReturnValue);
    }


    //
    // Get the catlog entry for the next provider in the chain
    //
    ReturnValue = gProviderCatalog->FindNextProviderInChain(
        lpProtocolInfo,
        &ThisProviderCatalogEntryId,
        &NextProviderCatalogEntryId);
    
    if (NO_ERROR == ReturnValue){
    
        //
        // Get the provider for the catlog entry
        //
        ReturnValue = gProviderCatalog->GetCatalogItemFromCatalogEntryId(
            NextProviderCatalogEntryId,
            &CatalogItem);
        if (NO_ERROR ==ReturnValue){

            //
            // The the next providers WSPSocket
            //
            Provider = CatalogItem->GetProvider();

            ReturnValue = Provider->WSPSocket(
                af,
                type,
                protocol,
                CatalogItem->GetProtocolInfo(),
                g,
                dwFlags,
                lpErrno);
            
            if (ReturnValue != INVALID_SOCKET){
                NextProviderSocket = ReturnValue;
                
                //
                // Create our socket object
                //
    
                Socket = new DSOCKET;
                if (Socket){
                    
                    ReturnValue = gUpCallTable.lpWPUCreateSocketHandle(
                        ThisProviderCatalogEntryId,
                        (DWORD) Socket,
                        lpErrno);
                    DEBUGF( DBG_TRACE,
                            ("Socket Returning Socket %X\n", ReturnValue));
                    
                    if (INVALID_SOCKET != ReturnValue){
                        Socket->Initialize(
                        Provider,
                        NextProviderSocket,
                        ThisProviderCatalogEntryId,
                        ReturnValue);

                        // Add this socket to the list of sockets.
                        EnterCriticalSection(&gSocketListLock);
                        InsertHeadList(
                            &gSocketList,
                            &Socket->m_list_linkage);
                        LeaveCriticalSection(&gSocketListLock);
                    } //if
                    else{
                        delete(Socket);
                    } //else
                } //if
            } //if
        } //if
        else{
            *lpErrno = ReturnValue;
            ReturnValue = INVALID_SOCKET;
        } //else
    } //if
    else{
        *lpErrno = ReturnValue;
        ReturnValue = INVALID_SOCKET;
    } //else    
    
    // Debug/Trace stuff
    POSTAPINOTIFY(( DTCODE_WSPSocket,
                    &ReturnValue,
                    gLibraryName,
                    &af,
                    &type,
                    &protocol,
                    &lpProtocolInfo,
                    &g,
                    &dwFlags,
                    &lpErrno));

    return(ReturnValue);
}




 INT
WSPAPI
WSPStringToAddress(
    IN     LPWSTR AddressString,
    IN     INT AddressFamily,
    IN     LPWSAPROTOCOL_INFOW lpProtocolInfo,
    OUT    LPSOCKADDR lpAddress,
    IN OUT LPINT lpAddressLength,
    IN OUT LPINT lpErrno )
/*++

Routine Description:

    WSPStringToAddress() converts a human-readable string to a socket address
    structure (SOCKADDR) suitable for pass to Windows Sockets routines which
    take such a structure.  If the caller wishes the translation to be done by
    a particular provider, it should supply the corresponding WSAPROTOCOL_INFO
    struct in the lpProtocolInfo parameter. 

Arguments:

    AddressString - points to the zero-terminated human-readable string to
                    convert. 

    AddressFamily - the address family to which the string belongs.

    lpProtocolInfo - (optional) the WSAPROTOCOL_INFO struct for a particular
                     provider. 

    Address - a buffer which is filled with a single SOCKADDR structure.

    lpAddressLength - The length of the Address buffer.  Returns the size of
                      the resultant SOCKADDR structure. 

Return Value:

    The return value is 0 if the operation was successful.  Otherwise the value
    SOCKET_ERROR is returned. 

--*/
{
    INT                 ReturnValue;
    DWORD               NextProviderCatalogEntryId;
    DWORD               ThisProviderCatalogEntryId;
    PPROTO_CATALOG_ITEM CatalogItem;
    PDPROVIDER          Provider;

    if (PREAPINOTIFY(( DTCODE_WSPAddressToString,
                       &ReturnValue,
                       gLibraryName,
                       &AddressString,
                       &AddressFamily,
                       &lpProtocolInfo,
                       &lpAddress,
                       &lpAddressLength,
                       &lpErrno)) ){
        return(ReturnValue);
    } //if

    //
    // Get the catlog entry for the next provider in the chain
    //
    ReturnValue = gProviderCatalog->FindNextProviderInChain(
        lpProtocolInfo,
        &ThisProviderCatalogEntryId,
        &NextProviderCatalogEntryId);

    if (NO_ERROR == ReturnValue){

        //
        // Get the provider for the catlog entry
        //
        ReturnValue = gProviderCatalog->GetCatalogItemFromCatalogEntryId(
            NextProviderCatalogEntryId,
            &CatalogItem);
        if (NO_ERROR == ReturnValue){

            //
            // Get the provider for the catlog entry
            //
            ReturnValue = gProviderCatalog->GetCatalogItemFromCatalogEntryId(
                NextProviderCatalogEntryId,
                &CatalogItem);
            if (NO_ERROR == ReturnValue){

                //
                // The the next providers WSPSocket
                //
                Provider = CatalogItem->GetProvider();
                ReturnValue = Provider->WSPStringToAddress(
                    AddressString,
                    AddressFamily,
                    lpProtocolInfo,
                    lpAddress,
                    lpAddressLength,
                    lpErrno);
            } //if
        } //if
    } //if

    POSTAPINOTIFY(( DTCODE_WSPAddressToString,
                    &ReturnValue,
                    gLibraryName,
                    &AddressString,
                    &AddressFamily,
                    &lpProtocolInfo,
                    &lpAddress,
                    &lpAddressLength,
                    &lpErrno));

    return(ReturnValue);

}



int
WSPAPI
WSPStartup(
    WORD wVersion,
    LPWSPDATA lpWSPData,
    LPWSAPROTOCOL_INFOW lpProtocolInfo,
    WSPUPCALLTABLE UpcallTable,
    LPWSPPROC_TABLE lpProcTable )
/*++

Routine Description:

    Initiate use of a WinSock service provider by a client.
    
Arguments:

    wVersionRequested - The highest version of WinSock SPI support that the
                        caller  can use. The high order byte specifies the
                        minor version (revision) number; the low-order byte
                        specifies the major version number. 

    lpWSPData - A pointer to the WSPDATA data structure that is to receive
                details of the WinSock service provider.

    lpProtocolInfo - A pointer to a WSAPROTOCOL_INFO struct that defines the
                     characteristics of the desired protocol.  This is
                     especially useful when a single provider DLL is capable of
                     instantiating multiple different service providers.

    UpcallTable	The WinSock 2 DLLs upcall dispatch table.

    lpProcTable - A pointer to the table of SPI function pointers.



Return Value:


--*/
{
    INT ReturnCode;
    
    EnterCriticalSection(&gInitCriticalSection);
#if !defined(NODEBUG)
    debugLevel = 0xff;
#endif

    ReturnCode = NO_ERROR;
    
    if (gStartupCount == 0){

        ReturnCode = WSASYSNOTREADY;
        
        // This is the first time that WSPStartup() has been called so lets get
        // ourselves ready to do bussiness

        // Save the WinSock2 upcall table 
        gUpCallTable = UpcallTable;

        // Initialize the socket list
        InitializeListHead(&gSocketList);
        InitializeCriticalSection(&gSocketListLock);

        //
        // Init the provider catalog
        //
        gProviderCatalog = new DCATALOG;
        if (gProviderCatalog){
            ReturnCode = gProviderCatalog->Initialize();
        } //if

        //
        // Init the worker thread
        //
        if (NO_ERROR == ReturnCode){
            gWorkerThread = new DWORKERTHREAD;
            if (gWorkerThread){
                ReturnCode = gWorkerThread->Initialize();
            } //if
        } //if

        //
        // Init the buffer manager
        //
        if (NO_ERROR == ReturnCode){
            gBufferManager = new DBUFFERMANAGER;
            if (gBufferManager){
                ReturnCode = gBufferManager->Initialize(); 
            } //if
        } //if
    } //if
    // If we succeded incremant the startup count
    if (NO_ERROR == ReturnCode){
        gStartupCount++;
    } //if
    LeaveCriticalSection(&gInitCriticalSection);
    //
    // Fill in the clients proceedure table with our entry points.
    //
    
    lpProcTable->lpWSPAccept = WSPAccept;
    lpProcTable->lpWSPAddressToString = WSPAddressToString;
    lpProcTable->lpWSPAsyncSelect = WSPAsyncSelect;
    lpProcTable->lpWSPBind = WSPBind;
    lpProcTable->lpWSPCancelBlockingCall = WSPCancelBlockingCall;
    lpProcTable->lpWSPCleanup = WSPCleanup;
    lpProcTable->lpWSPCloseSocket = WSPCloseSocket;
    lpProcTable->lpWSPConnect = WSPConnect;
    lpProcTable->lpWSPDuplicateSocket = WSPDuplicateSocket;
    lpProcTable->lpWSPEnumNetworkEvents = WSPEnumNetworkEvents;
    lpProcTable->lpWSPEventSelect = WSPEventSelect;
    lpProcTable->lpWSPGetOverlappedResult = WSPGetOverlappedResult;
    lpProcTable->lpWSPGetPeerName = WSPGetPeerName;
    lpProcTable->lpWSPGetSockName = WSPGetSockName;
    lpProcTable->lpWSPGetSockOpt = WSPGetSockOpt;
    lpProcTable->lpWSPGetQOSByName = WSPGetQOSByName;
    lpProcTable->lpWSPIoctl = WSPIoctl;
    lpProcTable->lpWSPJoinLeaf = WSPJoinLeaf;
    lpProcTable->lpWSPListen = WSPListen;
    lpProcTable->lpWSPRecv = WSPRecv;
    lpProcTable->lpWSPRecvDisconnect = WSPRecvDisconnect;
    lpProcTable->lpWSPRecvFrom = WSPRecvFrom;
    lpProcTable->lpWSPSelect = WSPSelect;
    lpProcTable->lpWSPSend = WSPSend;
    lpProcTable->lpWSPSendDisconnect = WSPSendDisconnect;
    lpProcTable->lpWSPSendTo = WSPSendTo;
    lpProcTable->lpWSPSetSockOpt = WSPSetSockOpt;
    lpProcTable->lpWSPShutdown = WSPShutdown;
    lpProcTable->lpWSPSocket = WSPSocket;
    lpProcTable->lpWSPStringToAddress = WSPStringToAddress;

    // Set the version info
    lpWSPData->wVersion = MAKEWORD(2,2); 
    lpWSPData->wHighVersion = MAKEWORD(2,2);
    
    return(ReturnCode);
}
