Microsoft Internet Information Server
Lottery - An ISAPI DLL that maintains a persistent worker thread

Lottery.Dll is an ISAPI extension that illustrates how to use a persistent
worker thread, allowing it to run independent of client requests.  The worker
thread continuously increments a lottery number, and when a client request
comes in, the current lottery number is sent as a reply.  The Lottery sample
also illustrates how to change security tokens and how to use 
HTTP_STATUS_PENDING.

Steps to build the sample:

1. Set the environment variables WWWROOT and WWWSCRIPTS to point to your 
   c:\inetsrv\wwwroot and c:\inetsrv\scripts directory, respectively.

2. Make sure your INCLUDE variable points to both the Win32 SDK and the ISAPI
   header files, and make sure your LIB variable points to the Win32 SDK.

3. With your PATH variable set to your compiler, run nmake from the command
   line inside the Lottery directory.

The build results will be copied to your WWW structure: Lottery.Dll will be
copied to the SDK sub-directory off the path specified in WWWSCRIPTS, and
Default.Htm will be copied to the SDK\Lottery sub-directory off the path
specified in WWWROOT.

The makefile that comes with the Internet SDK uses the DLL version of the C
Runtime Library.  You must have this DLL on your server in order for the
samples to work.  For example, if you are using Visual C++ 4.0 to build the
DLL, it will need the debug version of the C Runtime Library DLL, named
MSVCR40D.DLL.  Verify that this file is in your server's SYSTEM32 directory.
