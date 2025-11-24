=======================================================================
                   MICROSOFT (R) FOUNDATION CLASSES
=======================================================================

***********************************************************************
***               for Microsoft Windows (R) NT (tm)                 ***
***********************************************************************

-----------------------------------------------------------------------

This subdirectory contains the source code and example code for
the Microsoft Foundation Class (MFC) C++ library.  The MFC library is
a C++ Application Framework designed primarily for use in developing 
applications for Microsoft Windows.  The MFC library is used in
conjunction with the tools provided with the NT beta release.

The MFC library consists of two groups of classes.  One set is
designed for use with Microsoft Windows and includes classes
tailored for GUI application programming, such as classes for
windows, dialogs, controls, and menus.  The other classes
are a set of lower level, non-graphical, general purpose classes
designed to make it easier to write the part of your application 
that is independent of the user interface, and includes classes 
supporting collections, files, memory management, and 
runtime support.  The Windows classes take advantage of the
features provided by these general purpose classes.

The MFC library is designed to be efficient in both space
and time.  There is a minimal execution time overhead, and
the entire library, all 60 classes, is under 39K of executable code.

A complete introduction to the MFC library can be found in the
Overview section of the "Class Reference" book.


Windows Classes
---------------
Microsoft Windows and C++ are "object-oriented" and the MFC
library exploits the Windows operating environment.  The Windows
C++ classes offer the following features:

    o  Robust Application Framework for Windows
    o  Tight coupling to the Windows API
    o  Significant reduction in API "surface area"
    o  Complete classes for nearly all Windows objects
    o  Efficient processing of Windows messages with message maps
    o  Customizable window classes using C++ inheritance
    o  Utility classes to make Windows programming easier

The Windows classes can be used in conjunction with all the standard
Windows APIs and can easily incorporate existing Windows code.  In
addition, the MFC Windows classes provide a seamless migration to
the 32-bit Windows API.


General Purpose Classes
-----------------------
The general purpose classes are useful both with and without Windows.
If you are writing a program that does not use Windows, you can
still take advantage of the MFC library for your application's
user interface independent code.  All of the features are optional
and will incur no runtime overhead if you choose not to use them.  The
general purpose C++ classes include the following features:

    o  Common base class for many classes
    o  Runtime type and metaclass support
    o  Persistent objects
    o  Collection classes based on ANSI C++ template syntax
    o  String class
    o  Time and Date classes
    o  File classes
    o  Exception handling based on ANSI C++ syntax
    o  Diagnostic and debugging support


Support for Windows 3.1 features
--------------------------------
     The Microsoft Foundation classes provide support for the
     enhancements provided in Windows version 3.1. The following
     features are described in technical notes in the
     \MSTOOLS\MFC\DOC directory and demonstrated in sample programs in
     \MSTOOLS\MFC\SAMPLES. These API functions are documented only in
     the Help system. The following list describes the enhancements 
     that can be used to develop applications for both Windows 3.0 
     and Windows 3.1.

     o The development and use of custom controls is supported. In
       addition, owner draw controls and bitmap buttons are provided. 
       See TN014.TXT and the sample application CTRLTEST.

     o Common dialog operations are now supported with easily
       customized classes including CFileDialog (for both File Open
       and File Save As), CFindReplaceDialog (to implement modeless
       find and replace), CColorDialog (for color selection),
       CPrintDialog (for both print setup and print), and
       CFontDialog (for font selection). These new dialogs are
       described in TN013.TXT.

     o Dialog boxes now feature a gray background which is easily 
       customized.

     o OLE servers now register themselves at startup so that
       users do not need to use REGEDIT.EXE.
    
     o Using multiple inheritance with Microsoft Foundation
       classes is demonstrated in the sample application MINSVRMI,
       a small OLE server that uses multiple inheritance.

     o For applications that target Windows 3.1 only, the Microsoft 
       Foundation Class Library supports the most useful new Windows 
       3.1 API functions and messages.


Why use the Microsoft Foundation library instead of C and the SDK?
------------------------------------------------------------------
The MFC library has many advantages over using the traditional
C techniques in the Microsoft Software Development Kit for Windows.
Some of these advantages include:

    o  MFC decreases the API surface area; the SDK is a catalog of
    hundreds of functions, whereas the MFC library takes advantage
    of the inheritance and polymorphism supported by C++ to organize
    the Windows APIs in a logical and manageable manner.

    o  MFC provides a standard Windows application startup; the
    MFC class CWinApp implements the functionality normally provided
    in WinMain.

    o  MFC encapsulates Windows behavior; the class structure
    and data encapsulation supported by C++ safely hides many
    of the details that programmers need not concern themselves
    with.

    o  MFC handles many details of Windows programming;  gone are
    the days of registering Window classes or MakeProcInstance.

    o  MFC provides a object-oriented mechanism for routing
    Windows messages;  the message map supported by each window
    C++ class provides a cleaner and less error prone mechanism for
    handling the mapping of Windows messages to member functions.
    Messages, commands, and notifications can be handled in this
    object-oriented manner.

    o  MFC taps the power of C++.  C++ supports many features that
    make writing programs easier, such as type safety, encapsulation,
    inheritance, and polymorphism, that when used in conjunction with
    MFC provides the best technique for writing Windows programs
    in C++.


Usage
-----
Source code is provided so that you can build the library target
that you require.  The file SRC\README.TXT contains instructions for
building the library.

Before using the MFC library in your own code be sure that the
MFC\INCLUDE subdirectory is in your INCLUDE environment variable.


Subdirectories
--------------
The MFC subdirectory contains several subdirectories.  Nearly every
subdirectory contains a README.TXT file that describes the
directory's contents.  The following subdirectories are present:

    DOC\        - MFC technical notes describing implementation details.
    INCLUDE\    - MFC header files.
    SRC\        - MFC source files and instructions for building a library.
    LIB\        - MFC libraries in binary form.
    SAMPLES\    - MFC sample applications.


Documentation
-------------
The NT release of the MFC library ships with both on-line WinHelp and
QuickHelp documentation only. No MFC printed documentation is available 
with this release, but the printed MFC documentation provided with the 
Microsoft C/C++ 7.0 compiler can be used 'as-is'.

If you have any comments or suggestions regarding the Microsoft
Foundation Classes, please let Microsoft know through the beta program
liaison.


NOTE
----

The Microsoft Foundation Classes (MFC) library source code is
shipped "as is" and is designed for your own use and modification.
Microsoft Product Support can only offer limited support regarding
the MFC source code because of this benefit. 

Microsoft grants you a non-exclusive royalty-free right to use and 
modify the source code contained in any Microsoft Foundation Classes 
source code file for purposes of creating a software product.  
However, you may not include this code in source form (or 
any modified version of it) within any software product.   

Notwithstanding the Microsoft End User License Agreement, THIS SOURCE 
CODE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND.  YOU AGREE THAT 
NEITHER MICROSOFT NOR ANYONE ELSE INVOLVED IN CREATING, PRODUCING OR 
DELIVERING THE SOURCE CODE SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, 
CONSEQUENTIAL, OR INCIDENTAL DAMAGES RELATING TO THE SOURCE CODE.


Windows NT Release Notes:
-------------------------

-- This release is a strict superset of the Microsoft C/C++ 7.0 release
   of MFC.  Any applications written for the MFC C++ Windows API will
   need only be recompiled using the 32 bit tools in this release in order
   to run in full 32 bit mode.  For those familiar with MFC, you will
   note that the sample applications included in this release are 
   largely unchanged from the 16 bit release.  This release of MFC can be
   used interchangeably with the Microsoft C/C++ 7.0 release.

   SAMPLES ARE NT VERSION BY DEFAULT: The makefiles for the sample 
   applications have been changed to build the NT VERSION BY DEFAULT.  
   If you wish to build a Windows version using these sources, you must
   define WIN16MFC=1 when building.

-- In order to build the MFC sample applications you should have your
   environment set as follows:
   	set LIB=c:\mstools\lib;c:\mstools\mfc\lib
   	set INCLUDE=c:\mstools\h;c:\mstools\mfc\include
   These may also be set on the NMAKE command lines if you wish.  If your
   installation is on another drive or in another directory, then you
   should change c:\mstools accordingly.

-- Your PATH must include the Windows SDK Tools and Microsoft C/C++.

-- Online documentation is available via WinHelp (MFC10WH.HLP) and is
   installed by the SDK setup in MSTOOLS\BIN.

-- Do not use the /Zp switch for MFC applications.  If you do some system
   calls may not work (such as COMMDLG).

-- The LINK32 linker emits a warning when it does not extract any modules
   from a .LIB.  The MFC sample applications will emit this warning which can
   be safely ignored.

-- The Find Next feature on the MULTIPAD sample does not scroll the window
   vertically. 
