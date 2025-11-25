/*++


     Copyright c 1996 Intel Corporation
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

dsocket.h

Abstract:

    This  header  defines the "DSOCKET" class.  The DSOCKET class defines state
    variables  and  operations for DSOCKET objects within the LSP.DLL.  A
    DSOCKET  object  represents  all  of the information that the LSP.DLL knows
    about a socket. 

Author:

    bugs@brandy.jf.intel.com
    
Notes:

$Revision:   1.4  $

$Modtime:   11 Jul 1996 10:57:36  $

Revision History:

most-recent-revision-date email-name
description

Original version

--*/

#ifndef _DSOCKET_
#define _DSOCKET_

#include <windows.h>
#include "llist.h"
#include "classfwd.h"

class DSOCKET
{
  public:

    static
    INT
    DSocketClassInitialize();

    static
    INT
    DSocketClassCleanup();


    DSOCKET();

    INT
    Initialize(
        IN PDPROVIDER Provider,
        IN SOCKET     ProviderSocket,
        IN DWORD      CatalogEntryId,
        IN DWORD      Context
        );

    ~DSOCKET();

    SOCKET
    GetSocketHandle();

    PDPROVIDER
    GetDProvider();

    DWORD
    GetCatalogEntryId();

    DWORD
    GetContext();

    INT
    RegisterAsyncOperation(
        HWND     Window,
        UINT     Message,
        LONG     Events
        );

    VOID
    SignalAsyncEvent(
        );

    LONG
    GetAsyncEventMask();

    HANDLE
    GetAsyncEventHandle();
    
    LIST_ENTRY  m_list_linkage;
    // Provides the linkage space for a list of DSOCKET objects maintained by
    // the  DPROCESS  object  associated with this DSOCKET object.  Note that
    // this member variable must be public so that the linked-list macros can
    // maniplate the list linkage from within the DPROCESS object's methods.

    // Note that no LIST_ENTRY is required to correspond to the DPROVIDER
    // object associated  with  this  DSOCKET object since the DPROVIDER object
    // does not  maintain a list of sockets it controls.


  private:
    
    PDPROVIDER  m_provider;
    // Reference  to  the  DPROVIDER object representing the service provider
    // that controls this socket.

    SOCKET  m_socket_handle;
    // The  external  socket  handle  value  corresponding  to  this internal
    // DSOCKET object.

    DWORD   m_catalog_entry_id;
    // The catalog entry id of the provider that this socket is attached to.

    DWORD   m_context;
    // The socket handle returned from WPUCreateSocketHandle.

    LONG    m_async_events;
    // The event mask for the events the client has registered interest in.

    HWND    m_async_window;
    // The handle of the window to receive net event messages.
    
    SOCKET  m_async_socket;
    // The socket handle for this socket. (The same as m_context)
    
    UINT    m_async_message;
    // The message to send to the client to signal net envents.
    
    HANDLE  m_net_event;
    // The handle to the WIN32 event that is used with WSPEventSelect() to
    // register interest in events contained in m_async_events.
    
};   // class DSOCKET



inline SOCKET
DSOCKET::GetSocketHandle()
/*++

Routine Description:

    Retrieves  the  external socket-handle value corresponding to this internal
    DSOCKET object.

Arguments:

    None

Return Value:

    The corresponding external socket-handle value.
--*/
{
    return(m_socket_handle);
}




inline PDPROVIDER
DSOCKET::GetDProvider()
/*++

Routine Description:

    Retrieves  a reference to the DPROVIDER object associated with this DSOCKET
    object.

Arguments:

    None

Return Value:

    The reference to the DPROVIDER object associated with this DSOCKET object.
--*/
{
    return(m_provider);
}


inline DWORD
DSOCKET::GetCatalogEntryId()
/*++

Routine Description:

    Retrieves  a reference to the DPROVIDER object associated with this DSOCKET
    object.

Arguments:

    None

Return Value:

    The reference to the DPROVIDER object associated with this DSOCKET object.
--*/
{
    return(m_catalog_entry_id);
}


inline DWORD
DSOCKET::GetContext()
/*++

Routine Description:

    Retrieves the handle for thisn socket returned from WPUCreateSocketHandle.
    
Arguments:

    None

Return Value:

    The WS2_32.DLL socket handle associated with this socket.
--*/
{
    return(m_context);
}

inline
LONG
DSOCKET::GetAsyncEventMask()
/*++

Routine Description:

    Returns the event mask for this socket
    
Arguments:

    None

Return Value:

    The event mask for this socket.
--*/
{
    return(m_async_events);
}

inline
HANDLE
DSOCKET::GetAsyncEventHandle()
/*++

Routine Description:

   Returns the WIN32 event associated with this socket.
   
Arguments:

    None

Return Value:

    The WIN32 event associated with this socket.
--*/
{
    return(m_net_event);
}

#endif // _DSOCKET_
