

If you are installing the DirectX6 SDK on a system that does not have
Internet Explorer 4.0 (IE4) installed, you will need to install it in 
order to view the DirectX6 Documentation in the HTMLHelp format.  This 
is only required on Win95 systems without IE4.  HTMLHelp is the new 
Microsoft standard for viewing help files.  

To view the DirectX6 Documentation using HTMLHelp, first install IE4
and then install the HTMLHelp Active Control.


Installing IE4
--------------

You will find English and Japanese versions of IE4 in the enclosed folders.
For other language version of IE4, visit the Microsoft web site:
html:\\www.microsoft.com\ie.

To install one of the enclosed versions, change your working directory
to the desired verion and execute IE4Setup.exe.

A minimal install of IE4 can be accomplished with the following command 
line options:

   ie4setup.exe /c:"ie4wzd /Q /I:N /R:N /S:#e"

This will install IE4 quietly (no user interaction) without installing 
the Active Desktop, Icons and Links.  You will need to manually reboot
the system before IE4 is fully installed.


Installing HTMLHelp Active Control
----------------------------------

Following completion of the IE4 install and reboot, run the HTMLHelp 
setup in the HTMLHelp folder (hhupd.exe).  You do not need any command 
line options and a reboot is not required.


Viewing Help Files
------------------

Now you are ready to view the DirectX6 Documentation using HTMLHelp.
If you installed 'Documentation' with the DirectX6 SDK.  Look in the 
<drive>:\<DX6 install dir>\docs\DirectX6 for a DirectX.chm file.  
Double click on this file and you should be viewing the DirectX6 
Documentation in HTMLHelp.

If you experience any problems, contact the Microsoft DirectX Group at
directx@microsoft.com.
