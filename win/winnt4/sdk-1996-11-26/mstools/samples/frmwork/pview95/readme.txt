Process Viewer for Windows 95


SUMMARY
=======

The PView95 sample shows how to use the 32-bit ToolHelp API to enumerate 
processes, threads, and modules in Windows 95. It displays information about 
each process and all threads within a process in ListView controls. 
Finally, it shows how to terminate a process.

This sample is based on the GEN32 sample. It is a 32-bit only sample. 
It does not run on Windows NT.

MORE INFORMATION
================

PView95 has several non-obvious interface elements. First, the top ListView 
control lists all active processes, while the bottom pane lists all threads 
from a selected process. You must select a process from the list of processes 
before any data will be listed in the thread list. This is because PView95 
only displays threads that belong to a single process.

The column headers in both ListView controls can be used for sorting the 
displayed data. Simply click on a column header and the data will be sorted.

The tool bar uses ToolTips--leave the mouse cursor over a tool bar button 
for a short time and a brief description of the button appears.

Be careful about terminating processes. PView95 allows you to kill any 
Win32-based application. PView95 asks for confirmation before killing any 
process, so you have a second chance before the process will be killed.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
PView95  - Window procedure for the main application window.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
ProcThrd - Implements process, thread, module enumeration and
           manipulation functions.
ListView - Implements the process and thread ListView controls
ToolBar  - Implements the tool bar and ToolTips
