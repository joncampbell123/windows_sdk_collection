// Socket.cpp
//
// Copyright (c)1996 Microsoft Corporation, All Right Reserved
//
// Module Description:
//
//  	This a scaled down version of POP3 Win32 SDK
//		sample code.  This sample code implements simple 
//		TCP/IP echo server functionality using multiple 
//		threads	and Completion ports.
//
//
//----------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include "echosrv.h"

BOOL					fService = TRUE;
BOOL					fTestMode = FALSE;
SERVICE_STATUS_HANDLE   hService = 0;
CHAR                    szServiceName[] = "Echo Server";
SERVICE_STATUS          ServiceStatus;

#if DBG
extern  void TestLoop(void);
#endif

SOCKET					sListener;
HANDLE					ghCompletionPort;
WSADATA					WsaData;
BOOL					bServiceTerminating = FALSE;


SERVICE_TABLE_ENTRY EchoServerTable[] = {
            {szServiceName, EchoServerMain},
            {NULL, NULL } };


//+---------------------------------------------------------------------------
//
//  Function:   DoArgs
//
//  Synopsis:   This parses the process's start arguments, basically just to
//              see if we have a -noservice flag so that we don't talk to the
//              service controller.
//
//  Arguments:  [argc] --
//              [argv] --
//
//  History:    1-09-95   RichardW   Created
//
//  Notes:
//
//----------------------------------------------------------------------------
void
DoArgs(
    int argc,
    char * argv[])
{
    int i;
    char * arg;

    for (i = 1; i < argc ; i++)
    {
        arg = argv[i];

        if (*arg == '-')
        {
            //
            // Ooo, an option.
            //
            if (_stricmp(arg, "-noservice") == 0)
            {
                fService = FALSE;
                continue;
            }
            if (_stricmp(arg, "-testmode") == 0)
            {
                fTestMode = TRUE;
                continue;
            }
        }
    }
}

//+---------------------------------------------------------------------------
//
//  Function:   DoServiceController
//
//  Synopsis:   This calls into the service controller, and is never heard
//              from again.
//
//  Arguments:  (none)
//
//  Requires:
//
//  Returns:
//
//  History:    1-09-95   RichardW   Created
//
//  Notes:
//
//----------------------------------------------------------------------------
void
DoServiceController(void)
{
    StartServiceCtrlDispatcher(EchoServerTable);
}



//+---------------------------------------------------------------------------
//
//  Function:   main
//
//  Synopsis:   Process entry point.  Unless we're told to start without
//              the service controller, we quickly wait for a service start
//              command.
//
//  Arguments:  [argc] --
//              [argv] --
//
//  Requires:
//
//  Returns:
//
//  History:    1-09-95   RichardW   Created
//
//  Notes:
//
//----------------------------------------------------------------------------
void
main (int argc, char *argv[])
{

    //
    // Parse the arguments
    //
    DoArgs(argc, argv);

    //
    // If we're a service, go to the service controller, otherwise start
    // the service immediately.
    //
    if (fService)
    {
        DoServiceController();
    }
    else
    {
        EchoServerMain(argc, NULL);
    }

}

//+---------------------------------------------------------------------------
//
//  Function:   ServiceControlHandler
//
//  Synopsis:   Handles requests from the service controller.
//
//  Arguments:  [fdwControl] -- Request code
//
//  History:    1-11-95   RichardW   Created
//
//  Notes:
//
//----------------------------------------------------------------------------
VOID
WINAPI
ServiceControlHandler(
    DWORD           fdwControl)
{
    switch (fdwControl)
    {
        case SERVICE_CONTROL_STOP:

            UpdateServiceStatus(SERVICE_STOP_PENDING);

            //
            // Remember that the service is terminating.
            //

            bServiceTerminating = TRUE;

            //
            // Close the completion port and the listening socket.
            // These actions will cause the other threads to exit.
            //

            closesocket( sListener );
            CloseHandle( ghCompletionPort );

            UpdateServiceStatus(SERVICE_STOPPED);
            return;

        case SERVICE_CONTROL_INTERROGATE:
            UpdateServiceStatus(ServiceStatus.dwCurrentState);
            return;

        default:
            return;
    }

}

//+---------------------------------------------------------------------------
//
//  Function:   NotifyServiceController
//
//  Synopsis:   Notifies the service controller of our control entry point,
//              and tells it that we're trying to start up.
//
//  Arguments:  (none)
//
//  Algorithm:
//
//  History:    1-11-95   RichardW   Created
//
//  Notes:
//
//----------------------------------------------------------------------------
BOOL
NotifyServiceController(
            VOID)
{
    if (!fService)
    {
        return(TRUE);
    }
    ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    ServiceStatus.dwWin32ExitCode = 0;
    ServiceStatus.dwServiceSpecificExitCode = 0;
    ServiceStatus.dwCheckPoint = 0;
    ServiceStatus.dwWaitHint = 0;

    hService = RegisterServiceCtrlHandler(szServiceName, ServiceControlHandler);

    if (hService)
    {
        UpdateServiceStatus(SERVICE_START_PENDING);
        return(TRUE);
    }
    else
        return(FALSE);

}

BOOL
UpdateServiceStatus(DWORD   Status)
{
    if (hService)
    {
        ServiceStatus.dwCurrentState = Status;
        if ((Status == SERVICE_START_PENDING) || (Status == SERVICE_STOP_PENDING))
        {
            ServiceStatus.dwCheckPoint ++;
            ServiceStatus.dwWaitHint = 5000;    // 5 sec.
        }
        else
        {
            ServiceStatus.dwCheckPoint = 0;
            ServiceStatus.dwWaitHint = 0;
        }

        return(SetServiceStatus(hService, &ServiceStatus));
    }

    return(FALSE);
}

void
FailServiceStart(
    DWORD           Win32Code,
    DWORD           PrivateCode)
{
    ServiceStatus.dwWin32ExitCode = Win32Code;
    ServiceStatus.dwServiceSpecificExitCode = PrivateCode;
    UpdateServiceStatus(SERVICE_STOPPED);

}

void WINAPI EchoServerMain(
    DWORD       argc,
    LPTSTR      argv[])
{
    int error;

    if (!NotifyServiceController())
    {
        return;
    }


    UpdateServiceStatus(SERVICE_START_PENDING);


    if (fTestMode)
    {
#if DBG
        TestLoop();
#endif
    }
    else
    {

        error = WSAStartup( 0x0101, &WsaData );
        if ( error == SOCKET_ERROR ) {
            printf( "WSAStartup failed.\n" );
        }

        UpdateServiceStatus(SERVICE_START_PENDING);

        //
        // Initialize the EchoServer worker threads.
        //

        ghCompletionPort = InitializeThreads(  );
        if ( ghCompletionPort == NULL ) {
            printf( "it failed.\n" );
        }

        UpdateServiceStatus(SERVICE_RUNNING);

        //
        // Start accepting and processing clients.
        //

        AcceptClients( ghCompletionPort );
    }
}
