========================================================================
	  MICROSOFT FOUNDATION CLASS LIBRARY : ABOUT2 EXAMPLE PROGRAM
========================================================================

This application is exactly like the "About2" sample written by
Charles Petzold in his book, "Programming Windows."  This version was
written in C++ and uses the Foundation classes.  Compare the code of
these two versions to see how Windows applications written in C can be
adapted to use the features of the Microsoft Foundation Class Library.

MAKEFILE.
ABOUT2.MAK
ABOUT2.STS
	These describe the compile process for the NMAKE tool and the
	Programmer's Workbench.  MAKEFILE is an NMAKE-compatible makefile,
	while ABOUT2.MAK and ABOUT2.STS are the PWB-compatible makefile and
	PWB status file, respectively.

ABOUT2.H
ABOUT2.CPP
	These two files make up the entire behavior of the application.
	The application's classes are declared in the ABOUT2.H header
	file, and the member functions and message maps are defined in
	ABOUT2.CPP.

	Note that the CAbout2Dlg class's constructor, declared in ABOUT2.H,
	uses the constructor of the base class, CModalDialog (a Foundation
	class).  Normally, the CModalDialog's constructor requires the name
	of the dialog template resource.  CAbout2Dlg objects will always be
	associated with the "AboutBox" template (found in ABOUT2.RC), so
	the new constructor supplies this name explicitly.  Only the parent
	window argument is then required by the creator of these objects.

	The CAbout2Dlg also has some member variables.  These variables
	hold the state of the current color and figure, for the duration of
	the dialog.  When the dialog is invoked, the global variables
	nCurrentColor and nCurrentFigure are copied to these member
	variables, and if the user presses OK in the dialog, they are
	copied back into the global variables.  A modal dialog box should
	not modify the actual variables, but rather keep a separate copy
	that can be disposed of in case the dialog's Cancel button is
	pushed.

	A function is defined in ABOUT2.CPP which is not a member of either
	of the two classes in the application.  This is called Paint, and
	it is used by both classes to draw a given figure in a given color
	in any window's display context.  The dialog calls Paint to draw the
	figure according to the settings that the user makes in the dialog.
	Once the dialog is dismissed, the frame window uses Paint to draw
	the same figure in its client area.

	(For the extremely observant:  you may notice that the Paint
	function is slightly different in the C++ version from Charles
	Petzold's original PaintWindow.  The original version always
	creates a new display context in which to draw.  This version takes
	a display context (such as the CPaintDC of the frame window) if one
	is available, and creates a CDC if none is given.  This avoids
	obtaining and releasing extra display contexts from Windows.
	Painting during an OnPaint/WM_PAINT message should be done with the
	display context provided by CPaintDC/BeginPaint, to avoid unwanted
	flashing.)

RESOURCE.H
	This file defines control IDs used in the About dialog box,
	and the application's main window menu bar.

ABOUT2.ICO
	This is an icon file, which is used by the frame window, and also
	appears in the About dialog box.

ABOUT2.RC
	This is a listing of all of the Microsoft Windows resources which
	the program uses.  It includes two resources: the icon found in
	ABOUT2.ICO, and the application's main menu.

ABOUT2.DLG
	This file contains dialog control content and layout information.
	It was generated directly by the DLGEDIT dialog editor, part of
	the Microsoft Windows 3.1 Software Development Kit.  ABOUT2.DLG is
	processed by the Microsoft Windows Resource Compiler (it is
	included by ABOUT2.RC).

ABOUT2.DEF
	A typical module definition file, used by LINK to set specific
	application information in the executable file.
