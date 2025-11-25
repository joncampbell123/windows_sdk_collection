Windows Sample Framework
========================

This document contains the following sections:

    Overview
    Graph
    Brief Sample Descriptions
    Building the Samples
    Architectural Approach
    File Specific Information

Overview
--------

The sample framework has been designed to illustrate many
different Windows programming concepts. This includes samples
that are very basic, as well as samples that demonstrate the
new Windows 95 and Windows NT controls and functions. It is
designed to help both novice and advanced Windows programmers.

The Graph
---------

As you can see from the graph below (shown only in README.WRI),
most samples in the framework are "derived" from the Generic and
Gen32 samples. There are four samples (SIMPLE, INPUT, OUTPUT, and
MENU95) that do not derive from Generic.

Brief Sample Descriptions
-------------------------

ANALOG    - Common controls: status bar, progress bar,
            tracker (slider), up/down (spin).
BARMDI    - Status bar and tool bar in MDI application.
            Also shows ToolTips.
BARSDI    - Status bar and tool bar in SDI application.
            Also shows ToolTips.
BLANDMDI  - Basic MDI application.
CLIPTEXT  - Clipboard sample.
COMMONDG  - Common dialog sample.
DIALOG    - Modal and modeless dialog boxes.
DLLSKEL   - Win32 DLL sample.
EDITSDI   - Notepad like sample in SDI application.
FILESYS   - Shows basic implementation of some of the new
            File System functions.
GDIDIB    - Demonstrates the CreateDIBSection function.
GDIINPUT  - GDI sample showing lines, points, beziers,
            brushes, and pens, User input with mouse here.
GDIMETA   - Shows how to create, load, and save enhanced
            metafiles.
GDIOUT    - Simple GDI drawing/output functions. No
            user input here.
GDIPAL    - Shows the use of the Windows palette manager on
            palette devices. User can choose pen and brush
            colors from the system palette.
GDIPRINT  - Shows how to print DIBs and metafiles.
            NOTE: At this time, this sample does not print
            metafiles.
GEN32     - 32 bit only version of Generic.
GENERIC   - Basic windows application with About box.
HOOKS     - Comprehensive sample showing a Spy-like utility.
IATHREAD  - Interactive thread sample. Shows multithreaded MDI
            application, with user set thread priority.
INITREE   - Shows use of TreeView control by enumerating the .INI
            files in the Windows 95 directory. Use of ImageLists
            is also demonstrated.
INPUT     - Shows the use of simple input via mouse, keyboard, and timer.
LISTCTRL  - Demonstrates the ListView control.
            The ListView control also uses an ImageList.
MENU95*   - Shows several ways of using and manipulating
            menus, including handling menu commands from
            menu bars and pop menus, inserting menus on the
            fly, modifying menus, and implementing owner
            draw.
MINIHELP  - Shows how to call the Windows Help Engine from
            an application.
MIXTREE   - Demonstrates use of TreeView control by enumerating
            the mixer lines and controls of a mixer device using
            the Mixer Services. If a mixer device is not present,
            simulation data is used to generate the TreeView control
            and text displays. Incorporates TreeView notifications
            and recursive functions to traverse the TreeView's items.
MULTIPAD  - MDI version of EDITSDI. Notepad-like sample.
OUTPUT    - Shows the use of two fundamental text drawing
            functions and several line and shape drawing functions.
PROPSHET  - Simple implementation of a property sheet control.
PVIEW95*  - Sample that enumerates processes and threads.
            Uses 32-bit TOOLHELP API. Allows you to kill processes.
RTFEDIT   - Basic demo of the new RTF edit control.
SIMPCNTR  - Simple OLE2 container.
SIMPLE    - Shows the use of two fundamental text drawing
            functions and several line and shape drawing functions.
SIMPSVR   - Simple OLE2 Server.
SUBCLS    - Subclasses a list box and an edit control.
TABCTRL   - Shows how to implement the new tab control.
THREADS   - Basic thread sample.
THUNKS95* - Shows how to use the thunk compiler. Thunking
            scripts are provided.
TRACKBAR  - Shows how to use horizontal and vertical track bar controls.
VLISTVW   - Shows how to use listview controls with the LVS_OWNERDATA
            style.
WRITEPAD  - Multipad-like sample that uses the RTF edit control.

* - These samples use several functions specific to Windows 95 and
    therefore only run on Windows 95.

Building the Samples
--------------------

All of the samples can be built as 32-bits applications.

Each sample contains a MAKEFILE file for building the
application. Simply define the environment variables needed
for NMAKE by running the SETENV.BAT file found in the \MSTOOLS
directory. Then, run the NMAKE command to build the sample.

Architectural Approach
----------------------

The basic programming architecture is implemented using a message
based dispatching mechanism similar to that used by the Microsoft
Foundation Classes. Note that the mechanism used here is very
basic and is intended to be simple yet functional. It is NOT
intended to replace the method used by MFC.

Each message that needs to be processed is entered in a table
(all window and dialog procedures have their own table), and an
associated "handler function" is specified. This allows the
processing for each message to be isolated in a single function.
When a given message is bound for a window procedure, the
dispatching architecture will call the appropriate handler
function. All processing for the message is done in the handler
function.

The framework is designed to be modular, so that you can take
"pieces" of one sample and "plug" them into another.

File Specific Information
-------------------------

All samples from Generic (or Gen32) down contain the following
base files. Note that GENERIC.RC, GENERIC.ICO and GENERIC.DEF
(.DEF file does not exist for 32-bit only samples) will be
<samplename>.* rather than GENERIC.*

ABOUT    C    -- Complete code to bring up About box.
ABOUT    DLG  -- About box dialog template.
DISPATCH C    -- Dispatching routines for the messages.
GENERIC  DEF  -- Module definition file for 16-bit builds (some
                 samples are 32-bit only and will not contain a
                 DEF file).
GENERIC  ICO  -- Icon file.
GENERIC  RC   -- Resource file. Includes menus, icons, dialog
                 templates.
GLOBALS  H    -- Header file containing global variables and
                 function prototypes.
INIT     C    -- Initialization code for main application.
MAKEFILE      -- Makefile for building the sample.
MISC     C    -- Contains generic support functions.
README   TXT  -- Describes the sample.
WIN16EXT H    -- Header file for 16-bit builds (some samples are
                 32-bit only and will not contain this file).
WINMAIN  C    -- Standard WinMain function.
