This directory contains the files for the sample distributed application
"yield":

File          Description

README.TXT    Readme file for the YIELD sample
YIELD.IDL     Interface definition language file
YIELD.ACF     Attribute configuration file
YIELDC.C      Client main program
YIELDC.RC     Client resource file
YIELDC.DLG    Client dialog box definitions
YIELDC.DEF    Client module definition file
YIELDS.C      Server main program
YIELDP.C      Remote procedures
MAKEFILE      Nmake file for Windows NT
MAKEFILE.win  Nmake file for Win 3.x

------------------------------------------------------      
BUILDING SERVER APPLICATION FOR MICROSOFT WINDOWS NT:                           
------------------------------------------------------      

The following environment variables should be set for you already.       
  set CPU=i386                                                           
  set INCLUDE=c:\mstools\h                                               
  set LIB=c:\mstools\lib                                                 
  set PATH=c:\winnt\system32;c:\mstools\bin;

For mips, set CPU=mips                                                   

Build the sample server application:

  nmake cleanall
  nmake

These commands build the server executable program YIELDS.EXE.

--------------------------------------------     
BUILDING THE CLIENT APPLICATION FOR WIN 3.X     
--------------------------------------------     

Build the sample client application:

  nmake -f makefile.win cleanall
  nmake -f makefile.win

These commands build the client executable program YIELDC.EXE.

------------------------------------------       
RUNNING THE CLIENT AND SERVER APPLICATIONS       
------------------------------------------       
                                                 
On the server, enter

  yields

On the client, choose the Run command from the File menu in the
Microsoft Windows 3.x Program Manager and enter YIELDC.EXE.

Several command line switches are available to change settings for
the server application. For a listing of available switches, enter

  yields -?

Bug: If you cancel in the middle of a custom yield, you must wait 
for the number of seconds you originally set before making another 
remote procedure call.

