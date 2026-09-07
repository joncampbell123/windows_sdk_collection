FILEVIEW - A File Viewer Sample
-------------------------------------------

The FILEVIEW sample demonstrates the basic steps that need
to be taken in order to implement a simple text-based file viewer
under Windows 95. This particular sample is a viewer for text files, those
with the .TXT extension as well as any compound file with the CLSID
of CLSID_SampleTextFile defines as 0x00021116-0000-C000-000000000046.
The files FLATTEXT.TXT and COMPTEXT.TXT are two such files.
If you build the sample viewer and register it via the FVTEXT.REG file,
you will be able to use the sample viewer.  Note that the system has
a TXT file viewer installed by default, so to disable this in order
to test this sample, rename the vsasc8.dll file in the system\viewers
subdirectory or edit FVTEXT.REG and change the TXT extension to another
extension not supported by QuickView, like XXX, and then use text files
with the XXX extension.  

You can build the sample, once the Win32 SDK is properly installed, by
typing "nmake." Be aware that the sample built produces a DLL. Once this
DLL is registered, clicking with the right mouse button on one of the TXT
(XXX) files will display a context menu.  Within this context menu, you
will see the "QuickView" option. If you click on this menu item, the sample
file viewer will be invoked.

This sample was designed to require only small amounts of modification
to build a custom viewer and the areas where you would need to make
changes are clearly marked in the source code. The FileViewer component 
object itself used CLSID_FileViewerText:0x00021117-0000-C000-000000000046.  
Do not use this CLSID yourself! You will need to create your own unique CLSID
using the GUIDGEN.EXE utility that comes with the SDK. 

This sample builds FVTEXT.DLL and places it in the 
C:\WINDOWS\SYSTEM\VIEWERS directory.You need to create a different name
for your own Viewer and use your particular system path.  
Rename the following files to your custom name and make the 
indicated filename changes within those	files. 

FVTEXT.CPP - Main source file.  Change occurrences of "FVTEXT"
to your own name (occurs only in header comment and in one debug output macro.
FVTEXT.CPP has the CFileViewer object implementation which is
where most of the action is.  There is nothing in this file but
most of the CFileViewer member functions.  Most of the
functions will require modification depending on the type of
viewer you implement.  For the most part, most of the functions and
fields in CFileViewer need no modification.  Those specific places
needing it for sure are marked with MODIFY in these files.

FVTEXT.DEF -  Module definitions file.  Change the LIBRARY
and DESCRIPTION lines to match your implementation. NOTE:  YOU MUST
 *EXPORT* DllGetClassObject and DllCanUnloadNow!

FVTEXT.H - Main object header file.  Change occurrences of
"FVTEXT" to your own name (occurs in header comments and 
#ifndef/#define/#endif directives as well as two references to FVTEXT.CPP).
It is MOST IMPORTANT that you not use the CLSIDs defined here.  
These are strictly for use with this sample.  Use the SDK tool GUIDGEN.EXE 
to generate your own unique CLSIDs.

FVTEXT.ICO - Icon to use for FileViewer window.  Use your own icon.

FVTEXT.RC - Resources.  Change the "FVTEXT" in the header to your own
name and change the references to "fvtext.ico" and "fvtext.rcv" to your own names.

FVTEXT.RCV - Version information.  Change "FVTEXT" in the header 
comments to your own name.

FVTEXT.REG - Registry information for the FileViewer.  Change
the InprocServer32 entry (the last line) to your own DLL name.
Change the extension from CPP to the extension you are supporting.

MAKEFILE - This is an external makefile for use with NMAKE.  It is not a Visual C++
project file.  In general you will have to change the following macros
definitions for a custom viewer:

    Macro           Location    Use
    ---------------------------------------------------------------------
    DEFFILE         Line 24     Indicates your .DEF file
    TARGET          Line 42     Indicates the name of the FileViewer.
                                MAKEFILE assumes you have a .CPP, .H,
                                .RC, .RCV, and .ICO file with this name
                                and will build a .DLL with this name.

The dependency list at the bottom of the file should not need any
modifications unless you rename other files or add new files.  If you
add new files, be sure to add then to the OBJS macro at line 58.

CSTATHLP.CPP, CSTATHLP.H - Definition and implementation of 
a class called CStatusHelper that simplifies WM_MENUSELECT
processing. To utilize this object, create one and
call its MessageMap function (see CSTATHLP.CPP for documentation).
You can then call its MenuSelect function to handle all WM_MENUSELECT
messages. You can also ask CStatusHelper to display a specific message in the
message map by calling MessageDisplay. You pass an ID of the message in
the message map and the CStatusHelper will go locate the string and
display it.

CSTRTABL.CPP, CSTRTABL.H - Definition and implementation of a 
CStringTable class that simplifies stringtable management.

DBGOUT.H - This file contains a number of macros that help keep source
code clean when including debug output and conditional code.

IFILEVW.CPP - This file implements the IFileViewer interface required 
of a FileViewer using a class CImpIFileViewer (defined in FVTEXT.H). 
You should not have to make any changes to this file.  The IUnknown
members are delegated to CFileViewer's IUnknown implementation.

IPERFILE.CPP - This file implements the IPersistFile interface for a FileViewer using
the class CImpIPersistFile. 

FILEVIEW.CPP, FILEVIEW.H - FILEVIEW.H is the master header 
file for the entire sample that most of the .CPP files include.  It pulls in the other header
files necessary for the specific viewer.  In this sample it brings
in FVTEXT.H (FileViewer object specifics) and RESOURCE.H (resource
identifiers, etc.).  Since your FileViewer should have a different
name than FVTEXT you'll want to change the reference to FVTEXT.H.

FVPROC.CPP - This file contains the window procedures for the 
frame window, the viewport window, and the about box.
ViewportWndProc is what generates the display of the file.  This
will require major modifications depending on
what you need to do.
