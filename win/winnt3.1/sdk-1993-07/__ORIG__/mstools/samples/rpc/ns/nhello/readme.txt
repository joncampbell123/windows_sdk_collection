This directory contains the files for the sample distributed application 
"nhello":

File	Description

README.TXT	Readme file for the NHELLO sample
NHELLO.IDL	Interface definition language file
NHELLOC.C	Client main program
NHELLOS.C	Server main program
NHELLOP.C	Remote procedures
MAKEFILE	Nmake file to build for NT
MAKEFILE.DOS	Nmake file to build for MS-DOS

-----------------------------------------------------------------------
BUILDING CLIENT AND SERVER APPLICATIONS FOR MICROSOFT WINDOWS NT:
-----------------------------------------------------------------------

The following environment variables should be set for you already.
  set CPU=i386 
  set INCLUDE=c:\mstools\h
  set LIB=c:\mstools\lib
  set PATH=c:\winnt\system32;c:\mstools\bin;

For mips, set CPU=mips

Build the sample distributed application:
  nmake cleanall
  nmake

This builds the executable programs nhelloc.exe 
(client) and nhellos.exe (server).

-----------------------------------------------------------------------
BUILDING THE CLIENT APPLICATION FOR MS-DOS
-----------------------------------------------------------------------

After you install the Microsoft C/C++ version 7.0 development 
environment and Microsoft RPC toolkit on an MS-DOS or Microsoft Windows 
computer, you can build the sample client application for MS-DOS. Enter:

  nmake -f makefile.dos cleanall
  nmake -f makefile.dos

This builds the client application nhelloc.exe.

-----------------------------------------------------------------------
RUNNING THE CLIENT AND SERVER APPLICATIONS
-----------------------------------------------------------------------

On the server, enter

  net start rpclocator
  nhellos

(You can also Start the RPC Locator by running the Services 
program in Control Panel.)

On the client, enter

  net start workstation
  nhelloc

Note:  The client and server applications can run on the same Microsoft 
Windows NT computer when you use different screen groups.

Several command line switches are available to change settings for this 
program. For a listing of the switches available from the client 
program, enter:

  nhelloc -?

For a listing of switches available from the server program, enter

  nhellos -?
