/////////////////////////////////////////////////////////////////////////////
//       This is a part of the Microsoft Source Code Samples. 
//       Copyright (C) 1992-1996 Microsoft Corporation.
//       All rights reserved. 
//       This source code is only intended as a supplement to 
//       Microsoft Development Tools and/or WinHelp documentation.
//       See these sources for detailed information regarding the 
//       Microsoft samples programs.
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//   MODULE:  ASYN_IO.C                                              
//                                                                         
//                                                                         
//   PURPOSE: Demonstrates the basic concepts envolved when using          
//            asynchronous IO. In particular the main advantage is your    
//            app more correctly thread can go about doing useful work     
//            while waiting for IO. The main disadvantage is you must keep 
//            track of the file pointer.                                   
//                                                                         
//   INPUT: Both the File to read and File to write to must be entered     
//          on via the command line. Note the file to write to must be     
//          unique.                                                        
//                                                                         
//   FUNCTIONS:                                                            
//                                                                         
//       Do_BackgroundTask()                                               
//                                                                         
//                                                                         
//   COMMENTS: Reads a file asynchronously writes to another synchronously 
//                                                                         
//                                                                         
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>

#define SECTOR_SIZE  1024

VOID ErrorMsg         (LPCTSTR psz);
VOID Do_BackgroundTask(VOID);

VOID main(int argc, char *argv[])
{
	BOOL       bSuccess ;          // return code of last API called
	
	CHAR       buf[SECTOR_SIZE] ;  // Buffer used by either
                                   // ReadFile() or WriteFile()

	DWORD      cbBytesRead,        // count of bytes read by ReadFile()
                                   // When asynchonous I/O is performed
                                   // one must check how many bytes were
                                   // read by calling GetOverLappedResults().

               cbBytesWritten,     // count of bytes written by WriteFile()
	  
               cbBytesTransfered ; // this value is returned by
                                   // GetOverLappedResults() represents the
                                   // count of bytes transfered asynchronous

	DWORD      dwStatus,           // value returned by GetLastError()
               dwSuccess ;         // return code of last API called

    HANDLE     hInfile,            // Handle to the file to read
               hOutfile;           // Handle to the file to write to
    
    HANDLE     hEvent ;            // Handle to the synchronization Event

    OVERLAPPED OverLapped ;        // The operating system does not automatically
                                   // track the current file position of a file
                                   // open for asynchroous I/O. Instead one must
                                   // manually track this by updating the OVERLAPPED
                                   // structure. To update this structure one uses
                                   // the result returned by GetOverLappedResults().

/////////////////////////////////////////////////////////////////////////////
//                                                                         
//  This block of code initalizes all the necessary variables needed to do 
//  asynchronous IO. Specfically it opens the files and initializes the    
//  synchronization EVENT and OVERLAPPED structure needed to keep track    
//  of asynchronous file I/O                                               
//                                                                         
/////////////////////////////////////////////////////////////////////////////

	// check if running on Windows NT, if not, display notice and terminate
	if (GetVersion() & 0x80000000)
    	{
    	MessageBox(NULL,
                   "This sample application can only be run on Windows NT.\n"
                   "This application will now terminate.",
                   "Event",
                   MB_OK | MB_ICONSTOP | MB_SETFOREGROUND );
      	return;
   		}

   	if (argc != 3)
   		{
      	printf ("\nUsage: ASYN_IO input_file output_file\n");
      	ExitProcess(1L);
   		}

	// Open file which will be copying.  Note we are opening this 
	// asynchronously

   	hInfile = CreateFile(argv[1],              // File to read
                         GENERIC_READ,         // Open file for reading only
                         FILE_SHARE_READ,      // Allow others to read file
                         NULL,                 // No security
                         OPEN_EXISTING,        // Fail if file doesn't exit
                         FILE_FLAG_OVERLAPPED, // Async I/o
                         NULL);

	if (hInfile == INVALID_HANDLE_VALUE)
   		{
      	printf("Error opening %s.\n", argv[1]);
      	ErrorMsg("CreateFile");
   		}

	// Create the file to write to; here we have opened it for synchronous IO.

	hOutfile = CreateFile(argv[2],                 // File to write to
                          GENERIC_WRITE,           // Open file only to write to
                          0,                       // No sharing allowed
                          NULL,                    // No security
                          CREATE_NEW,              // Fail if file exists
                          FILE_ATTRIBUTE_NORMAL,   // No attributes
                          NULL);

   	if (hOutfile == INVALID_HANDLE_VALUE)
   		{	
    	printf("Error opening %s.\n", argv[2]);
      	ErrorMsg("CreateFile") ;
   		}

	// Create Synchronizing Event

   	hEvent = CreateEvent(NULL,    // No security
                         TRUE,    // Manual reset
                         FALSE,   // Initially Event set to non-signaled state
                         NULL);   // No name
   	if (hEvent == NULL)
		ErrorMsg("CreateEvent") ;

	//
    // Intialize OverLapped structure. We will be using an Event instead
    // of the file handle for synchronizing I/O. One reason to use a unique
    // event instead of the file handle would be a so multi-threaded app
	// would know which thread to unblock when the handle is set to the
	// signaled state.
 	//

   	memset(&OverLapped, 0, sizeof(OVERLAPPED));
   	OverLapped.hEvent = hEvent ;

   	bSuccess = TRUE ;

	/***************************************************************************\
	**                                                                         **
	** This is the main function of this program. Here we loop until the file  **
	** has been copied. Notice that even though we are using asynchronous I/O  **
	** ReadFile() may still return sychronously if it can be done fast enough  **
	**                                                                         **
	\***************************************************************************/

   	while (bSuccess)
   		{

		// Read file asynchronously

      	bSuccess = ReadFile(hInfile,        // Handle of file to read
              				buf,            // Buffer to store input
              				SECTOR_SIZE,    // Number of bytes to read
              				&cbBytesRead,   // Number of bytes read. See note]
                              				// above in delcaration
              				&OverLapped);   // Used for asynchronous I/O

		// Check whether ReadFile() has completed yet

      	if (!bSuccess && ((dwStatus = GetLastError()) == ERROR_IO_PENDING))
      		{
        	printf("\nIO Pending");

	
			// Read not complete yet first execute the background task then check
			// to the if event "hEvent" set to the signaled state. If the event is
			// not set signaled state then loop through again - run the background
			// task and check the event.
			
        	Do_BackgroundTask();

        	dwSuccess = WaitForSingleObject(hEvent, 0);

        	if (dwSuccess == WAIT_FAILED)
				{
            	ErrorMsg("WaitForSingleObject");
        		}
        	else if (dwSuccess == WAIT_OBJECT_0)
        		{
            	bSuccess = GetOverlappedResult(hInfile, 
            	                               &OverLapped, 
            	                               &cbBytesTransfered, 
            	                               FALSE);
            	if (!bSuccess)
                	ErrorMsg("GetOverLappedResult");
            	else
            		{
                	// Read has completed now find out how many bytes have been read
                	printf ("\nNumber of bytes transfered is %d\n", cbBytesTransfered);
                	OverLapped.Offset += cbBytesTransfered ;
                	cbBytesRead = cbBytesTransfered ;
            		}
        		}
       		else if (dwSuccess == WAIT_TIMEOUT)
            	{
            	Do_BackgroundTask () ;
      			}
      		}

		// ReadFile() read file synchronously; update Overlapped structure

 		else if (bSuccess  && cbBytesRead != 0)
      		{
         	printf ( "\nNumber of bytes transfered is %d\n", cbBytesRead ) ;
         	OverLapped.Offset += cbBytesRead ;
      		}

		// ReadFile() has found the end of the file; exit loop

      	else if (!bSuccess  && GetLastError() == ERROR_HANDLE_EOF)
      		{
         	printf("\nEnd of file read\n");
         	break ;
      		}
      	else
			{
	
			// An error occured while reading, print out status and exit loop.
	
        	ErrorMsg ("ReadFile");
			}

	// Write to file synchronously

	bSuccess = WriteFile(hOutfile,             // Handle of file to write to
              			 buf,                  // Data to write to file
              			 cbBytesRead,          // Number of bytes to write
              			 &cbBytesWritten,      // Number of bytes written
              			 NULL);                // Only need for asynchonous output

	if (!bSuccess)
    	ErrorMsg("WriteFile");

   } // End of WHILE loop

   CloseHandle(hInfile);        // Close handle to input file
   FlushFileBuffers(hOutfile);  // Make sure all data written to file first
   CloseHandle (hOutfile);      // Close handle to output file
   ExitProcess(0L);

}

/////////////////////////////////////////////////////////////////////////////
//                                                                         
//   FUNCTION: Do_BackgroundTask ( VOID )                                  
//                                                                         
//   PURPOSE: Works in the background while IO sytems                      
//                                                                         
//   COMMENTS:                                                             
//                                                                         
//      Currently this does nothing if I have the time I will come up      
//      with some silly task to keep the user entertained.                 
//                                                                         
//   INPUT: None                                                           
//                                                                         
//   OUTPUT: None                                                          
//                                                                         
/////////////////////////////////////////////////////////////////////////////
VOID Do_BackgroundTask(VOID)
{
  Sleep (5000L) ;
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         
//   FUNCTION: ErrorMsg(LPTSTR)                                            
//                                                                         
//   PURPOSE: Prints out an error message the return code of the last API  
//            to fail.                                                     
//                                                                         
//   INPUT: psz: This string is a description of where the code failed in  
//          the module                                                     
//                                                                         
//   OUTPUT: None                                                          
//                                                                         
/////////////////////////////////////////////////////////////////////////////

VOID ErrorMsg(LPCTSTR psz)
{
	LPVOID lpvMessageBuffer;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
  				  NULL, GetLastError(), 
  				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), //The user default language
  				  (LPTSTR)&lpvMessageBuffer, 0, NULL);

	//... now display this string
	printf("ERROR: API        = %s.\n", psz);
	printf("       error code = %d.\n", GetLastError());
	printf("       message    = %s.\n", (LPTSTR)lpvMessageBuffer);

	// Free the buffer allocated by the system
	LocalFree(lpvMessageBuffer);

	ExitProcess(EXIT_FAILURE);
}
