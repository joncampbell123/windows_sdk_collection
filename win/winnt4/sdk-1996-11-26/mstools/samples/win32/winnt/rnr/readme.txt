Registration and Name Resolution (RNR) Service


SUMMARY
=======

The RNR sample demonstrates how to write a Windows NT service that 
communicates using Windows Sockets. There are seven executables in this 
sample. The rnrsrv.exe and rnrsvc*.exe programs are variations on the same 
functional service -- a program that connects to client applications, 
receives data, and echoes it back. The rnrsrv.exe program is single-
threaded, and handles one client at a time. The other four rnrsvc*.exe 
programs use progressively more complicated threading models. See the 
comments at the top of the client*.c files for more information.

Two other executables are provided here to support the server examples.
To install or remove any of the services, use rnrsetup.exe. To connect
to, and test the services, use rnrclnt.exe.

MORE INFORMATION
================

There is an article titled "Write an NT WinSock Service" in the December
1994 issue of BYTE magazine. It describes the Registration and Name
Resolution sample code, including an in-depth description of the threading
models.

