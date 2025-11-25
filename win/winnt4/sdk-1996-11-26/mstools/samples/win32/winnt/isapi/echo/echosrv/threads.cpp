// Socket.cpp
//
// Copyright (c)1996 Microsoft Corporation, All Right Reserved
//
// Module Description:
//
// This module includes function for initializing
// a thread pool and server worker thread 
// implementation using Completion ports.
//
// Modified by ZorG from original Win32 SDK POP3 sample


#include <windows.h>
#include "echosrv.h"

extern BOOL bServiceTerminating;



HANDLE InitializeThreads ()

/*++

Routine Description:

    Starts up the server worker threads.  We use two worker threads
    per processor on the system--this is choosen as a good balance
    that ensures that there are a sufficient number of threads available
    to get useful work done but not too many that context switches
    consume significant overhead.

Arguments:


Return Value:

    HANDLE - A handle to the completion port if everything was 
        successful, or NULL if there was a failure.  

--*/

{

    DWORD i;
    HANDLE hCompletionPort;
    HANDLE hThreadHandle;
    DWORD dwThreadId;
    SYSTEM_INFO systemInfo;


    hCompletionPort = CreateIoCompletionPort( INVALID_HANDLE_VALUE, NULL, 0, 0 );
    if ( hCompletionPort == NULL ) {
        return NULL;
    }


    //
    // Determine how many processors are on the system.
    //

    GetSystemInfo( &systemInfo );

    //
    // Create worker threads that will service the actual overlapped
    // I/O requests.  Create two worker threads for each processor
    // on the system.
    //

    for ( i = 0; i < systemInfo.dwNumberOfProcessors * 2; i++ ) {

        hThreadHandle = CreateThread(
                            NULL,
                            0,
                            ServerWorkerThread,
                            hCompletionPort,
                            0,
                            &dwThreadId
                            );
        if ( hThreadHandle == NULL ) {
            CloseHandle( hCompletionPort );
            return NULL;
        }

        //
        // Close each thread handle as we open them.  We do not need
        // the thread handles.  Note that each thread will continue
        // executing.
        //

        CloseHandle( hThreadHandle );
    }

    //
    // All was successful.
    //

    return hCompletionPort;

} // InitializeThreads



DWORD WINAPI ServerWorkerThread (
    LPVOID WorkContext
    )

/*++

Routine Description:

    This is the main worker routine for the simple EchoServer worker threads.  
    Worker threads wait on a completion port for I/O to complete.  When 
    it completes, the worker thread processes the I/O, then either pends 
    new I/O or closes the client's connection.  When the service shuts 
    down, other code closes the completion port which causes 
    GetQueuedCompletionStatus() to wake up and the worker thread then 
    exits.  

Arguments:

    WorkContext - the completion port handle that will get I/O completion
        notifications.

Return Value:

    DWORD - status of the thread.

--*/

{
    HANDLE hCompletionPort = WorkContext;
    BOOL bSuccess;
    DWORD dwIoSize;
    LPOVERLAPPED lpOverlapped;
    PCLIENT_CONTEXT lpClientContext;
    
    //
    // Loop servicing I/O completions.
    //

    while ( TRUE ) {


        bSuccess = GetQueuedCompletionStatus(
                       hCompletionPort,
                       &dwIoSize,
                       (LPDWORD)&lpClientContext,
                       &lpOverlapped,
                       (DWORD)-1
                       );

        //
        // If the service is terminating, exit this thread.
        //

        if ( bServiceTerminating ) {
            return 0;
        }

        //
        // If the IO failed, close the socket and free context.
        // In addition when client closes a socket even though
		// bSuccess is set to True, dwIoSize is set to 0 and we 
		// need to check for that, otherwise we will end up with 
		// an infinite loop
		// ZorG 1-17-96

        if ( !bSuccess || (bSuccess && (0 == dwIoSize))) {
            CloseClient( lpClientContext, FALSE );
            continue;
        }

        //
        // If the request was a read, process the client request.
        //

        if ( ClientIoRead == lpClientContext->LastClientIo  ) {

            //
            // BUGBUG: if this were a real production piece of code,
            // we would check here for an incomplete read.  Because
            // TCP/IP is a stream oriented protocol, it is feasible
            // that we could receive part of a client request.
            // Therefore, we should check for the CRLF that ends a
            // client request.
            //

            // --- DavidTr: Slide 7(a) -----------------------------------
            //
            // Set up context information and perform an overlapped 
            // write on the socket.  
            //

            lpClientContext->LastClientIo = ClientIoWrite;

            bSuccess = WriteFile(
					            (HANDLE)lpClientContext->Socket,
						        lpClientContext->Buffer,
							    dwIoSize,
								&dwIoSize,
								&lpClientContext->Overlapped
                               );
            if ( !bSuccess && (ERROR_IO_PENDING != GetLastError( )) ) {
                CloseClient( lpClientContext, FALSE );
                continue;
            }

            //
            // Continue looping to get completed IO requests--we
            // do not want to pend another read now.
            //

            continue;
		}
		else if ( ClientIoWrite == lpClientContext->LastClientIo ) {

            //
            // Clean up after the WriteFile().
            //
        } 

        // Pend another read request to get the next client request.
        //

        lpClientContext->LastClientIo = ClientIoRead;
 
        bSuccess = ReadFile(
                       (HANDLE)lpClientContext->Socket,
                       lpClientContext->Buffer,
                       sizeof(lpClientContext->Buffer),
                       &dwIoSize,
                       &lpClientContext->Overlapped
                       );
        if ( !bSuccess && (ERROR_IO_PENDING != GetLastError()) ) {
            CloseClient( lpClientContext, FALSE );
            continue;
        }

        //
        // Loop around to get another completed IO request.
        //
    }

    return 0;

} // ServerWorkerThread
