Sample: Demonstration of the Win32 Debug API.

Summary:

The Debug Event Browser (DEB) Win32 SDK sample demonstrates
the new Win32 Debug API.  This sample acts as a debugger for
both newly created debuggee processes or attaches to current
active processes.

DEB is not a debugger in the traditional sense; it is a
browser as its name implies.  DEB displays the debug events,
and their relevant properties, as they occur and invokes
the default handlers supplied either by the debuggee or the
system.  Only the minimal debug event handling is imposed
such that the debug events are displayed and the debuggee is
continued on its normal course of execution.

The sample is source code compatible for the Intel 80x86,
the MIPS R4x00, and the DEC Alpha AXP Windows NT platforms.

More Information:

The following is a complete list of files accompanying the
Debug Event Browser sample:

   DEB.BMP        DEB bitmap used by DEB.RTF
   DEB.DEF        DEB module definition file (not used)
   DEB.DLG        DEB dialog resource script file
   DEB.H          DEB ID values and user message defines
   DEB.HPJ        DEB help project file
   DEB.ICO        DEB main icon
   DEB.RC         DEB resource file
   DEB.RTF        DEB help topic file
   DEB1.ICO       DEB animated icon number one
   ...
   DEB8.ICO       DEB animated icon number eight
   DEBDEBUG.C     DEB debug support functions
   DEBDEBUG.H     DEB debug support functions header
   DEBMAIN.C      DEB main module - WinMain and callbacks
   DEBMAIN.H      DEB main module header
   DEBMISC.C      DEB miscellaneous support functions
   DEBMISC.H      DEB miscellaneous support functions header
   LINKLIST.C     Ordered doubly-linked list library
   LINKLIST.DEF   Ordered doubly-linked list module definition file
   LINKLIST.H     Ordered doubly-linked list header
   MAKEFILE       Make file for the entire sample
   README.TXT     This file you are presently reading
   TOOLBAR.BMP    Toolbar bitmap used by DEB.RTF
   TOOLBAR.C      Toolbar functions
   TOOLBAR.DEF    Toolbar module definition file
   TOOLBAR.H      Toolbar header file
