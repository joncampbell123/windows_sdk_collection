--------------------------------------------------------------
Installing the Software Development Kit

You can use the INSTALL.BAT file to install the programs, libraries, 
and include files of the Software Development Kit on your hard
disk.  To install the development kit, copy the file
INSTALL.BAT from disk 2 to your hard disk, then type

	INSTALL <binDir> <winDir> <incDir> <libDir>

where
 <binDir> is the pathname of the directory to receive programs.
	  The recommended directory is \BIN.

 <winDir> is the pathname of the directory to receive Windows
	  applications and files. The recommended directory is \WINDOWS.

 <incDir> is the pathname of the directory to receive include
	  files. The recommended directory is \INCLUDE.

 <libDir> is the pathname of the directory to receive library
	  files. The recommended directory is \LIB.

After installing the development kit, you should modify your
AUTOEXEC.BAT file to set the following environment variables
to the given values:

	set PATH=\bin;\windows
	set LIB=\lib
	set INCLUDE=\include

You should also create a \TMP directory to hold temporary files 
created by some of the development kit programs, and assign the 
name of this directory to the environment variables TEMP and TMP.

Finally, you should add the following statements to your CONFIG.SYS file:

	FILES=20
	BUFFER=20

After modifying AUTOEXEC.BAT and CONFIG.SYS, reboot your computer.
 
NOTE: INSTALL.BAT does not install the source files for the
sample applications. You will need to copy these files as
needed using the DOS COPY command. 

See Chapter 1 of the Microsoft Windows Programmer's Utility Guide 
for more information about installing the Software Development Kit.

For a complete listing of files in the Software Development
Kit, see the file FILELIST.TXT on disk 2.

--------------------------------------------------------------
Update to Windows 1.03

The file UPDATE.TXT (on disk 2) contains a list of changes and 
additions to the Microsoft Windows Programmer's Reference and 
Microsoft Windows Programming Guide for version 1.03. This file
is a text copy of the document "Update to Programmer's Reference
and Programming Guide" provided with the Software Development
Kit.

--------------------------------------------------------------
Changes Regarding Windows 1.04

The file 104_NEWS.TXT ( on disk 2) contains a small list of
of differences between Windows 1.03 and Windows 1.04.

--------------------------------------------------------------
Creating a "Slow Boot" Version of Windows

Some developers may want to create a "slow boot" version of Windows.
It is called a slow boot version because it loads the
individual drivers and dynamic-link libraries of Windows
separately. This takes longer to load and start than the
version of Windows created by the Setup program. The advantage
of the slow boot version is that you can change individual
parts of Windows quickly without having to run the Setup
program again. For example, if you change your display adapter
you just change your display driver file and Windows is ready
to go.

To create a slow boot version of Window, do the following:

1.	Copy all files from your Windows version 1.03 Setup 
	and Build disks to a directory on your hard disk. 

2.	Replace KERNEL.EXE with the debugging version of KERNEL.EXE 
	found on disk 5 in the Software Development Kit.

3.	Select the keyboard, mouse, and display drivers you
	wish to use with Windows and rename them KEYBOARD.DRV,
	MOUSE.DRV, and DISPLAY.DRV. For example, if you want 
	drivers for a United States keyboard, a Microsoft Mouse, 
	and a IBM Color Graphics Adapter, rename the files
	USA.DRV, MOUSE.DRV, and CGA.DRV.

4.	Rename the LOFONTS.FON or HIFONTS.FON file to FONTS.FON.  
	Use LOFONTS.FON for low resolution displays, such as
	the Color Graphics Adapter, and HIFONTS.FON for high
	resolution displays such as Hercules Graphics Card.

5.	Rename the .GRB file that matches your display driver to
	DISPLAY.GRB.  For example, if your display driver is
	CGA.DRV, rename CGA.GRB.

6.	Rename the .LGO file that matches your display driver to
	DISPLAY.LGO.  For example, if your display driver is
	CGA.DRV, rename CGA.LGO.

7.	Create a dummy WIN100.OVL file by typing the command:

		echo >WIN100.OVL
	
You start the slow boot Windows by using KERNEL.EXE instead of 
WIN.COM. Type the following:

	kernel [<program-name>]

where <program-name> is the name of the Windows application
you wish to start with Windows. If you do not specify a
program name, the kernel starts the MS-DOS Executive by
default.

--------------------------------------------------------------
Creating Windows "Quickly"

You can use the Setup program to quickly create a version 
of Windows by specifying the /Q option on the Setup command line.  
The option directs Setup to create Windows without prompting
the user for input. The option is intended to be used to
create a version of Windows from a hard disk or high-capacity
floppy disk.

When the /Q option is given, Setup begins creating Windows
without prompting for input from the user. Instead it uses the
arguments given on the Setup command line as responses to the
questions it would otherwise have asked. 

The syntax for the Setup program with the /Q option is:

  SETUP /Q <pathname> <keyboard-driver> <mouse-driver> <display-driver>

where
 <pathname> is the pathname of the directory to receive Windows.

 <keyboard-driver> 
	    is the file name of the keyboard driver to install.
	    For example, specifying USA.DRV causes the United States 
	    keyboard driver to be installed. 

 <mouse-driver> 
	    is the file name of the mouse driver to install.
	    For example, specifying NOMOUSE.DRV causes the null 
	    mouse driver to be installed (that is, no mouse
	    present). 

 <display-driver> 
	    is the file name of the display driver to install.
	    For example, specifying CGA.DRV causes the Color
	    Graphics Adapter driver to be installed. 

If the specified files are not present or if any other required file
is not found, Setup displays an error message and terminates.

IMPORTANT: Before using the /Q option you must copy all files from the
Windows version 1.03 Setup and Build disks to a directory on your
hard disk or high-capacity floppy drive. 

--------------------------------------------------------------
Creating a Debugging Version of Windows

You can use the MKDEBUG.BAT file, found on disk 2, to create a 
debugging version of Windows. This file transfers the debugging 
KERNEL.EXE, USER.EXE, and GDI.EXE files to a copy of the Windows 
version 1.03 Setup and Build disks.  This lets you run the Setup 
program and create a debugging version of Windows.

IMPORTANT: Use only copies of the original Windows version
1.03 Setup and Build disks with the MKDEBUG.BAT file. Keep the 
originals in a safe place. Use them only when you want to make 
a nondebugging version of Windows.

MKDEBUG.BAT removes the RAMDRIVE.SYS file from your Setup disk
to make room for the debugging version of KERNEL.EXE.

For an explanation of how to create a debugging version of Windows, 
see Chapter 1 of the Microsoft Windows Programmer's Utility Guide. 

--------------------------------------------------------------
Patching the Kernel for Pascal Support

Before using the debugging version of the KERNEL.EXE file to
debug Windows applications written in Pascal, you should modify
KERNEL.EXE using the 103PATCH.BAT batch file. This file uses
the DEBUG.EXE program to patch the kernel and prepare it for
Pascal applications.

To use 103PATCH.BAT, copy the file from disk 4 to the
directory containing KERNEL.EXE. Make sure that DEBUG.EXE is in 
a directory specified by your PATH environment variable. Type

	103PATCH.BAT

The batch file creates a backup file KERNEL.BAK and then
modifies KERNEL.EXE.

Note that 103PATCH.BAT replaces the patching programs PATCHDBG.EXE 
and PATCHRTL.EXE named in Chapter 1 of the Microsoft Windows 
Programmer's Utility Guide.

--------------------------------------------------------------
Dialog Editor: Scroll Bars

The Dialog Editor treats dialog units in a slightly different
way than Windows. This may have an impact on the positioning
of scroll bars and other controls within a dialog box created
by the Dialog Editor.

--------------------------------------------------------------
Resource Compiler, Rc 

As noted in Chapter 3 of the Microsoft Windows Programmer's Utility
Guide, backslashes used as pathname separators with the 
#include statement must be escaped, that is, they must be
given twice. For example the statement 

	#include "c:\foo\bar.h" 

is not correct, it should be

	#include "c:\\foo\\bar.h"

Backslashes used as separators in statements such as RCINCLUDE 
and ICON statements must not be escaped. The following
examples are correct:

	RCINCLUDE \foo\bar.dlg 
	hello ICON c:\foo\hello.ico


Nested RCINCLUDE statements are not allowed.

--------------------------------------------------------------
The Font Editor, FONTEDIT.EXE

The Font Editor's maximum size for font files is 64K bytes.  This size 
includes the font bitmap and the font file header.

The Font Editor's maximum character width is 64 pixels. The
maximum character height is 32 pixels.

The Font Editor will prompt you to save changes to the current
font file if you attempt to close the editor or exit Windows without 
first saving the changes. You have the option of saving the
changes, ignoring the changes, or canceling the command to
close or exit.
 
The Width command in the Width menu displays a dialog box in
which you can enter the new width in pixels.

If you copy and paste a character from one part of a
variable-width font to another, the character retains its
original width.

--------------------------------------------------------------
INCLUDE Variable 

The Microsoft C and Pascal compilers and the Microsoft Macro 
Assembler look for include files in the directories specified by the 
INCLUDE environment variable.  A good way of handling common include 
files, such as WINDOWS.H and CMACROS.INC, is to put them in an 
\INCLUDE directory and add the statement 

	set INCLUDE=\INCLUDE

to your AUTOEXEC.BAT file.

-------------------------------------------------------------
Sample Applications

The Software Development Kit contains the source files for a
variety of sample applications. These samples are intended to
demonstrate some of the most important features of Windows as
well as how to use Window functions and messages in C and Pascal 
applications.

There are the following sample applications:

 HELLO	This application shows the general form of a Windows 
	application, the WinMain function, how to create a window, 
	and the window function.

 TYPE	This application shows how to process input from the keyboard 
	and how to display it in a window.

 SHAPES	This application shows how to use a menu and how to draw 
	interesting shapes in a window. 

 TRACK	This application shows how to use input from the mouse and 
	how to give visual feedback to the user as he moves the mouse.

 FONTTEST	
	This application shows how to create dialog boxes and process 
	user input from the boxes. It also shows how to create and 
	select fonts.

 TEMPLATE	
	This empty application is a template which application 
	developers can use to start development of their own 
	applications. 

 MAPMODES	
	This application shows the effect of the various mapping modes 
	on output in a window. The mapping modes, an important part of 
	Windows, let applications change the size and positioning of 
	their output without changing the data used to generate the 
	output.

 MOTION	This application shows how to create and use use dynamic-link 
	libraries with an application. It also illustrates the effect 
	of code motion and loading on demand.

 SAMPLE	This application is similar to the template application but 
	contains additional features such as standard File and Edit 
	menus and dialog boxes.

 CLOCK	This application is similar to the Clock application of the 
	Windows version 1.03 Desktop Applications.  Unlike the
	desktop application, this Clock has two different methods of 
	drawing the iconic clock.  A menu lets the user choose 
	drawing the clock directly to the screen or to the
	screen through a bitmap.  It is an example of how 
	to use bitmaps and compatible device contexts.

 CARDFILE	
	This application is similar to the Cardfile application of the 
	Windows version 1.03 Desktop Applications. It shows how to 
	combine the features demonstrated in other sample
	applications to make a full-scale application.  

 MUZZLE	This Pascal application shows the general form of a Windows 
	applications and how to create and use a window.

All sample applications, except MUZZLE, are written in the C language. 
Some applications also have assembly-language support routines. 

Each application comes with a "make file" which you can supply
to the Make program to automatically compile and link the
sample application. For example, to compile and link the HELLO
application, copy all HELLO source files to a directory on
your hard disk and type

	make hello

The make files for the hello application and most other applications 
assume that you have installed the Software Development Kit and the 
Microsoft C compiler version 4.0. Some applications assume
that you have also installed the Microsoft Macro Assembler
version 4.0.

For more information about the HELLO, TYPE, SHAPES, TRACK, and FONTTEST 
applications, see the Microsoft Windows Programming Guide.

--------------------------------------------------------------
Building MUZZLE.EXE, Pascal Sample Application

When building MUZZLE with the file support turned on, the Pascal
compiler generates a warning which halts compilation after the first
pass.  If the compilation is completed by hand (by typing "pas2") the
build will continue successfully.

--------------------------------------------------------------
Slap Junior Program (SLAPJR.EXE) 

The SlapJr application lets you capture the contents of the
display screen, a window, or a client area to the clipboard.
From the clipboard, you can paste the screen into Paint's
canvas and print the screen using Paint's Print command in the
File menu. 

To use SlapJr, double-click the filename SLAPJR.EXE in the
MS-DOS Window, or with the keyboard, use the cursor keys to select
the filename and press ENTER.

WARNING: Do not attempt to start SlapJr by using the Run or
Load commands in the MS-DOS Executive's File menu. Using these
commands may cause an unrecoverable system error.

SlapJr displays a dialog box explaining what keys you may use
to capture all or a portion of the display screen to the
clipboard.  Click the Ok button to proceed.

Once SlapJr is loaded and running you can use the following
keys to capture:

	Use		To
	---		--------------------------
	CTL_F9		Capture the client area of the current window
	ALT_F9		Capture the entire display screen
	SHIFT_F9 	Capture the current Window

NOTE: You may need to modify the output mode options of your
printer to accommodate printing a window or the entire display
screen. For example, you may need to set the printer
resolution to a greater value to ensure that the entire screen
can be printed on a page.  To set the printer output modes,
use the Printer command in the Setup menu of the Control Panel
program. See the Microsoft Windows User's Guide for more
details.

--------------------------------------------------------------
The WINDWP.C File

This file contains the current source for the DefWindowProc
function used for default message processing in all
applications. The file is intended to be used by application
developers to determine what messages are processed by
DefWindowProc and what the default processing for those
messages is. The default processing for individual messages 
can also be found in Chapter 8 of the Windows Programmer's
Reference. WINDWP.C is on disk 7 in \USER.

--------------------------------------------------------------
The COMM.C File

This file contains the current source for the serial
communication portion of the Terminal application. The file is
intended to be used by application developers to determine how
the communication functions provided with Windows are used to
access serial communication devices. The functions are also described
in Chapter 4 or the Windows Programmer's Reference. COMM.C is
on disk 7 in \TERMINAL.

--------------------------------------------------------------
Preload Data Segments for Dynamic-Link Libraries

When creating dynamic-link libraries make sure that the library
data segment (if any) is given the PRELOAD attribute. For
example, the MOTLIB1.EXE and MOTLIB2.EXE libraries called by the
MOTION.EXE sample program must have PRELOAD data segments.

--------------------------------------------------------------
C Runtime Functions, SS != DS Restrictions

IMPORTANT: Although the C runtime functions listed in Appendix C of the
Windows Programmer's Utility Guide can be used in dynamic-link 
libraries (Windows libraries), you must not pass stack addresses
as arguments to these functions. Also you must not assume that
returned pointers reference stack addresses. For more information 
about C runtime function restrictions, see Chapter 2 of the Microsoft 
Windows Programmer's Utility Guide.
--------------------------------------------------------------
