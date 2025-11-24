This directory contains the files for the sample
distributed application "mandel":

File          Description

README.TXT    Readme file for the MANDEL sample
MDLRPC.IDL    Interface definition language file
MDLRPC.ACF    Attribute configuration file
MANDEL.C      Client main program
MANDEL.H      Client global data
REMOTE.C      Client code that calls remote procedures
RPC.ICO       Client icon
MANDEL.DEF    Client module definition file
MANDEL.RC     Client resource script file
SERVER.C      Server main program
CALC.C        Remote procedures
MAKEFILE      nmake utility for Windows NT
MAKEFILE.WIN  nmake utility for Win 3.x

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

This builds the executable programs client.exe and
server.exe for Microsoft Windows NT.  

--------------------------------------------   
BUILDING THE CLIENT APPLICATION FOR WIN 3.X   
--------------------------------------------   

After you install the Microsoft C/C++ version 7.0
development environment and the Microsoft RPC version
1.0 toolkit on a Microsoft MS-DOS and Windows computer, 
you can build the sample client application for Win 3.x.
Enter:

  nmake -f makefile.win cleanall
  nmake -f makefile.win

This builds the client application client.exe.

------------------------------------------          
RUNNING THE CLIENT AND SERVER APPLICATIONS          
------------------------------------------          

On the server, enter

  server

On the client, enter

  net start workstation
  client

Note:  The client and server applications can run on
the same Microsoft Windows NT computer when you use
different screen groups.  If you run the client on the
Microsoft MS-DOS and Windows computer, choose the Run
command from the File menu in the Microsoft Windows 3.x
Program Manager and enter client.exe.

Several command line switches are available to change
settings for the server program. For a listing of switches
available from the server program, enter

  server -?
