This directory contains the files for the sample
distributed application "cluuid":

File          Description

README.TXT    Readme file for the cluuid sample
CLUUID.IDL    Interface definition language file
CLUUID.ACF    Attribute configuration file
CLUUIDC.C     Client main program
CLUUIDS.C     Server main program
CLUUIDP.C     Remote procedures
MAKEFILE      Nmake file to build for NT
MAKEFILE.DOS  Nmake file to build for MS-DOS

This sample program demonstrates how to supply
multiple implementations of the remote procedure
specified in the interface.  It also demonstrates
how the client selects among the implementations
by providing a client object uuid.

The server calls RpcObjectSetType to associate a
client object uuid with the object uuid in the
Object Registry Table. The server initializes a
manager entry point vector (manager epv) and
then calls RpcRegisterIf to associate the interface
uuid and the object uuid with the manager epv in the
Interface Registry Table.

When the client makes a remote procedure call,
the client object uuid is mapped to the object uuid
in the Object Registry Table.  The resulting
object uuid and the interface uuid are mapped to
a manager entry point vector in the Interface
Registry Table.

By default, in this example, the server registers
two implementations of the "hello, world" function
HelloProc and HelloProc2.  The HelloProc2
implementation is associated with the object uuid
"11111111-1111-1111-1111-111111111111". When
the client makes a procedure call with a null
uuid, the client's request is mapped to the
original HelloProc.  When the client makes a
procedure call with the client object uuid
"11111111-1111-1111-1111-11111111111", the
client's request is mapped to HelloProc2 (which
prints the string in reverse).

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

This builds the executable programs cluuidc.exe
(client) and cluuids.exe (server).

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

This builds the client application cluuidc.exe.

------------------------------------------
RUNNING THE CLIENT AND SERVER APPLICATIONS
------------------------------------------

On the server, enter

  cluuids

On the client, enter

  net start workstation
  cluuidc

To call the second implementation of the function,
on the client, enter
  cluuidc -u "11111111-1111-1111-1111-111111111111"

Note:  The client and server applications can run on
the same Microsoft Windows NT computer when you use
different screen groups.

Several command line switches are available to change
settings for this program. For a listing of the switches
available from the client program, enter

  cluuidc -?

For a listing of switches available from the server 
program, enter

  cluuids -?
