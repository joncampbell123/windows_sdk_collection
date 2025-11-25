/*************************************************************************\
*  PROGRAM: OvComm.c
*
*  PURPOSE:
*
*     To demonstrate overlapped serial communication in multiple threads
*
*  FUNCTIONS:
*
*    main()         - Setups up console mode
*    ProcessIO      - Comm Port Setup, thread creation and cleanup
*
*  GLOBAL VARIABLES:
*
*    - comHandle      : Handle to comm device
*    - hSetupEvent    : Handle of the event signalling setup completion
*    - bDone          : Boolean controlling execution of threads
*    - lpszPort       : String containing port name
*    - lpszSerialSpec : String containing serial specification
*    - g_timeouts     : Original COMMTIMEOUTS to be restored when closing
*
*  COMMENTS:
*
*    To Use:
*       Start the application from the command line.  The application will
*       by default use COM1 9600,N,8,1.  If desired, the comm port and
*       serial specification can be supplied on the command line.
*       The application uses the console to display received data.  
*       Keyboard input from the console's standard input handle
*       is used to get data to send to the comm port.
*
*    What Happens:
*       The program utilizes 4 threads.  A main thread initializes the comm
*       port and console, creates 3 worker threads and waits for the threads
*       to terminate.  Each worker thread controls a single facet of 
*       serial communications.
*
*       A Status Thread is created to monitor status events.
*       When an event occurs, the event is communicated to the user via 
*       OutputDebugString.
*
*       A Reader Thread is created to read the comm port and display the
*       received data on the console output.
*       
*       A Writer Thread is created to accept keyboard input and send it
*       out the comm port.
*
*       Control-Break causes program termination.
*
\*************************************************************************/

#include <windows.h>
#include <stdio.h>

#include "OvComm.h"


/*************************************************************************\
*
*  FUNCTION: ErrorReporter(char *)
*
*  PURPOSE: Report error to user
*
*  COMMENTS: Reports error string in console and in debugger
*
\*************************************************************************/
void ErrorReporter(char * szMessage)
{
	DWORD dwErr;
	char * szTemp;
    char * szFormat = "Error %d: %s.\n\r";

    dwErr = GetLastError();

	// allocate a temporary buffer
	szTemp = malloc(strlen(szMessage) + 20);

	if (szTemp == NULL)	// if no buffer, then printf message
		printf(szFormat, dwErr, szMessage);
	else {	
		wsprintf(szTemp, szFormat, dwErr, szMessage);
		printf(szTemp);		        // printf message on console
		OutputDebugString(szTemp);	// and debugger
		free(szTemp);			    // free buffer
	}

    return;
}


/*************************************************************************\
*
*  FUNCTION: ErrorHandler(char *)
*
*  PURPOSE: Handle a fatal error
*
*  COMMENTS: Calls ErrorReporter function and then exits.
*            This function is used to report fatal errors before the comm
*            port is opened.
*
\*************************************************************************/
void ErrorHandler(char * szMessage)
{	
    ErrorReporter(szMessage);
	exit(0);
}


/*************************************************************************\
*
*  FUNCTION: ErrorInComm(char *)
*
*  PURPOSE: Handle a fatal error after comm has started
*
*  COMMENTS: Calls ErrorReporter, BreakDownCommPort and exits.
*
\*************************************************************************/
void ErrorInComm(char * szMessage)
{
	ErrorReporter(szMessage);
    BreakDownCommPort();
    exit(0);
}


/*************************************************************************\
*
*  FUNCTION: SetupCommPort( void )
*
*  PURPOSE: Setup Communication Port with our settings
*
*  COMMENTS:
*
\*************************************************************************/
HANDLE SetupCommPort( void )
{
	COMMTIMEOUTS timeouts;
	DCB dcb;

	// sync event for controlling thread startup
	hSetupEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hSetupEvent == NULL)
		ErrorHandler("CreateEvent(Setup Event)");

	// communication handle
	comHandle = CreateFile( lpszPort, 
	                        GENERIC_READ | GENERIC_WRITE, 
	                        0, 
	                        0, 
	                        OPEN_EXISTING,
							FILE_FLAG_OVERLAPPED,
							0);

	if (comHandle == INVALID_HANDLE_VALUE) {   
        printf("Problem opening port: %s\n", lpszPort);
		ErrorHandler("CreateFile");
    }
	
    FillMemory(&dcb, sizeof(dcb), 0);
    dcb.DCBlength = sizeof(dcb);
    if (!BuildCommDCB(lpszSerialSpec, &dcb)) {   
        printf("Problem with serial spec line: %s\n", lpszSerialSpec);
        ErrorInComm("BuildCommDCB");
    }

    dcb.EvtChar = EVT_CHAR;

	if (!SetCommState(comHandle, &dcb))
		ErrorInComm("SetCommState");

    // Save original comm timeouts and set new ones
    if (!GetCommTimeouts(comHandle, &g_timeouts))
        ErrorInComm("GetCommTimeouts");

    // Need ReadIntervalTimeout here to cause the read operations
    // that we do to actually timeout and become overlapped.
    // Specifying 1 here causes ReadFile to return very quickly
    // so that our reader thread will continue execution after a ReadFile
    // is overlapped.
    // Specifying MAXDWORD here causes ReadFile to return immediately
    // without overlapping.
    timeouts.ReadIntervalTimeout = 1; 
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 0;
	timeouts.WriteTotalTimeoutConstant = 0;

	if (!SetCommTimeouts(comHandle, &timeouts))
		ErrorInComm("SetCommTimeouts");

    // raise DTR
	if (!EscapeCommFunction(comHandle, SETDTR))
		ErrorInComm("EscapeCommFunction (SETDTR)");

    // set initial thread state: NOT DONE
	bDone = FALSE;

	return comHandle;
}


/*************************************************************************\
*
*  FUNCTION: BreakDownCommPort( void )
*
*  PURPOSE: Clean-up and close comm port
*
*  COMMENTS:
*
\*************************************************************************/
void BreakDownCommPort( void )
{
    // lower DTR
	if (!EscapeCommFunction(comHandle, CLRDTR))
		ErrorReporter("EscapeCommFunction(CLRDTR)");

    // restore original comm timeouts
    if (!SetCommTimeouts(comHandle, &g_timeouts))
        ErrorReporter("SetCommTimeouts (Restoration to original)");

	if (!PurgeComm(comHandle, PURGE_FLAGS))
        ErrorReporter("PurgeComm");

    CloseHandle(comHandle);
}


/*************************************************************************\
*
*  FUNCTION: ReportModemStatus(DWORD)
*
*  PURPOSE: Report modem status line states
*
*  COMMENTS: Upper case means the signal is high
*
\*************************************************************************/
void ReportModemStatus(DWORD dwModemStatus)
{
	BOOL bCTS, bDSR, bRING, bRLSD;
    char szMessage[35];

	bCTS = MS_CTS_ON & dwModemStatus;
	bDSR = MS_DSR_ON & dwModemStatus;
	bRING = MS_RING_ON & dwModemStatus;
	bRLSD = MS_RLSD_ON & dwModemStatus;

    strcpy(szMessage, "MODEM STATUS: ");
    strcat(szMessage, bCTS ? "CTS " : "cts ");
	strcat(szMessage, bDSR ? "DSR " : "dsr ");
	strcat(szMessage, bRING ? "RING " : "ring ");
	strcat(szMessage, bRLSD ? "RLSD " : "rlsd ");
    strcat(szMessage,"\n\r");

    OutputDebugString(szMessage);	

    return;
}


/*************************************************************************\
*
*  FUNCTION: CheckModemStatus ( void )
*
*  PURPOSE: Check new status, report if changed
*
*  COMMENTS:
*
\*************************************************************************/
void CheckModemStatus( void )
{
	static DWORD dwOldStatus = 0;
	DWORD dwNewModemStatus;

	if (!GetCommModemStatus(comHandle, &dwNewModemStatus))
		ErrorReporter("GetCommModemStatus");

	if (dwNewModemStatus != dwOldStatus) {  // status changed?
        ReportModemStatus(dwNewModemStatus);
        dwOldStatus = dwNewModemStatus;
    }

    return;
}


/*************************************************************************\
*
*  FUNCTION: ReportCommError( void )
*
*  PURPOSE: Call ClearCommError and report the results
*
*  COMMENTS: This function is called if an EV_ERR occurs.
*
\*************************************************************************/
void ReportCommError( void )
{
    COMSTAT comStat;
    DWORD   dwErrors;
    char    szMessage[100];
    BOOL    bBREAK, bDNS, bFRAME, bIOE, bMODE;
    BOOL    bOOP, bOVERRUN, bPTO, bRXOVER, bRXPARITY, bTXFULL;

    if (!ClearCommError(comHandle, &dwErrors, &comStat))
        ErrorInComm("ClearCommError");

    bDNS = dwErrors & CE_DNS;
    bIOE = dwErrors & CE_IOE;
    bOOP = dwErrors & CE_OOP;
    bPTO = dwErrors & CE_PTO;
    bMODE = dwErrors & CE_MODE;
    bBREAK = dwErrors & CE_BREAK;
    bFRAME = dwErrors & CE_FRAME;
    bRXOVER = dwErrors & CE_RXOVER;
    bTXFULL = dwErrors & CE_TXFULL;
    bOVERRUN = dwErrors & CE_OVERRUN;
    bRXPARITY = dwErrors & CE_RXPARITY;

    strcpy(szMessage, "ERROR: ");
    strcat(szMessage, bDNS ? "DNS " : "");
    strcat(szMessage, bIOE ? "IOE " : "");
    strcat(szMessage, bOOP ? "OOP " : "");
    strcat(szMessage, bPTO ? "PTO " : "");
    strcat(szMessage, bMODE ? "MODE " : "");
    strcat(szMessage, bBREAK ? "BREAK " : "");
    strcat(szMessage, bFRAME ? "FRAME " : "");
    strcat(szMessage, bRXOVER ? "RXOVER " : "");
    strcat(szMessage, bTXFULL ? "TXFULL " : "");
    strcat(szMessage, bOVERRUN ? "OVERRUN " : "");
    strcat(szMessage, bRXPARITY ? "RXPARITY " : "");
    strcat(szMessage, "\n\r");

    OutputDebugString(szMessage);

    if (comStat.fCtsHold)
        OutputDebugString("Tx waiting for CTS signal\n\r");

    if (comStat.fDsrHold)
        OutputDebugString("Tx waiting for DSR signal\n\r");

    if (comStat.fRlsdHold)
        OutputDebugString("Tx waiting for RLSD signal\n\r");

    if (comStat.fXoffHold)
        OutputDebugString("Tx waiting, XOFF char rec'd\n\r");

    if (comStat.fXoffSent)
        OutputDebugString("Tx waiting, XOFF char sent\n\r");
    
    if (comStat.fEof)
        OutputDebugString("EOF character received\n\r");
    
    if (comStat.fTxim)
        OutputDebugString("Character waiting for Tx\n\r");

    if (comStat.cbInQue) {
        sprintf(szMessage, "%d bytes in input buffer\n\r", comStat.cbInQue);
        OutputDebugString(szMessage);
    }

    if (comStat.cbOutQue) {
        sprintf(szMessage, "%d bytes in output buffer\n\r", comStat.cbOutQue);
        OutputDebugString(szMessage);
    }

    return;
}

/*************************************************************************\
*
*  FUNCTION: ReportStatusEvent (DWORD)
*
*  PURPOSE: Report a comm status event
*
*  COMMENTS: This is different than line status (aka Modem Status).
*
\*************************************************************************/
void ReportStatusEvent(DWORD dwStatus)
{
	BOOL bBREAK, bCTS, bDSR, bERR;
	BOOL bRING, bRLSD, bRXCHAR, bRXFLAG, bTXEMPTY;
    char szMessage[70];

	bCTS = EV_CTS & dwStatus;
	bDSR = EV_DSR & dwStatus;
	bERR = EV_ERR & dwStatus;
	bRING = EV_RING & dwStatus;
	bRLSD = EV_RLSD & dwStatus;
	bBREAK = EV_BREAK & dwStatus;
	bRXCHAR = EV_RXCHAR & dwStatus;
    bRXFLAG = EV_RXFLAG & dwStatus;
	bTXEMPTY = EV_TXEMPTY & dwStatus;

    strcpy(szMessage, "EVENT: ");
    strcat(szMessage, bCTS ? "CTS " : "");
	strcat(szMessage, bDSR ? "DSR " : "");
	strcat(szMessage, bERR ? "ERR " : "");
	strcat(szMessage, bRING ? "RING " : "");
	strcat(szMessage, bRLSD ? "RLSD " : "");
	strcat(szMessage, bBREAK ? "BREAK " : "");
    strcat(szMessage, bRXFLAG ? "RXFLAG " : "");
	strcat(szMessage, bRXCHAR ? "RXCHAR " : "");
	strcat(szMessage, bTXEMPTY ? "TXEMPTY " : "");
    strcat(szMessage, "\n\r");

    OutputDebugString(szMessage);

    if (bERR)
        ReportCommError();

	CheckModemStatus();

	return;
}


/*************************************************************************\
*
*  FUNCTION: StatusProc (LPVOID)
*
*  PURPOSE: Thread function to control comm status checking
*
*  COMMENTS: Events that occur more than once in a while, like
*            EV_RXCHAR, EV_TXEMPTY or event EV_RXFLAG may only be
*            reported once.  The reason is that subsequent occurrences
*            of an as yet unreported event simply trigger an already
*            triggered flag.  Events are not queued up like Window
*            Messages.
*
\*************************************************************************/
DWORD WINAPI StatusProc(LPVOID lpV)
{
	DWORD	   dwWaitResult, dwCommEvent;
	HANDLE 	   hEvent;
	BOOL	   bWaitingOnStatusHandle = FALSE;
	OVERLAPPED os = {0};

	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEvent == NULL)
		ErrorInComm("CreateEvent(Status)");

	os.hEvent = hEvent;	

	if (!SetCommMask(comHandle, EVENT_FLAGS))
		ErrorInComm("SetCommMask");

    // initial check before everything else gets going
    CheckModemStatus();

    // This event signals that the comm mask has been setup and initial
    // modem status reported.
    // This prevents any reads/writes until the program is ready to
    // start reporting events.
    if (!SetEvent(hSetupEvent))
		ErrorInComm("SetEvent (Setup Event)");

	// wait for status changes to occur and report them every so often
	for ( ; !bDone ; Sleep(STATUS_CHECK_TIMEOUT))
	{
		// check and possibly report modem status
		CheckModemStatus();

		// check and report event status
		if (bWaitingOnStatusHandle)	{     // waiting on previous WaitCommEvent
			dwWaitResult = WaitForSingleObject(hEvent, 0);	// check event
			if (dwWaitResult == WAIT_OBJECT_0) {            //  event signalled
                ReportStatusEvent(dwCommEvent);
                bWaitingOnStatusHandle = FALSE;
                ResetEvent(hEvent);
            }
        }

        if (!bWaitingOnStatusHandle) {   	// previous event isn't pending?
        	// wait for another (or first) event
            if (!WaitCommEvent(comHandle, &dwCommEvent, &os)) {
                if (GetLastError() != ERROR_IO_PENDING)	// is Wait pending?
                    ErrorInComm("WaitCommEvent");
                    
                bWaitingOnStatusHandle = TRUE;
            }
            else	
                // WaitCommEvent returned immediately
                ReportStatusEvent(dwCommEvent); 
        }
    }

    CloseHandle(hEvent);
    OutputDebugString("Status Thread Exiting\n\r");

    return 1;
}


/*************************************************************************\
*
*  FUNCTION: OutputABuffer (HANDLE, char *, DWORD)
*
*  PURPOSE: OutputABuffer to the console standard out
*
*  COMMENTS: If buffer is 0 length, then do nothing.
*
\*************************************************************************/
void OutputABuffer(HANDLE hOut, char * lpBuf, DWORD dwBufLen)
{
	DWORD dwWritten;

	if (dwBufLen != 0) {
		if (!WriteFile(hOut, lpBuf, dwBufLen, &dwWritten, NULL))
			ErrorReporter("WriteFile (console) in Reader");
	}
    else
        OutputDebugString("Attempt to write empty buffer\n\r");
	return;
}


/*************************************************************************\
*
*  FUNCTION: ReaderProc (LPVOID)
*
*  PURPOSE: Thread function controls comm port reading and console output
*
*  COMMENTS:
*
\*************************************************************************/
DWORD WINAPI ReaderProc(LPVOID lpV)
{
	BOOL 	   bWaitingOnRead = FALSE;
	char 	   lpBuf[MAX_READ_BUFFER];
	DWORD 	   dwRead, dwRes;
	HANDLE 	   hReadEvent;
	OVERLAPPED ov = {0};
	HANDLE 	   hOut;	// std output

	hOut = (HANDLE) lpV;

	hReadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hReadEvent == NULL)
		ErrorInComm("CreateEvent (ReadEvent)");

	ov.hEvent = hReadEvent;

	while (!bDone)
	{
		if (bWaitingOnRead) {
		    // wait for read to finish
			dwRes = WaitForSingleObject(hReadEvent, READ_TIMEOUT);	
			switch(dwRes)
			{
				case WAIT_TIMEOUT:  		// read timed out
                    // timeouts are not reported because they happen too often
                    // take comments out of next line to report read timeouts
                    // OutputDebugString("Read timeout\n\r");
					break;
			
				case WAIT_OBJECT_0:			// read completed
					if (!GetOverlappedResult(comHandle, &ov, &dwRead, TRUE)) {
                        if (GetLastError() == ERROR_OPERATION_ABORTED)
                            OutputDebugString("Read aborted\n\r");
                        else
                        	ErrorInComm("GetOverlappedResult (in Reader)");
                    }
					else {
						// read completed successfully
						bWaitingOnRead = FALSE;
						OutputABuffer(hOut, lpBuf, dwRead);
						ResetEvent(hReadEvent);	  // reset event for next read
					}

					break;
			
				default:					// wait on event failed
						ErrorReporter("WaitForSingleObject(hReadEvent)");
						break;
			}
		}

		if (!bWaitingOnRead) {
            // issue read
			if (!ReadFile(comHandle, lpBuf, MAX_READ_BUFFER, &dwRead, &ov)) {
			    // read failed
				if (GetLastError() != ERROR_IO_PENDING)	  // read not delayed?
					ErrorInComm("ReadFile (comHandle) in Reader");

				bWaitingOnRead = TRUE;
			}
			else {    // read completed immediately
				if (dwRead != 0)
					OutputABuffer(hOut, lpBuf, dwRead);
                else
                    OutputDebugString("Read 0 bytes and didn't overlap.\n\r");
			}
		}
	}

    CloseHandle(hReadEvent);
	OutputDebugString("Reader Thread Exiting\n");

	return 1;
}


/*************************************************************************\
*
*  FUNCTION: WriterProc(LPVOID)
*
*  PURPOSE: Thread function controls console input and comm port writing
*
*  COMMENTS:
*
\*************************************************************************/
DWORD WINAPI WriterProc(LPVOID lpV)
{
	HANDLE     hIn;
	char       chRead;
	DWORD      dwRead, dwWritten;
	OVERLAPPED ov = {0};

	hIn = (HANDLE) lpV;

	while (!bDone)
	{
		// block the console read until a character key is pressed
		if (!ReadFile(hIn, &chRead, 1, &dwRead, NULL))
			ErrorInComm("ReadFile (std in) in Writer Thread");

		// if char is actually read from the keyboard
		if (dwRead == 1) {
            // send character out comm port
			if (!WriteFile(comHandle, &chRead, 1, &dwWritten, &ov)) {
				if (GetLastError() == ERROR_IO_PENDING) {  // write is delayed
					// check write status and wait til complete. 
					// can't do another write until the first one finishes.
					if (!GetOverlappedResult(comHandle, &ov, &dwWritten, TRUE))
						ErrorInComm("GetOverlappedResult(in Writer)");
				}
				else
					ErrorInComm("WriteFile (comHandle)");
			}
			
			if (dwWritten != 1)
				OutputDebugString("Error writing character to port.\n\r");
		}
	}

	OutputDebugString("Writer Thread Exiting\n");

	return 1;
}


/*************************************************************************\
*
*  FUNCTION: ProcessIO (HANDLE, HANDLE)
*
*  PURPOSE: Get comm i/o going
*
*  COMMENTS: 
*
\*************************************************************************/
void ProcessIO(HANDLE hIn, HANDLE hOut)
{
	DWORD dwReaderId, dwWriterId, dwStatusId, dwRes;
	HANDLE hReader, hWriter, hStatus;
    HANDLE hThreads[3];

	// setup comm port
	SetupCommPort();

    // create all threads (suspended)
	hReader = CreateThread( NULL, 
	                        0, 
	                        (LPTHREAD_START_ROUTINE) ReaderProc, 
	                        (LPVOID) hOut, 
	                        CREATE_SUSPENDED, 
	                        &dwReaderId);
	if (hReader == NULL)
		ErrorInComm("CreateThread (Reader)");

	hWriter = CreateThread( NULL, 
	                        0, 
	                        (LPTHREAD_START_ROUTINE) WriterProc, 
	                        (LPVOID) hIn, 
	                        CREATE_SUSPENDED, 
	                        &dwWriterId);
	if (hWriter == NULL)
		ErrorInComm("CreateThread (Writer)");

	hStatus = CreateThread( NULL, 
	                        0, 
	                        (LPTHREAD_START_ROUTINE) StatusProc, 
	                        (LPVOID) NULL, 
	                        CREATE_SUSPENDED, 
	                        &dwStatusId);
	if (hStatus == NULL)
		ErrorInComm("CreateThread (Status)");

	// start status monitoring thread
	ResumeThread(hStatus);
	
    // Comm event mask has been initialized and initial modem status reported 
    // when hSetupEvent has been signalled.
    if (WaitForSingleObject(hSetupEvent, 1000) != WAIT_OBJECT_0)
		OutputDebugString("Setup event not set.  Proceeding anyway.\n\r");
	
    CloseHandle(hSetupEvent);

    // start reader/writer threads
	ResumeThread(hReader);
	ResumeThread(hWriter);

    hThreads[0] = hStatus;
    hThreads[1] = hReader;
    hThreads[2] = hWriter;

	// wait for threads to exit
	for (;;) {
        dwRes = WaitForMultipleObjects(3, hThreads, TRUE, 5000);
		if (dwRes == WAIT_OBJECT_0)
			break;

		if (dwRes == WAIT_FAILED)
			ErrorInComm("WaitForMultipleObjects");

        if (bDone && dwRes == WAIT_TIMEOUT) {
            if (WaitForSingleObject(hStatus, 0) == WAIT_TIMEOUT)
                OutputDebugString("Status Thread didn't exit.\n\r");

            if (WaitForSingleObject(hReader, 0) == WAIT_TIMEOUT)
                OutputDebugString("Reader Thread didn't exit.\n\r");

            if (WaitForSingleObject(hWriter, 0) == WAIT_TIMEOUT)
                OutputDebugString("Writer Thread didn't exit.\n\r");

            break;
        }
	}

    BreakDownCommPort();

	// clean up handles
    
	CloseHandle(hReader);
	CloseHandle(hWriter);
	CloseHandle(hStatus);

	OutputDebugString("ProcessIO Exiting\n");
}


/*************************************************************************\
*
*  FUNCTION: SetupCommandLineOptions(int, char *)
*
*  PURPOSE: Parse command line arguments
*
*  COMMENTS: If arguments are supplied, then use them.
*            If arguments are not supplied, then use defaults.
*
\*************************************************************************/
BOOL SetupCommandLineOptions(int cArgs, char * argv[])
{
    int i;

    for ( i = 0; i < cArgs; i++) {
        if (strchr(argv[i], '?'))
            return FALSE;
    }

    switch(cArgs)
    {
        case 1: 
                strcpy(lpszPort, "COM1");   // default port
                strcpy(lpszSerialSpec, "9600,n,8,1"); // default serial spec
                break;

        case 2: 
                strcpy(lpszPort, argv[1]);
                strcpy(lpszSerialSpec, "9600,n,8,1");
                break;

        case 3:                
                strcpy(lpszPort, argv[1]);
                strcpy(lpszSerialSpec, argv[2]);
                break;
        
        default:
                return FALSE;
    }

    return TRUE;
}

/*************************************************************************\
*
*  FUNCTION: ControlHandlerProc( DWORD )
*
*  PURPOSE: Handle console control events
*
*  COMMENTS: All we do here is set our exiting flag
*
\*************************************************************************/
BOOL WINAPI ControlHandlerProc( DWORD dwCtrlType )
{
    // we can clean up here
    OutputDebugString("Control Event Handled\n\rExiting\n\r");
    printf("\n\n\rExiting...");
    bDone = TRUE;

    return TRUE;   // don't call default handler
}



/*************************************************************************\
*
*  FUNCTION: main(int, char *)
*
*  PURPOSE: Call ParseCommandLine function, Setup console mode, setup
*           console control handling and start io routine
*
*  COMMENTS:
*
\*************************************************************************/
int main(int argc, char * argv[])
{
	DWORD dwOldMode, dwNewMode, dwVersion;	
	HANDLE hStdOut, hStdIn;

    // check if running on Win32s, if so, display notice and terminate
    
    dwVersion = GetVersion();
    if( !(dwVersion < 0x80000000 ) && (LOBYTE(LOWORD(dwVersion)) < 4) ) {
        MessageBoxA( NULL,
            "This sample application will not run on Windows 3.1.\n"
            "This application will now terminate.",
            "OvComm",
            MB_OK | MB_ICONSTOP | MB_SETFOREGROUND );
        return 1 ;
    }

    // parse command line
    if (!SetupCommandLineOptions(argc, argv)) {   
        printf("Usage: comm <port identifier> <serial spec string>\n");
        printf("\n   example:  comm com1 9600,N,8,1\n");
        printf("     The serial spec string cannot contain spaces\n");
        printf("     The serial spec string follows the same guidelines\n");
        printf("       as the MS-DOS 'mode' command\n");
        return 0;
    }
    
    // setup console mode and std handles
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdIn = GetStdHandle(STD_INPUT_HANDLE);

	if (hStdOut == hStdIn)
		ErrorHandler("GetStdHandle");

	if (!GetConsoleMode(hStdIn, &dwOldMode))
		ErrorHandler("GetConsoleMode (old)");

    // turn off echo and CR & ctrl-c handling; I'll do it myself.
	dwNewMode = dwOldMode 
	            & ~ENABLE_LINE_INPUT 
	            & ~ENABLE_ECHO_INPUT 
	            & ~ENABLE_PROCESSED_INPUT;

	if (!SetConsoleMode(hStdIn, dwNewMode))
		ErrorHandler("SetConsoleMode (new)");

    if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)ControlHandlerProc, TRUE))
        ErrorHandler("SetConsoleCtrlHandler");

	ProcessIO(hStdIn, hStdOut);

    // restore console mode
	if (!SetConsoleMode(hStdIn, dwOldMode))
		ErrorReporter("SetConsoleMode (old)");

 	return 0;
}

