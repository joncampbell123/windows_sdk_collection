Purpose:
WebRunner is an Internet Server Application. It runs on the HTTP Server machine 
and allows remote execution of the text based utilities, such as netstat,
ping, etc. 

Theory of operation:
The client connects to the HTTP server via Web browser. Commands are entered 
through the HTML form on the remote machine. The entered command is transfered
to the server and gets executed on the server machine. 

Implementation notes:
WebRunner is capable of running in two different modes: user mode and 
administrator mode. In the administrator mode virtually any text based command,
entered on the client machine will be executed on the server. This can create
a potential security hole. In user mode only authorized commands can be 
executed. Authorized commands appear on the client's machine Web Browser in the
list box. Since any HTML form can be spoofed (i.e. client string 
"field1=data1&field2=data2 can be manually created and then passed to the server 
via POST or GET) the following verification takes place. When WebRunner is running in 
user mode, the received command is looked for in the list of authorized
commands. Only if the command is on this list does it get executed. The list of  
authorized commands and the mode is stored in the registry. All of the above
described functionality is incorporated in runner.dll. This is the Internet Server 
Extension dll that gets loaded by the server and exposes the following functions:
HttpExtensionProc()
GetExtensionVersion().
WebRunner is configured by the a control panel applet: WebRun.cpl. This 
application is responsible for writing the list of commands to the registry 
and creats the initial HTML file. The initial HTML file is different for user 
mode and administrator mode. In user mode the Web Page has a list box with the 
allowed commands.  In administrator mode the HTML page has an edit box to enter 
any command.

Registry location:
Following registry entries control WebRunner.

HKEY_LOCAL_MACHINE\Software\WebRunner\Commands
HTMLListBoxLines - REG_SZ. 
This key control contains the commands displayed in the list box in the user mode. 
It has following form: 
  <option>command1</option>
This field can be manually edited to add/remove commands. The control panel applet is capable of adding and deleting commands.

HKEY_LOCAL_MACHINE\Software\WebRunner\Parameters
Mode - REG_BINARY.
01000000 is for administrator mode, 00000000 for user mode. This field can be manually 
edited, to change modes. The control panel applet is capable of changing WebRunner modes.

FilePath - REG_SZ.
This parameter controls the location of the initial HTML file.  It can be manually edited.

Installation:
Put all files in the same directory and run setup.bat. It will build both 
the control panel applet and the extension dll. It will then copy webrun.cpl to the 
system directory. Setup will then start the control panel applet to configure 
WebRunner. You can build runner.dll and WebRun.cpl  separately with the provided 
makefiles.


List of files:
readme.txt             This file.          
setup.bat              Setup batch file.
runner.def 	       Definition file for the runner.dll	
runner.mak             Make file for the Internet Server extension dll.      
webrun.mak 	       Make file for control panel applet: webrun.cpl                              
runcpl.def             Definition file for control panel applet.
runcpl.c               Source  code for control panel applet.
runcpl.h               Header file for  control panel applet.                             
runcpl.ico             Icon for the control panel applet.
runcpl.rc              Resource file for control panel applet.
resource.h             Header file for with the resource identifiers.
runner.c               Source file for the Internet Server extension dll.
runner.h               Header file for the Internet Server extension dll.
runner.rc              Resource file for Internet Server extension dll.

Known Issues:
- Internal commands such as "DIR" or "VER" cannot be run by themselves.  In order
to run them enter "cmd /c dir" or "cmd /c ver".
- Web Runner does not recognize that Windows Applications are not console applications.
Therefore this version will never recognize that the application has or has not terminated 
and thus will never return a response.  Also, unless the application quits on its own accord,
it will hang in memory until you reboot.
- The commands run as the local system account which has unlimited authority on the local 
machine, and no authority across the network.



