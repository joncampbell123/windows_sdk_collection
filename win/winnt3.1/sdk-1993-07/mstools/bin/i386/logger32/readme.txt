
                     Microsoft Usage and Description README File

                        Windows 32-bit API Parameter Logger
				   July 1993



1. Hardware/Software Requirements

	o  80386/486 based PC
	o  Windows NT 3.1



2. Logging DLLS

        o  zdvapi32.dll     advapi32 APIs' parameter logger DLL
        o  zernel32.dll     kernel32 APIs' parameter logger DLL
        o  zser32.dll       user32 APIs' parameter logger DLL
        o  zdi32.dll        gdi32 APIs' parameter logger DLL



3. Other Required Files

        o  logger32.dll     API logger DLL

	o  apf32cvt.exe     Converts modules of the application to be logged.



4. Installing/Running the API Logger

	o  Copy all the logging dlls to the system sub-directory of your
           windows directory (i.e. %windir%\system32).

        o  Copy LOGGER32.DLL to the system sub-directory of your windows
           directory (i.e. %windir%\system32).

        o  Use APF32CVT to convert the modules from the application you
           wish to log.

           Examples:
              apf32cvt ?                        -- Displays the usage
              apfcnvrt zser32 perfmon.exe       -- Log user32 calls made by
                                                   perfmon.exe
              apfcnvrt zser32 zdi32 perfmon.exe -- Log user32 & gdi32 calls
						   made by perfmon.exe

        o  Run the application you wish to log.

        o  The application will run slower since it is being logged.  It will
	   create two output files:  OUTPUT32.LOG and OUTPUT32.DAT.
	   The data is written to disk every time the logger's 32K buffer fills
	   up - every few seconds normally.  Make sure you have enough free
	   space on your disk since the log file could become very large.

        o  Use APF32CVT to restore use of original modules in the
           logged application. ("unhooking" the application from the logger)

           Examples:
              apfcnvrt ?                        -- Displays the usage
              apfcnvrt undo perfmon.exe         -- Restores original perfmon.exe
                                                   imports
              apf32cvt user32 perfmon.exe *.dll -- "unhooks" user32 profiling
                                                   from perfmon.exe and its dlls
              apfcnvrt zser32 zdi32 perfmon.exe -- "unhooks" user32 & gdi32
						   profiling from perfmon.exe



5.  Logger Logfile Format

        o  Logger will create two output files:  OUTPUT32.LOG and OUTPUT32.DAT.
	   OUTPUT32.LOG is a log of all of the API calls made by the
	   application.  OUTPUT32.DAT holds any large data that are greater than
	   128 characters.  OUTPUT32.LOG will put "DATAFILE offset" in place of
	   the actual data where "offset" is the offset into the OUTPUT32.DAT
	   file.  OUTPUT32.DAT is a binary data file, whereas OUTPUT32.LOG is a
	   text data file.  Both files are created in the current directory.
           Each api call record has 2 sections - APICALL and APIRET.

              APICALL - Api parameters are recorded here.  If the param is a
                        ptr, and points to documented structure, then the
                        structure contents are recorded.  Structure contents
                        are enclosed in brackets.

              APIRET  - Api return code, plus any additional return data.



6. Comments

        o  The logger works only on x86 platforms.
	o  Logger DLLs (z*.dll) have the same name as 32-bit Windows API
	   profiling DLLs.  Therefore, one should be careful not to mix
	   profiling and logging DLLs.
	o  There is NO logging DLL for crtdll.dll.
	o  Applications running under logging DLLs will run much slower.


*** END OF README ***
