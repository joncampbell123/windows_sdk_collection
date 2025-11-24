========================================================================
      MICROSOFT FOUNDATION CLASS LIBRARY : EXAMPLE CODE
========================================================================

I. Introduction

This directory contains all of the Microsoft Foundation Class (MFC)
library example programs, which highlight many object-oriented
Windows programming techniques and class library features.  Each 
example is a complete and functional program that explores
different aspects of the library.

These programs are useful as learning tools, when used in conjunction
with the MFC reference documentation.  You can take the code provided
and experiment with it.  One group of sample programs, in the TUTORIAL
subdirectory, supplements the MFC Tutorial and should be studied in
conjunction with that document.

Each subdirectory has both PWB-compatible and NMAKE-compatible makefiles.
The PWB makefiles can be identified by the .MAK file extension.  NMAKE
makefiles are called 'makefile', with the exception of the TUTORIAL
subdirectory (the difference is explained in TUTORIAL\README.TXT).

-----------------------------------------------------------------------
Windows and C runtime libraries referenced by the makefiles should
be installed as part of the NT setup.  These libraries can be found
in \MSTOOLS\LIB.
-------------------------------------------------------------------------

II. Compiling sample programs with Microsoft NMAKE

To compile a sample program with NMAKE:

1. Ensure you have the appropriate C runtime and Foundation
runtime libraries.  All MFC library variants necessary to build
the samples are provided in binary form in MFC\LIB.


2. Ensure you have set up your LIB, INCLUDE, and PATH environment variables
appropriately.  For example:

   set LIB=C:\MSTOOLS\LIB;C:\MSTOOLS\MFC\LIB
   set INCLUDE=C:\MSTOOLS\H;C:\MSTOOLS\MFC\INCLUDE
   set PATH=%PATH%;C:\MSTOOLS\BIN

Remember you must put the MFC\LIB and MFC\INCLUDE directories in
your LIB and INCLUDE paths, respectively, if you wish to compile
Foundation applications.

3. Change your working directory to the appropriate sample file 
directory and invoke NMAKE.  

In most cases, invoking NMAKE with no arguments will compile and 
link the retail version of that directory's sample application.  
If you wish to build the debug version, specify 'DEBUG=1' on the 
NMAKE command line.  If you want to remove object, compiled resource, 
and executable files, specify the 'clean' target.  For example:

  cd \mstools\mfc\samples\about2
  nmake                       ' creates retail version of about2.exe
  nmake clean                 ' removes about2.exe/.obj/.res
  nmake DEBUG=0               ' creates retail version of about2.exe
  nmake clean                 ' removes about2.exe/.obj/.res
  nmake DEBUG=1               ' creates debug version of about2.exe

Object and resource files are created in the current directory.

Compilation procedures for the tutorial subdirectory are slightly
different than is documented here.  See MFC\SAMPLES\TUTORIAL\README.TXT
for more information.

NOTE: See technical note MFC\DOC\TN007.TXT for special information
on setting up your Windows environment to handle Foundation library
debugging output.


-----------------------------------------------------------------------

III. Samples

Here is a list of the sample directories and an overview of their
content.  See below for more detailed explanations

	ABOUT2      simple dialog box example.
	CHART       a simple bar/line charting application.
	CTRLTEST    a control test driver showing custom controls.
	FILEVIEW    a simple text file viewer.
	HELLO       basic application described in the MFC Tutorial.
	HELLOAPP    an extremely simple MFC application.
	MDI         demonstrates how to program to the MFC MDI interface.
	MINMDI      a bare-bones MDI application.
	MINSVR      a minimal OLE server application.
	MINSVRMI    a minimal OLE server application with multiple inheritance.
	MULTIPAD    an MDI NOTEPAD application.
	OCLIENT     an example of an Object Linking and Embedding (OLE) client.
	OSERVER     a simple OLE server application (called BIBREF)
	SHOWFONT    a font attribute viewer.
	TEMPLDEF    a non-Windows tool for expanding C++ templates.
	TRACER      a Windows utility to view and set diagnostic trace options.
	TUTORIAL    source for tutorial examples.

-------------------------------------------------------------------------

ABOUT2\ABOUT2.EXE

This program is a re-implementation of the ABOUT program found in
Charles Petzold's book, "Programming Windows"; it has been written
using C++ and the Foundation classes.  ABOUT2 allows you to draw
either a rectangle or an ellipse in one of several colors in the
client area of a application main window.  The source code
illustrates the following concepts:

    - Simple Foundation application structuring.
    - Simple dialog box initialization and management.
    - Graphics Device Interface (GDI) wrapper classes.

-------------------------------------------------------------------------

CHART\CHART.EXE

This program implements a simple bar/line charting application.
You can enter a data set (list of integers), display the data in
bar or line chart form, print the chart, save the data to disk, 
and read in previously saved data.  The source code illustrates
the following concepts:

    - Dialog box management.
    - Document state management (dirty, clean).
    - Using GDI calls to draw a graph to a device context.
    - Using Foundation CFile and CArchive classes to save and
      restore user data.
    - Printing.

-------------------------------------------------------------------------

CTRLTEST\CTRLTEST.EXE

The main application (in DCONTROL.H, DCONTROL.CPP and DCONTROL.RC)
provides a simple frame window with a single menu to drive the tests.
This can be easily extended to drive additional tests.

All the examples are based off a the class CParsedEdit which
is derived from the standard Windows CEdit class.  CParsedEdit
provides a simple keyboard input filter to only allow numbers,
letters, control characters, combinations of the above or any
characters.

This sample also includes MUSCROLL.DLL, the "micro scrolling"
custom control provided in the Windows 3.1 SDK, as well as tests
for Pen Windows edit items (requires Windows for Pen).

The source code illustrates the following concepts:

	- example parsed edit control (CParsedEdit) derived from
		the standard Windows CEdit class.
	- bitmap buttons
	- owner draw/self draw controls
	- owner draw/self draw menus
	- using C++ to create controls for a dialog using Create
		member functions for the controls (not recommended,	
		DERTEST.CPP, DERTEST.DLG)
	- exporting custom controls and registering a new WndClass
		so the controls can be used by the dialog manager
		(the "PAREDIT" control class used in WCLSTEST.CPP
		and WCLSTEST.DLG).  These controls can be edited with the
		SDK dialog editor (DLGEDIT.EXE) as custom controls
		by typing in the class name "PAREDIT" and the
		hex representation of the PES_ styles.
	- using SubclassWindow/SubclassDlgItem to dynamically
		subclass a dialog control to add specialized behaviour
		(SUBTEST.CPP, SUBTEST.DLG).
	- example of an external control packed as a DLL (MUSCROLL.DLL)
		being used by C++ code with a special C++ wrapper class.
	- an example of a spin button using the external MUSCROLL.DLL.
	- examples of Windows for Pen special edit controls

	more advanced topics:
	- ON_CONTROL handler (in WCLSTEST.CPP responding to new
			control notification PEN_INVALIDCHAR).
	- example of how style bits (PES_ style bits for the parsed
		edit) can be stripped off before the normal CEdit
		control is created.

The source code to this application should be read along with
Foundation technical note 14 (MFC\DOC\TN014.TXT) describing
custom controls and other topics.


-------------------------------------------------------------------------

FILEVIEW\FILEVIEW.EXE

This program implements a simple text file viewer.  Unlike 
Windows NotePad, this program places no limit on the size 
of the text file that may be viewed.  This program is for
viewing text files only, and not editing them.  The source code
illustrates the following concepts:

    - Foundation application structuring.
    - Use of the Foundation file classes as base
      classes for derivation and specialization.
    - Use of the GDI classes for graphical output.
    - Use of Message Maps for handling window scrolling.


-------------------------------------------------------------------------

HELLO\HELLO.EXE

Described in the MFC Tutorial, this application shows the basics of
using the Microsoft Foundation Class Library to write applications
for the Microsoft Windows environment.  This application creates and
displays a fully-functional frame window, which in turn displays a
text string in its center.  The source code illustrates the following
concepts:

    - Simple Foundation application structuring.
    - Integrating Windows resources with your application.

-------------------------------------------------------------------------

HELLOAPP\HELLOAPP.EXE

This is an extremely simple MFC application that simply creates a
main frame window with the caption 'Hello World!'.  It is compiled
using retail libraries only.

-------------------------------------------------------------------------

MDI\MDI.EXE

The MDI application demonstrates how to program to the MFC wrapper of 
the Windows Multiple Document Interface (MDI) wrapper.  When started,
the MDI application provides an MDI frame window, and two kinds of MDI
child windows that you may open within the frame.  One child window
is similar to the HELLO example program (it displays "Hello World"
in its client area).  The other kind of child window contains a 
bouncing ball.  The source code illustrates the following concepts:

    - Foundation MDI application structuring.
    - Coordinating MDI frame and child classes.
    - Using Windows timers.
    - Using the CBitmap class for constructing and drawing a
      bit pattern into a device context.

-------------------------------------------------------------------------

MINMDI\MINMDI.EXE

This is a bare-bones MDI application that concentrates on showing the
minimum work needed to set up and maintain MDI child windows within
an MDI frame window.  You can use this application as a starting point
for your own experimentation with the MDI interface.  The source code
illustrates the following concepts:

    - Foundation MDI application structuring.
    - Coordinating MDI frame and child classes.

-------------------------------------------------------------------------

MINSVR\MINSVR.EXE

MINSVR is a minimal OLE server application that implements
a graphical ellipse drawing.  When running MINSVR *directly* (from
the program manager, file manager, or an icon), MINSVR is registered
with the registration data base.  YOU MUST RUN MINSVR DIRECTLY before
you can use it with other OLE programs.  Once you have run MINSVR
directly, you can use it with OLE Clients where it provides basic
embeddable object services.

The source code illustrates the following concepts:

    - Constructing a minimal OLE server
    - Programming with the MFC OLE classes
    - Supporting embedded objects

It is highly recommended that you read Foundation technical notes 8, 9,
and 10 (MFC\DOC\TN008.TXT, etc.) for more information on the MFC OLE
classes and constructing OLE clients and servers using MFC.


-------------------------------------------------------------------------

MINSVRMI\MINSVRMI.EXE

MINSVRMI is a simple OLE server application that implements a
graphical elliptical drawing.  It is the same program as MINSVR
except it is implemented using multiple inheritance.  When MINSVRMI is
*directly* run, it registers itself with the registration database. 
YOU MUST RUN MINSVRMI directly before using it with other OLE
programs.  When run from an OLE client, MINSVRMI allows you to embed
objects in the client application's document.

The source code illustrates the following concepts:

    - Constructing a minimal OLE server
    - Programming with the MFC OLE classes
    - Using multiple inheritance with MFC and MFC OLE classes
    - Supporting embedded objects

It is highly recommended that you read Foundation technical notes 8, 9,
and 10 (MFC\DOC\TN008.TXT, etc.) for more information on the MFC OLE
classes and constructing OLE clients and servers using MFC.

You should also read Foundation technical note 16 (MFC\DOC\TN016.TXT)
for more information on MFC and multiple inheritance.

-------------------------------------------------------------------------

MULTIPAD\MULTIPAD.EXE

Microsoft Windows provides a simple application called NOTEPAD.  The
MFC library adaptation of this program uses the MDI paradigm to allow
the user to manipulate any number of concurrently open text files. 
MULTIPAD is fully functional and includes printing support as well as
other features usually found only in commercial Windows applications:
a status bar at the bottom of the window, and a cache of
most-recently-used filenames (saved between program invocations) so
that users may quickly access recently used documents.  The source
code illustrates the following concepts:

    - Non-trivial MDI application structuring.
    - Menu command dispatching.
    - Dialog box initialization and management.
    - Printing.
    - Porting existing Windows code for use with MFC.
    - Swap tuning an application for Windows for maximum efficiency.


-------------------------------------------------------------------------

OCLIENT\OCLIENT.EXE

OCLIENT is an example of an Object Linking and Embedding (OLE) client
application.  It uses the Foundation OLE classes.  This program is a port
of the OLEDEMOC program which is shipped by the Microsoft OLE Software
Development Kit.  The program allows you to insert both embedded and
linked objects into a document.  OCLIENT also allows you to perform both
file and clipboard operations on the objects as well as activate the
servers to which the objects belong. The source code illustrates the
following concepts:

    - OLE client programming using MFC
    - Inserting embedded objects into a client
    - Pasting linked and embedded objects into a client
    - Copying objects to the clipboard
    - Loading and saving objects to and from disk

It is highly recommended that you read Foundation technical notes 8, 9,
and 10 (MFC\DOC\TN008.TXT, etc.) for more information on the MFC OLE
classes and constructing OLE clients and servers using MFC.

-------------------------------------------------------------------------

OSERVER\BIBREF.EXE

BIBREF is a simple OLE server application that implements
bibliographical references.  When BIBREF is *directly* run, it
registers itself with the registraton database and allows you to add,
modify, and delete bibliographical references from a list maintained
by the application.  YOU MUST RUN BIBREF DIRECTLY before using it
with other OLE programs.  When run from an OLE client, BIBREF allows
you to embed objects containing these references in the client
application's document.

The source code illustrates the following concepts:

    - Constructing an OLE server
    - Programming with the MFC OLE classes
    - Supporting embedded objects

It is highly recommended that you read Foundation technical notes 8, 9,
and 10 (MFC\DOC\TN008.TXT, etc.) for more information on the MFC OLE
classes and constructing OLE clients and servers using MFC.


-------------------------------------------------------------------------

SHOWFONT\SHOWFONT.EXE

This program is a C++/Foundation adaptation of a sample application
provided in the Microsoft Windows 3.0 Software Development Kit (SDK).
SHOWFONT is a font attribute viewer.  It allows you to quickly
determine the visual and logical characteristics of Windows GDI FONT
resources.  The source code illustrates the following concepts:

    - Modal and modeless dialog box management.
    - CFont attribute manipulation.
    - Menu command dispatching.


-------------------------------------------------------------------------

TEMPLDEF\TEMPLDEF.EXE

TEMPLDEF is a non-Windows tool that helps you write and use 
template-like classes.  Templates are a proposed (but not established)
C++ language feature that are very useful for industrial-strength
programming.  The MFC library's collection classes use template
classes (and the tool provided in this directory) to create specific
collection types from general collection types.

The C++ language will someday support such features, but this tool
makes some of these proposed advantages available now.  TEMPLDEF
reads a "template" file, and writes a new C++ class which is
type-safe.

After you have built the templdef tool, you can use the MKCOLL.BAT
batch file and the array, list, and map template files (all in the
TEMPLDEF subdirectory) to generate your own versions of arrays,
lists, and maps.

For more information on the templdef tool, see MFC Technical Note #4,
"Template Classes and AFX" (located in MFC\DOC\TN004.TXT).  For more
information on proposed C++ templates, see Chapter 14 of "The Annotated
C++ Reference Manual," by Ellis and Stroustrup.



-------------------------------------------------------------------------

TRACER\TRACER.EXE

TRACER is a tiny Windows utility that allows you to view and set the 
Foundation Windows diagnostic trace option flags described in 
MFC\DOC\TN007.TXT.  The source code illustrates the following concepts:

    - Reading and writing Windows profile strings
    - Writing a Foundation application that uses a modal dialog
      box but does not use the regular Windows/MFC message pump.


-------------------------------------------------------------------------

TUTORIAL\PHBOOK.EXE

PHBOOK is a phone list maintenance application, and is the subject
of the MFC tutorial.  PHBOOK allows you to create, edit, print and
save lists of people's names and their telephone numbers.  In addition
to MFC application programming techniques, this application demonstrates 
the development of an abstract data model and the coupling of that
data model to a graphical user interface implemented with MFC.  See
TUTORIAL\README.TXT for more information on building this application.
The source code illustrates the following concepts:

    - MFC Application organization.
    - Printing.
    - Dialog box management.
    - Coupling an abstract data model to a user interface.
    - Using MFC object serialization (CArchive class) to save
      and load user data.

-------------------------------------------------------------------------
