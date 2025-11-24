This directory contains the files for the sample
distributed application "whello":

File          Description

README.TXT    Readme file for the WHELLO sample
WHELLO.IDL    Interface definition language file
WHELLO.ACF    Attribute configuration file
WHELLOC.C     Client main program
WHELLO.RC     Client resource file
WHELLO.DLG    Client dialog box definitions
WHELLO.DEF    Client module definition file
WHELLOS.C     Server main program
WHELLOP.C     Remote procedures
MAKEFILE      Nmake file for Windows NT
MAKEFILE.WIN  Nmake file for Win 3.x

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

Build the sample server application:

  nmake cleanall
  nmake

These commands build the executable program whellos.exe
and whelloc.exe for Microsoft Windows NT.

--------------------------------------------     
BUILDING THE CLIENT APPLICATION FOR WIN 3.X     
--------------------------------------------     

Build the sample client application:

  nmake -f makefile.win cleanall
  nmake -f makefile.win

These commands build the client executable program
whelloc.exe.

------------------------------------------    
RUNNING THE CLIENT AND SERVER APPLICATIONS    
------------------------------------------    

On the server, enter

  whellos

On the client, enter

  net start workstation
  whelloc

Note:  The client and server applications can run on     
the same Microsoft Windows NT computer when you use      
different screen groups.  If you run the client on the   
Microsoft MS-DOS and Windows computer, choose the Run    
command from the File menu in the Microsoft Windows 3.x  
Program Manager and enter whelloc.exe.                    

Several command line switches are available to change
settings for the server application. For a listing of
available switches, enter

  whellos -?

