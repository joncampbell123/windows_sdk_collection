README.TXT - Microsoft Windows for Pen Computing Files

This file contains the following information regarding building
Microsoft Windows for Pen Computing applications:

I.   Introduction
II.  Hardware Requirements and Basic Limitations
III. Installation Notes and Procedures
IV.  Shipping PENWIN.DLL with your application
V.   Release Notes


I.  Introduction
================

The Windows SDK contains sufficient components from Windows for
PenComputing to allow the application designer to build and test
pen applications. The Microsoft Mouse can be used to get a rough
idea of how recognition and a pen will work.

However, it is STRONGLY recommended that Windows for Pen
Computing hardware in the form of a computer or peripheral device
be used during the design and development process.  This is
critical because a pen gives the application designer an accurate
feeling for how an application will work in real situations AND
because the Microsoft Alphanumeric Recognition System shipped
with the SDK has been optimized for a pen and will work less well
with a mouse.


II.  Hardware Requirements and Basic Limitations
================================================

   1. A driver for the Microsoft Mouse has been provided so that
      simple testing of pen functionalities can be done. This pen
      driver is called MSMOUSE.DRV.

   2. At this time, only VGA displays can be used with the pen
      extensions. The VGAP.DRV display driver is a modified
      version of the VGA.DRV that supports inking.  It is
      required if the pen functionalities are to be tested.

   3. Handwriting recognition with the Microsoft Mouse will
      achieve poor results in comparison to digitizer hardware
      designed specifically for Pen Computing.  The recognizer
      has been designed to work with pen computers and
      peripherals with true digitizer input and its associated
      high data rates and high data resolution.

	4. The spell checking technology included in Windows for Pens may
      be used exclusively for the purpose of improving handwriting 
      recognition.  It is not to be used by applications as a spell
      checker or spelling corrector.
      


III. Installation Notes and Procedures  (***** READ THIS *****)
===============================================================

Installing the Pen Components - Minimum
---------------------------------------
This procedure will result in a system that will let you build
applications that contain hedit and bedit controls - and call any
of the Windows for Pen Computing APIs.  You will not be able to
perform handwriting recognition or see ink on the screen.

SYSTEM.INI Changes

The following items must be added or changed in your SYSTEM.INI
file so that the pen extensions will work. NOTE: BACK UP YOUR OLD
SYSTEM.INI file before proceeding.

1.  In the "[boot]" section:

    a.  Add "penwindows" to the list of drivers after the
       "drivers=" key. For example:

             drivers=mmsystem.dll penwindows

2. In the "[Drivers]" section:

    a. Add a new item "penwindows" and set it equal to the path
       to PENWIN.DLL.  For example:

             penwindows=C:\WINDEV\REDISTRB\PENWIN.DLL

When Windows is restarted PENWIN.DLL will be loaded as an
installed driver and you will be able to run applications
containing bedit and hedit controls and call the Windows for Pen
Computing APIs.

Installing the Pen Components - Complete
----------------------------------------
This procedure will result in a system that will run pen
applications and allow you to experiment with handwriting
recognition and inking functionalities in your applications. Once
again, interaction with the mouse will prove inferior in every
respect to interaction with a true pen device - but this system
of using the special mouse driver will allow you to experiment
and perform rudimentary testing of your pen functionalities.

SYSTEM.INI Changes

The following items must be added or changed in your SYSTEM.INI
file so that the pen extensions will work. NOTE: BACK UP YOUR OLD
SYSTEM.INI file before proceeding.

1.  In the "[boot]" section:

    a. Change the "display.drv=" line so that the display driver
       is the pen capable VGAP.DRV shipped with the Windows SDK.
       For example:

			display.drv=C:\WINDEV\PEN\VGAP.DRV

       Note:  Only the VGA display device is supported by the pen
       components in the Windows SDK.

    b. Add "pen penwindows" to the list of drivers after the
       "drivers=" key. For example:

             drivers=mmsystem.dll pen penwindows

    c. Change the "mouse.drv=" line so that it points to
       YESMOUSE.DRV. For example:

             mouse.drv=C:\WINDEV\PEN\YESMOUSE.DRV

2. In the "[Drivers]" section:

    a. Add a new item "pen" and set it equal to the path to
       MSMOUSE.DRV. For example:

              pen=C:\WINDEV\PEN\MSMOUSE.DRV

    b. Add a new item "penwindows" and set it equal to the path
       to PENWIN.DLL.  For example:

              penwindows=C:\WINDEV\PEN\PENWIN.DLL
 
3.  PENWIN.INI Changes

The PENWIN.INI file contains a number of initialization settings
for Windows for Pen Computing.

There are also two explicit paths that must correctly identify
the locations of MARS.DLL and MARS.MOB.  The Windows SDK
installation procedure did not update these paths for users who
chose to install to a directory other than the default.  As this
is the case, you need to update them by hand.

Open PENWIN.INI with any generic text editor (like Windows
Notepad) and change the path to MARS.DLL so that it correctly
identifies points to the current MARS.DLL location.  Do the same
for MARS.MOB.

Once the paths are correct, the file should be copied to the
Windows 3.1 root - that is, the directory containing the Windows
3.1 WIN.COM.


IV.  Shipping PENWIN.DLL with your application
==============================================

PENWIN.DLL is a fully redistributable component of Windows for
Pen Computing.  Because applications will seek to leverage the
Pen API - hedit and bedit controls in particular - PENWIN.DLL can
be shipped with your application.  There are some considerations
to keep in mind in shipping PENWIN.DLL with your application:

1.  PENWIN.DLL functions ONLY under Windows 3.1.  It WILL NOT
    WORK with Windows 3.0 because it functions only as an
    installable device driver - a feature not present in Windows
    3.0.

2.  As with other redistributable components such as COMMDLG.DLL
    and the OLE libraries, it is the responsibility of the
    application vendor to determine whether PENWIN.DLL has
    already been installed (there is a GetSystemMetrics() call
    for this) and to ensure that the version of PENWIN.DLL with
    the latest version stamping is the one that is running. These
    issues are the same for all redistributable components, and
    further information is contained elsewhere in this SDK.

3.  Unlike some of the other redistributable components, if your
    application installs PENWIN.DLL for the first time, or
    replaces the current version with a later one, Windows will
    have to be restarted.  As an installable driver PENWIN.DLL
    can be loaded only at Windows boot time.  Restarting Windows
    can be accomplished via an ExitWindows() call or by simply
    prompting the user to do so.

    Note:  To install PENWIN.DLL on a Windows 3.1 system follow
           the "Minimum" procedure listed above.

4. PENWIN.DLL may be in either the \WINDOWS or the
   \WINDOWS\SYSTEM directory.  The default will be \WINDOWS but
   since Windows for Pen Computing is an OEM product. Microsoft
   cannot completely control where PENWIN.DLL is located on a
   particular machine.

V.  Release Notes
=================

Release Notes for the Microsoft(R) Windows for Pen Computing
Programmer's Reference, version 1.00 (C) Copyright 1992 Microsoft
Corporation.

This section contains release notes for version 1.00 of the
Microsoft(R) Windows for Pen Computing Programmer's Reference.
The information in this section is more current than the
information in the manual.  Where this file conflicts with
printed documentation, you should assume that this file is
correct.

Microsoft revises its documentation at the time of reprinting;
the manuals and online help files may already include some of
this information.

Hedits - Delayed recogntion mode
-----------------------
Setting focus in hedit causes any text in the control to appear
even if the control is in "ink" mode.  If this is undesireable
the control should never be allowed to get the focus.

Sending an hedit the WM_HEDITCTL message with the HE_SETINKMODE
parameter will clear the hedit's text buffer.  The same message to a 
bedit will preserve the control's text contents.

PostVirtualMouseEvent
---------------------
Values greater than the maximum resolution in X direction
(usually 640) and max resoultion Y direction (usually 480) will
overflow.

ALC_USEBITMAP
-------------
The Microsoft recognizer does not implement ALC_USEBITMAP in the
alcPriority field for version 1.0. Note that alcPriority is
implemented for the alc field.

ProcessWriting
--------------
In the description of ProcessWriting, it says "The window
specified by the hwnd parameter receives a WM_PARENTNOTIFY
message when ProcessWriting destroys its inking window."

The window never gets the WM_PARENTNOTIFY message since it is not
guaranteed that an inking window is created.

Microsoft User Dictionary DLL
-----------------------------
The documentation incorrectly states that up to 16 dictionaries
can be loaded.

MSSPELL.DLL actually allows only six wordlists to be loaded.
Consequently, version 1.00 of the Microsoft User Dictionary DLL
allows only six wordlists to be loaded at a time.

WM_HEDITCTL HE_CHAROFFSET
-------------------------
Under the documentation for WM_HEDITCTL messages, under
HE_CHAROFFSET, it says "See the related HE_CHAROFFSET".  It
should say "See the related HE_CHARPOSITION".

WM_SKB Message
--------------
When the state of the SKB changes, a WM_SKB message is posted.
The documentation says that the LOWORD of the lParam contains
information on what changed and that the HIWORD contains the
window handle of the SKB.

Actually, the LOWORD contains the hWnd and the HIWORD contains
the information on what changed.

This should be corrected in two places: in the ShowKeyboard
function and in the WM_SKB message documentation.

Also, one more value should be mentioned: the HIWORD of lParam
contains SKN_TERMINATED (value 0xffff) if the keyboard has been
closed.

SKN_TERMINATED
--------------
WM_SKB sends SKN_TERMINATED in HIWORD(lParam) when terminating
SKN_TERMINATED (0xffff) is sent in the HIWORD(lParam) when the
WM_SKB is sent to notify top-level windows that the On-Screen
Keyboard is being terminated.  This needs to be added to the
documentation for the WM_SKB message and for the ShowKeyboard
function.

REC_DEBUG
---------
In _Guide_to_Pen_Programming_ Chapter 11 Pen Messages and
Constants, Under REC_ Values, Under Debugging Values, it says:

"REC_DEBUG   All debugging return values are less than this."

It should say:

"REC_DEBUG   All debugging return values are less than or equal
to this."

clErrorLevel
------------
The documentation for the RC field clErrorLevel says that this
value can range from 0 to 100.  In the PENWIN.H file, the minimum
CL value is defined as

    #define CL_MINIMUM 1

The correct minimum for clErrorLevel is 1

Dictionary Searches
-------------------
The documentation is somewhat confusing on the point of
dictionary enumeration procedures in chapter 7, page 103.  To
expound:

If there are ten dictionaries in the dictionary path, and the
ninth finds a match for a particular enumeration in a symbol
graph, the remaining symbol graph elements will STILL be
enumerated - checking for a match in a higher-order dictionary.
In other words, the other eight dictionaries before the ninth
dictionary in the list will get a shot at finding a "better"
match.  Enumeration of dictionaries therefore can be said to stop
only when the symbol graph is exausted, or the first dictionary
in the list responds affirmatively to a query.


Documentation Clarification - rc.RectBound
------------------------------------------
Here is additional detail on the rectBound element of the RC
structure.

rc.rectBound will be ignored if PCM_RECTBOUND is not set.  The
documentation suggests on page 237 that rc.lPcm = PCM_RECTBOUND
only determines how the recognition context will end.

Documentation incorrect:  DRV_SetSamplingDist
---------------------------------------------
pg. 226 chapter 10 - PENINFO structure:  

In the notes after nSamplingRate and nSamplingDist, the driver
messages that manipulate these fields are misnamed.  In the
printed documentation, they are named DRV_SetSamplingDist and
DRV_SetSamplingRate; they are actually DRV_SetPenSamplingDist and
DRV_SetPenSamplingRate.

RecognizeData and Ink:  Clarification
--------------------------------------
Calls to RecognizeData may return an rcresult that references
pendata different than that used as a Parameter to the call.  For
example, Strokes may be removed and the rgbInk and nInkWidth
fields of the PENDATAHEADER may not match the values in the
original pendata, as no inking has taken place during this
recognition context.

List of characters effected by ALC_PUNC
---------------------------------------
On page 253 of the documentation, the list of chars in ALC_PUNC
has two semicolons; one of these should be a colon.

DLLs that use hedit and bedit controls take note!
-------------------------------------------------
Any DLL which creates an hedit or bedit control must have a non-0
heap size.  This is because those controls allocate buffers out
of this heap.

Dictionary and Recognizer ISVs:  When Windows ends
--------------------------------------------------
When a Windows session is about to end, PENWIN.DLL takes the
following actions:

1) Calls all the dictionaries in the global recognition context
   with a DIRQ_CLEANUP message and then frees the corresponding
   DLLs.

2) Calls the CloseRecognizer function for the current recognizer
   and frees the corresponding DLL.

Dictionaries and recognizers can take the appropriate cleanup
action at this time. The limitations on things that can be done
are the same as those when an application receives a
WM_ENDSESSION message.

Expense Sample May Require .INI settings
----------------------------------------
When running the Expense sample app, to get the custom word lists
loaded, one of the following two conditions must be true:

1) NAMES.DIC and DEPTNAME.DIC must be in the same directory as
   USERDICT.DLL.

or

2) The following lines must be added to PENWIN.INI to point the
   word lists

    [Expense]
    namedict=c:\windev\samples\expense\names.dic
    deptnamedict=c:\windev\samples\expense\deptname.dic

SetAlcBitGesture, ResetAlcBitGesture, IsAlcBitGesture Removed
-------------------------------------------------------------
The Windows for Pen Computing Programmer's Reference refers to
the above macros.  They have been removed because the ability to
set alc bits for gestures was not implemented.

REC_ error values from Recognize()
----------------------------------
In the documentation for Recognize API and REC_ values in chapter
11, there is a Debugging values section.  There is a sentence
that reads: "All of the values listed in the following table are
in debug version only."  That sentence should be replaced with
the following: "All of the values below are providing for
debugging information.  A well-behaved application should not
specify an RC which causes any of these values to be returned."

DIRQ_SUGGEST not implemented
----------------------------
In version 1.0 of Windows for Pens, the dictionary shipped with the system
does not support DIRQ_SUGGEST.
