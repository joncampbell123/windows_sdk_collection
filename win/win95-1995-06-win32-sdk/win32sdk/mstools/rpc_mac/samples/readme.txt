This directory contains the files for the sample
distributed application "hello":

File          Description

README.TXT    Readme file for the HELLO sample
HELLO.IDL     Interface definition language file
HELLO.ACF     Attribute configuration file
HELLOC.C      Client main program
HELLOS.C      Server main program
HELLOP.C      Remote procedures
MAKEFILE      Nmake file to build for Windows NT or Windows 95
HELLO.RC      Resource for used for build the client
HELLORES.H  Contains the resource ids
WGEN.R         Mac resource file

In order to build the sample you need to do the following:

1)  Install MSVC 2.0 with Mac Cross Development support
2)  Install WIN32 SDK
3)  Install MAC RPC SDK
4)  Copy the Mac RPC sample to your machine
5)  Run the following commands

REM Start 
set Cpu=i386
set MSVC20=c:\msvc20  (path to your msvc20 directory)
set MSTOOLS=c:\MSTOOLS (path to your win32 sdk directory)
call %MSVC20%\bin\vcvars32
set Path=%Mstools%\bin;%path%
set Lib=%Mstools%\lib;%Mstools%\mssetup\lib;%lib%
set Include=%Mstools%\include;%Mstools%\mssetup\include;%include%
set MACSDK=%MSVC20%\m68k
REM End


6) In the file helloc.c you must change pszNetworkAddress in 
the function CallServer to contain the network address of the
machine on which the server is running.
7) Make sure that you are running the AppleTalk protocol and 
Services for Macintosh on the server. 

nmake cleanall
nmake all

This builds the client and server applications.

------------------------------------------
RUNNING THE CLIENT AND SERVER APPLICATIONS
------------------------------------------

On the server, enter

  hellos -p ncacn_at_dsp

On the client:
  1. Copy helloc.exe using mfile:
      mfile copy /t APPL client\helloc.exe :<Mac Hard Drive Name>:helloc.exe
  2. Run helloc and select Run\Call Server to call the server. 

For a listing of switches available from the server
program, enter

  hellos -?
