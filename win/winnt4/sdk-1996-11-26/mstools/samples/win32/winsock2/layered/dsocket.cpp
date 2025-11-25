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

dsocket.cpp

Abstract:

This module contains the implemetation of the dsocket object used
by lsp.dll

Author:

    bugs@brandy.jf.intel.com

Notes:

$Revision:   1.5  $

$Modtime:   11 Jul 1996 10:56:58  $

Revision History:

--*/

#include "precomp.h"
#include "globals.h"



DSOCKET::DSOCKET(
    )
/*++

Routine Description:

    DSOCKET  object  constructor.   Creates and returns a DSOCKET object.  Note
    that  the  DSOCKET object has not been fully initialized.  The "Initialize"
    member function must be the first member function called on the new DSOCKET
    object.

Arguments:

    None

Return Value:

    None
--*/
{
    // Set our data members to known values
    m_provider          = NULL;
    m_socket_handle     = (SOCKET)SOCKET_ERROR;
    m_catalog_entry_id  = NULL;
    m_context           = INVALID_SOCKET;
    m_async_events      = NULL;
    m_async_window      = NULL;
    m_async_socket      = INVALID_SOCKET;
    m_async_message     = NULL;
    m_net_event         = NULL;
}




INT
DSOCKET::    Initialize(
        IN PDPROVIDER Provider,
        IN SOCKET     ProviderSocket,
        IN DWORD      CatalogEntryId,
        IN DWORD      Context
        )
/*++

Routine Description:

    Completes  the  initialization  of  the  DSOCKET object.  This must be the
    first  member  function  called  for  the  DSOCKET object.  This procedure
    should be called only once for the object.

Arguments:

    Provider - Supplies  a  reference  to  the DPROVIDER object associated with
               this DSOCKET object.
               
    ProviderSocket - The socket handle returned from the lower level provider.

    CatalogEntryId - The CatalogEntryId for the provider referenced by
                     m_provider.

    Context        - The socket handle returned from WPUCreateSocketHandle().
    
Return Value:

    The  function returns ERROR_SUCCESS if successful.  Otherwise it
    returns an appropriate WinSock error code if the initialization
    cannot be completed.
--*/
{
    // Store the provider and process object.
    m_provider = Provider;
    m_socket_handle = ProviderSocket;
    m_catalog_entry_id = CatalogEntryId;
    m_context = Context;
    
    DEBUGF( DBG_TRACE,
            ("Initializing socket %X\n",this));
    return(ERROR_SUCCESS);
}



DSOCKET::~DSOCKET()
/*++

Routine Description:

    DSOCKET  object  destructor.   This  procedure  has  the  responsibility to
    perform  any required shutdown operations for the DSOCKET object before the
    object  memory  is  deallocated.
    
Arguments:

    None

Return Value:

    None
--*/
{
    
    gWorkerThread->UnregisterSocket(
            this);
    DEBUGF( DBG_TRACE,
            ("Destroying socket %X\n",this));
}


INT
DSOCKET::RegisterAsyncOperation(
    HWND     Window,
    UINT     Message,
    LONG     Events
    )
/*++

Routine Description:

    Registers interest in net work events.
    
Arguments:

    Window  - The handle to the window that will receive notification of
              network events. 

    Message - The message to send for net event notification.

    Events  - The events to be registered.
    
Return Value:

    NO_ERROR on success else a valid winsock errorcode.
    
--*/
{
    INT ReturnCode;

    ReturnCode = WSAENOBUFS;
    
    if (Events){
        //Create a WIN32 event to hand to the worker thread.
        if (NULL == m_net_event){
            m_net_event =  CreateEvent(
                NULL,
                TRUE,
                FALSE,
                NULL);
        } //if

        // Write down the user request
        if (m_net_event){
            m_async_window  = Window;
            m_async_socket  = m_context;
            m_async_message = Message;
            m_async_events  = Events;

            // Register this socket with the worker thread.
            ReturnCode = gWorkerThread->RegisterSocket(
                this);
        } //if        
    } //if
    else{
        DEBUGF( DBG_TRACE,
                ("Unegistering socket %X\n",this));
        m_async_window  = NULL;
        m_async_socket  = NULL;
        m_async_message = NULL;
        m_async_events  = NULL;

        ReturnCode = gWorkerThread->UnregisterSocket(
            this);
    } //else
    return(ReturnCode);
}


VOID
DSOCKET::SignalAsyncEvent(
    )
/*++

Routine Description:

     The notification function called by the worker thread to signal network
     events. 

Arguments:

    None

Return Value:

    None
--*/
{
    WSANETWORKEVENTS  Events;
    INT               ErrorCode;

    ErrorCode = NO_ERROR;
    assert(this != NULL);

    // Find out what happend.
    m_provider->WSPEnumNetworkEvents(
        m_socket_handle,
        m_net_event,
        //NULL,
        &Events,
        &ErrorCode);
     
    if (NO_ERROR == ErrorCode){

        //
        // Signal all the valid events
        //
        
        if (Events.lNetworkEvents & FD_READ & m_async_events){
            PostMessage(
                m_async_window,
                m_async_message,
                m_context,
                MAKELONG( FD_READ,Events.iErrorCode[FD_READ_BIT] ));
        
        } //if
    
        if (Events.lNetworkEvents & FD_WRITE & m_async_events){
            PostMessage(
                m_async_window,
                m_async_message,
                m_context,
                MAKELONG( FD_WRITE,Events.iErrorCode[FD_WRITE_BIT] ));
        } //If

        if (Events.lNetworkEvents & FD_OOB & m_async_events){
            PostMessage(
                m_async_window,
                m_async_message,
                m_context,
                MAKELONG( FD_OOB,Events.iErrorCode[FD_OOB_BIT] ));
        } //if
    
        if (Events.lNetworkEvents & FD_ACCEPT & m_async_events){
            PostMessage(
                m_async_window,
                m_async_message,
                m_context,
                MAKELONG( FD_ACCEPT,Events.iErrorCode[FD_ACCEPT_BIT] ));
        } //if
    
        if (Events.lNetworkEvents & FD_CONNECT & m_async_events){
            PostMessage(
                m_async_window,
                m_async_message,
                m_context,
                MAKELONG( FD_CONNECT,Events.iErrorCode[FD_CONNECT_BIT] ));
        } //if
    
        if (Events.lNetworkEvents & FD_CLOSE & m_async_events){
            PostMessage(
                m_async_window,
                m_async_message,
                m_context,
                MAKELONG( FD_CLOSE,Events.iErrorCode[FD_CLOSE_BIT] ));
        } //if
    
        if (Events.lNetworkEvents & FD_QOS & m_async_events){
            PostMessage(
                m_async_window,
                m_async_message,
                m_context,
                MAKELONG( FD_QOS,Events.iErrorCode[FD_QOS_BIT] ));
        } //if
    
        if (Events.lNetworkEvents & FD_GROUP_QOS & m_async_events){
            PostMessage(
                m_async_window,
                m_async_message,
                m_context,
                MAKELONG( FD_GROUP_QOS,Events.iErrorCode[FD_GROUP_QOS_BIT] ));
        } //if
    } //if
}
