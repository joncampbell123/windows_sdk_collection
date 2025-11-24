This directory contains the files for the sample
distributed application "usrdef":

File          Description

README.TXT    Readme file for the USRDEF sample
USRDEF.IDL    Interface definition language file
USRDEF.ACF    Attribute configuration file
USRDEFC.C     Client main program
USRDEFS.C     Server main program
USRDEFP.C     Remote procedures
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

This builds the executable programs usrdefc.exe 
(client) and usrdefs.exe (server).

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

This builds the client application usrdefc.exe.

------------------------------------------
RUNNING THE CLIENT AND SERVER APPLICATIONS
------------------------------------------

On the server, enter

  usrdefs

On the client, enter

  net start workstation
  usrdefc

Note:  The client and server applications can run on
the same Microsoft Windows NT computer when you use
different screen groups.

Several command line switches are available to change
settings for this program. For a listing of the switches
available from the client program, enter

  usrdefc -?

For a listing of switches available from the server 
program, enter

  usrdefs -?
