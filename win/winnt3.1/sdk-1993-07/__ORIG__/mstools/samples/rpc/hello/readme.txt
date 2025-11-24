This directory contains the files for the sample
distributed application "hello":

File          Description

README.TXT    Readme file for the HELLO sample
HELLO.IDL     Interface definition language file
HELLO.ACF     Attribute configuration file
HELLOC.C      Client main program
HELLOS.C      Server main program
HELLOP.C      Remote procedures
MAKEFILE      Nmake file to build for NT
MAKEFILE.DOS  Nmake file to build for MS-DOS

-------------------------------------------
BUILDING CLIENT AND SERVER APPLICATIONS FOR
MICROSOFT WINDOWS NT:
-------------------------------------------

The following environment variables should be set for you already.       
  set CPU=i386                                                           
  set INCLUDE=c:\mstools\h                                               
  set LIB=c:\mstools\lib                                                 
  set PATH=c:\winnt\system32;c:\mstools\bin;

For mips, set CPU=mips                                                   

Build the sample distributed application:
  nmake cleanall
  nmake

This builds the executable programs helloc.exe 
(client) and hellos.exe (server).

------------------------------------------
BUILDING THE CLIENT APPLICATION FOR MS-DOS
------------------------------------------

After you install the Microsoft C/C++ version 7.0
development environment and the Microsoft RPC version
1.0 toolkit on an MS-DOS or Microsoft Windows computer,
you can build the sample client application for MS-DOS.
Enter:

  nmake -f makefile.dos cleanall
  nmake -f makefile.dos

This builds the client application helloc.exe.

------------------------------------------
RUNNING THE CLIENT AND SERVER APPLICATIONS
------------------------------------------

On the server, enter

  hellos

On the client, enter

  net start workstation
  helloc

Note:  The client and server applications can run on
the same Microsoft Windows NT computer when you use
different screen groups.

Several command line switches are available to change
settings for this program. For a listing of the switches
available from the client program, enter

  helloc -?

For a listing of switches available from the server 
program, enter

  hellos -?
