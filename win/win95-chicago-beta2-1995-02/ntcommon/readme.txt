10/21/94


Release Notes for Common Controls on NT (Beta release)
==========================================================

This version of the Common Controls for NT was intended to
help people writing applications for Windows 95 develop on
Windows NT v3.5.  These controls are not finalized, and
any UI testing/confirmation should be done on Windows 95 instead
of NT.  The reason for this is NT does not currently support
the new Windows 95 look and feel (eg: 3D window borders,
proportional scroll bar thumbs, etc).

The controls are Unicode and support both Unicode and ANSI
applications.

This Beta DLL is NOT to be redistributed.  These controls will
not be considered final until Windows 95 has shipped, at which 
time they will be made available for Windows NT.

The controls are new functionality to NT and will require changes 
to User32 to run them.  The version of the controls in this release 
contain stubs to replace the missing User functions so that they 
will run on Windows NT 3.5 (build 807).  This was done for testing 
purposes only.  The stubs are not a 100% replacement for the real 
functions.

Please remember that this information is confidential.

System Requirements:
====================

Windows NT v3.5 running on either a X86, MIPS, Alpha.


Development Environment:
========================

Windows 95 SDK
--------------

This version of comctl32.dll is compatible with the commctrl.h and
comctl32.lib from the Windows 95 SDK.  You do not need the commctrl.h
and .lib file is you are writing an ANSI application.


or:


Win32 SDK for Windows NT
------------------------

If you are writing a Unicode application, then use the comctl32.h
from the \inc directory, and the corresponding .lib file.



How to Install comctl32.dll:
============================

1)  Change to your \winnt35\system32 directory.
2)  Rename comctl32.dll comctl32.sav
3)  copy \comctl32\*\retail\comctl32.dll
    Where the "*" is either x86, mips, alpha.
4)  Reboot


Known differences and problems:
===============================

There are several known differences between the common controls
on Windows 95 and the controls for NT.

1)  3D window border styles are not supported by NT's User.
    (eg:  WS_EX_CLIENTEDGE).

2)  Proportional scroll bar thumbs are not supported by NT's User,
    so in a listview the thumb will not scroll to the end
    even though the contents will be at the end.  If the
    thumb was drawn proportionally, it would go to the end.

3)  The Customize toolbar dialog box in NT's version of
    File Manager does not work.  This only affects File Manager.

4)  The UnPause toolbar button in NT's version of Print Manager
    does not work.  This only affects Print Manager.

5)  The default printer combo box in Printer Manager does not appear.
    This only affects Print Manager.

NOTE: If you simply include comctl32.dll in your application directory 
      instead of copying it into your system32 directory (_ONLY_ when 
      running on Windows NT), you can avoid problems 3-5 above.

How to Report Bugs:
===================

  Please report bugs in the Windows 95 Beta Forum.  Be sure to include
  repro steps and stack traces if applicable.
