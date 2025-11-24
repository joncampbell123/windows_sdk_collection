========================================================================
	   MICROSOFT FOUNDATION CLASS LIBRARY : HELLO EXAMPLE PROGRAM
========================================================================

This application is a simple Windows program to demonstrate the basics
of using the Microsoft Foundation classes.  The Microsoft Foundation
Class Library Tutorial describes this application in detail, but here's
a summary of what you will find in each of the files that make up Hello.

MAKEFILE.
HELLO.MAK
HELLO.STS
	These two files make it easy to compile the application.  The
	MAKEFILE describes the build process for the NMAKE tool, and the
	other files, HELLO.MAK and HELLO.STS, make it easy to use the
	Programmer's Workbench to build HELLO.

	To use the MAKEFILE, just go to the DOS prompt, and type
		nmake
	to build the program.  You can choose to include
	debugging support by using the option:
		nmake DEBUG=1
	(Make sure you use uppercase letters for this option.) 
	The DEBUG=0 option specifies that no support is compiled into the
	code.  If you do not specify a DEBUG option, the default is 
	DEBUG=0.


HELLO.H
HELLO.CPP
	These two files make up the entire behavior of the application.
	In the HELLO.H header file, two C++ classes are declared: CTheApp
	and CMainWindow.  These classes override and extend the behavior
	of their base Foundation classes, CWinApp and CFrameWnd,
	respectively.  The source file HELLO.CPP contains the member functions
	and message maps declared in the header file.

	CWinApp, a Foundation class, does all of the typical stuff that
	most Microsoft Windows applications must:  it initializes itself,
	then it creates and runs a message loop until the application ends.
	The Hello program extends this behavior by overriding the
	InitInstance member function in our own class, CTheApp.  This
	member function is automatically called during startup of the
	application.  In InitInstance, we create and show our main window.

	CMainWindow is a pretty simple window class, and it follows the
	typical behavior found in frame windows.  It is based on CFrameWnd,
	a Foundation class.  Our frame window uses a message map to
	associate member functions with Microsoft Windows messages.  These
	functions respond to the standard paint message, and a menu choice
	command message.

	Since the Foundation provides all of the normal code for the
	WinMain function and other initialization (in the CWinApp class),
	it is only necessary to create an object of our CTheApp class to
	start the program and let it run.  This is found in the one global
	variable in HELLO.CPP, called theApp.

HELLO.ICO
	This is an icon file, which is used by the Hello frame window.

HELLO.DLG
	This is a dialog definition file, which defines the
	characteristics and layout of the modal dialog AboutBox,
	which is displayed by the Hello application.  This dialog file
	was generated using the Windows 3.1 SDK DLGEDIT tool.


RESOURCE.H
	This is a header file that contains Windows menu item IDs.
	It was generated using the Windows 3.1 SDK DLGEDIT tool.

HELLO.RC
	This is a listing of all of the Microsoft Windows resources which
	the program uses.  It includes three resources: the icon found in
	HELLO.ICO, a menu, and the dialog defined in HELLO.DLG.

HELLO.DEF
	This file contains parameters the linker needs to correctly
	link a Microsoft Windows application, including: the name and
	description of the application, and the size and type of the runtime
	heap and runtime stack.  The numbers in this file are typical for
	small applications made with the Microsoft Foundation Class Library.
