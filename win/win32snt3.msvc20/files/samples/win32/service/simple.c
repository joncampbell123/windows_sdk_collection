
/******************************************************************************\
*       This is a part of the Microsoft Source Code Samples.
*       Copyright (C) 1993 Microsoft Corporation.
*       All rights reserved.
*       This source code is only intended as a supplement to
*       Microsoft Development Tools and/or WinHelp documentation.
*       See these sources for detailed information regarding the
*       Microsoft samples programs.
\******************************************************************************/

///////////////////////////////////////////////////////
//
//  Service.c --
//      main program for Service sample.
//
//      This service simply opens a named pipe
//      (called \\.\pipe\simple), and reads from it.
//      It then mangles the data passed in and writes
//      the result back out to the pipe.
//
//      The simple service will respond to the basic
//      service controller functions, i.e. Start,
//      Stop, and Pause.
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

// this event is signalled when the
//  worker thread ends
//
HANDLE                  hServDoneEvent = NULL;
SERVICE_STATUS          ssStatus;       // current status of the service

SERVICE_STATUS_HANDLE   sshStatusHandle;
DWORD                   dwGlobalErr;
DWORD                   TID = 0;
HANDLE                  threadHandle = NULL;
HANDLE                  pipeHandle;


//  declare the service threads:
//
VOID    service_main(DWORD dwArgc, LPTSTR *lpszArgv);
VOID    service_ctrl(DWORD dwCtrlCode);
BOOL    ReportStatusToSCMgr(DWORD dwCurrentState,
                            DWORD dwWin32ExitCode,
                            DWORD dwCheckPoint,
                            DWORD dwWaitHint);
VOID    StopSampleService(LPTSTR lpszMsg);
VOID    die(char *reason);
VOID    worker_thread(VOID *notUsed);
VOID    StopSimpleService(LPTSTR lpszMsg);



//  main() --
//      all main does is call StartServiceCtrlDispatcher
//      to register the main service thread.  When the
//      API returns, the service has stopped, so exit.
//
VOID
main()
{
    SERVICE_TABLE_ENTRY dispatchTable[] = {
        { TEXT("SimpleService"), (LPSERVICE_MAIN_FUNCTION)service_main },
        { NULL, NULL }
    };

    if (!StartServiceCtrlDispatcher(dispatchTable)) {
        StopSimpleService("StartServiceCtrlDispatcher failed.");
    }
}



//  service_main() --
//      this function takes care of actually starting the service,
//      informing the service controller at each step along the way.
//      After launching the worker thread, it waits on the event
//      that the worker thread will signal at its termination.
//
VOID
service_main(DWORD dwArgc, LPTSTR *lpszArgv)
{
    DWORD                   dwWait;
    PSECURITY_DESCRIPTOR    pSD;
    SECURITY_ATTRIBUTES     sa;

    // register our service control handler:
    //
    sshStatusHandle = RegisterServiceCtrlHandler(
                                    TEXT("SimpleService"),
                                    (LPHANDLER_FUNCTION)service_ctrl);

    if (!sshStatusHandle)
        goto cleanup;

    // SERVICE_STATUS members that don't change in example
    //
    ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    ssStatus.dwServiceSpecificExitCode = 0;


    // report the status to Service Control Manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_START_PENDING, // service state
        NO_ERROR,              // exit code
        1,                     // checkpoint
        3000))                 // wait hint
        goto cleanup;

    // create the event object. The control handler function signals
    // this event when it receives the "stop" control code.
    //
    hServDoneEvent = CreateEvent(
        NULL,    // no security attributes
        TRUE,    // manual reset event
        FALSE,   // not-signalled
        NULL);   // no name

    if (hServDoneEvent == (HANDLE)NULL)
        goto cleanup;

    // report the status to the service control manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_START_PENDING, // service state
        NO_ERROR,              // exit code
        2,                     // checkpoint
        3000))                 // wait hint
        goto cleanup;

    // create a security descriptor that allows anyone to write to
    //  the pipe...
    //
    pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR,
                SECURITY_DESCRIPTOR_MIN_LENGTH);

    if (pSD == NULL) {
        StopSimpleService("LocalAlloc pSD failed");
        return;
    }

    if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) {
        StopSimpleService("InitializeSecurityDescriptor failed");
        LocalFree((HLOCAL)pSD);
        return;
    }

    // add a NULL disc. ACL to the security descriptor.
    //
    if (!SetSecurityDescriptorDacl(pSD, TRUE, (PACL) NULL, FALSE)) {
        StopSimpleService("SetSecurityDescriptorDacl failed");
        LocalFree((HLOCAL)pSD);
        return;
    }

    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = pSD;
    sa.bInheritHandle = TRUE;       // why not...

    // open our named pipe...
    //
    pipeHandle = CreateNamedPipe(
                    "\\\\.\\pipe\\simple",  // name of pipe
                    PIPE_ACCESS_DUPLEX,     // pipe open mode
                    PIPE_TYPE_MESSAGE |
                    PIPE_READMODE_MESSAGE |
                    PIPE_WAIT,              // pipe IO type
                    1,                      // number of instances
                    0,                      // size of outbuf (0 == allocate as necessary)
                    0,                      // size of inbuf
                    1000,                   // default time-out value
                    &sa);                   // security attributes

    if (!pipeHandle) {
        StopSimpleService("CreateNamedPipe");
        LocalFree((HLOCAL)pSD);
        return;
    }

    // start the thread that performs the work of the service.
    //
    threadHandle = (HANDLE)_beginthread(
                    worker_thread,
                    4096,       // stack size
                    NULL);      // argument to thread

    if (!threadHandle)
        goto cleanup;

    // report the status to the service control manager.
    //
    if (!ReportStatusToSCMgr(
        SERVICE_RUNNING, // service state
        NO_ERROR,        // exit code
        0,               // checkpoint
        0))              // wait hint
        goto cleanup;

    // wait indefinitely until hServDoneEvent is signaled.
    //
    dwWait = WaitForSingleObject(
        hServDoneEvent,  // event object
        INFINITE);       // wait indefinitely

cleanup:

    if (hServDoneEvent != NULL)
        CloseHandle(hServDoneEvent);


    // try to report the stopped status to the service control manager.
    //
    if (sshStatusHandle != 0)
        (VOID)ReportStatusToSCMgr(
                            SERVICE_STOPPED,
                            dwGlobalErr,
                            0,
                            0);

    // When SERVICE MAIN FUNCTION returns in a single service
    // process, the StartServiceCtrlDispatcher function in
    // the main thread returns, terminating the process.
    //
    return;
}



//  service_ctrl() --
//      this function is called by the Service Controller whenever
//      someone calls ControlService in reference to our service.
//
VOID
service_ctrl(DWORD dwCtrlCode)
{
    DWORD  dwState = SERVICE_RUNNING;

    // Handle the requested control code.
    //
    switch(dwCtrlCode) {

        // Pause the service if it is running.
        //
        case SERVICE_CONTROL_PAUSE:

            if (ssStatus.dwCurrentState == SERVICE_RUNNING) {
                SuspendThread(threadHandle);
                dwState = SERVICE_PAUSED;
            }
            break;

        // Resume the paused service.
        //
        case SERVICE_CONTROL_CONTINUE:

            if (ssStatus.dwCurrentState == SERVICE_PAUSED) {
                ResumeThread(threadHandle);
                dwState = SERVICE_RUNNING;
            }
            break;

        // Stop the service.
        //
        case SERVICE_CONTROL_STOP:

            dwState = SERVICE_STOP_PENDING;

            // Report the status, specifying the checkpoint and waithint,
            //  before setting the termination event.
            //
            ReportStatusToSCMgr(
                    SERVICE_STOP_PENDING, // current state
                    NO_ERROR,             // exit code
                    1,                    // checkpoint
                    3000);                // waithint

            SetEvent(hServDoneEvent);
            return;

        // Update the service status.
        //
        case SERVICE_CONTROL_INTERROGATE:
            break;

        // invalid control code
        //
        default:
            break;

    }

    // send a status response.
    //
    ReportStatusToSCMgr(dwState, NO_ERROR, 0, 0);
}



//  worker_thread() --
//      this function does the actual nuts and bolts work that
//      the service requires.  It will also Pause or Stop when
//      asked by the service_ctrl function.
//
VOID
worker_thread(VOID *notUsed)
{
    char                    inbuf[80];
    char                    outbuf[80];
    BOOL                    ret;
    DWORD                   bytesRead;
    DWORD                   bytesWritten;

    // okay, our pipe has been creating, let's enter the simple
    //  processing loop...
    //
    while (1) {

        // wait for a connection...
        //
        ConnectNamedPipe(pipeHandle, NULL);

        // grab whatever's coming through the pipe...
        //
        ret = ReadFile(
                    pipeHandle,     // file to read from
                    inbuf,          // address of input buffer
                    sizeof(inbuf),  // number of bytes to read
                    &bytesRead,     // number of bytes read
                    NULL);          // overlapped stuff, not needed

        if (!ret)
            // pipe's broken... go back and reconnect
            //
            continue;

        // munge the string
        //
        sprintf(outbuf, "Hello! [%s]", inbuf);

        // send it back out...
        //
        ret = WriteFile(
                    pipeHandle,     // file to write to
                    outbuf,         // address of output buffer
                    sizeof(outbuf), // number of bytes to write
                    &bytesWritten,  // number of bytes written
                    NULL);          // overlapped stuff, not needed

        if (!ret)
            // pipe's broken... go back and reconnect
            //
            continue;

        // drop the connection...
        //
        DisconnectNamedPipe(pipeHandle);
    }
}



// utility functions...



// ReportStatusToSCMgr() --
//      This function is called by the ServMainFunc() and
//      ServCtrlHandler() functions to update the service's status
//      to the service control manager.
//
BOOL
ReportStatusToSCMgr(DWORD dwCurrentState,
                    DWORD dwWin32ExitCode,
                    DWORD dwCheckPoint,
                    DWORD dwWaitHint)
{
    BOOL fResult;

    // Disable control requests until the service is started.
    //
    if (dwCurrentState == SERVICE_START_PENDING)
        ssStatus.dwControlsAccepted = 0;
    else
        ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |
            SERVICE_ACCEPT_PAUSE_CONTINUE;

    // These SERVICE_STATUS members are set from parameters.
    //
    ssStatus.dwCurrentState = dwCurrentState;
    ssStatus.dwWin32ExitCode = dwWin32ExitCode;
    ssStatus.dwCheckPoint = dwCheckPoint;

    ssStatus.dwWaitHint = dwWaitHint;

    // Report the status of the service to the service control manager.
    //
    if (!(fResult = SetServiceStatus(
                sshStatusHandle,    // service reference handle
                &ssStatus))) {      // SERVICE_STATUS structure

        // If an error occurs, stop the service.
        //
        StopSimpleService("SetServiceStatus");
    }
    return fResult;
}



// The StopSimpleService function can be used by any thread to report an
//  error, or stop the service.
//
VOID
StopSimpleService(LPTSTR lpszMsg)
{
    CHAR    chMsg[256];
    HANDLE  hEventSource;
    LPTSTR  lpszStrings[2];

    dwGlobalErr = GetLastError();

    // Use event logging to log the error.
    //
    hEventSource = RegisterEventSource(NULL,
                            TEXT("SimpleService"));

    sprintf(chMsg, "SimpleService error: %d", dwGlobalErr);
    lpszStrings[0] = chMsg;
    lpszStrings[1] = lpszMsg;

    if (hEventSource != NULL) {
        ReportEvent(hEventSource, // handle of event source
            EVENTLOG_ERROR_TYPE,  // event type
            0,                    // event category
            0,                    // event ID
            NULL,                 // current user's SID
            2,                    // strings in lpszStrings
            0,                    // no bytes of raw data
            lpszStrings,          // array of error strings
            NULL);                // no raw data

        (VOID) DeregisterEventSource(hEventSource);
    }

    // Set a termination event to stop SERVICE MAIN FUNCTION.
    //
    SetEvent(hServDoneEvent);
}
