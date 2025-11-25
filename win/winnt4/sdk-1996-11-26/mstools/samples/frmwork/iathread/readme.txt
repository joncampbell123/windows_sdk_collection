Adjusting Relative Thread Priorities


SUMMARY
=======

The IATHREAD sample shows how to create a multithreaded MDI application, and 
allows the user to adjust relative thread priorities to experiment with the
feel of a multithreaded application.

This sample is based on the THREADS sample. It is a 32-bit only sample.

MORE INFORMATION
================

This sample has one main thread that handles all of the user interface. All 
child window procedures run in the main user interface thread. When a child 
window is created, a new thread is created that does work on the "document" 
that the child window represents, and draws in the child's client area. Each 
child window uses its window extra bytes to store a pointer to a data 
structure that is shared between the child window procedure and the child's 
worker thread. One important part of this data structure is a critical 
section that is used to synchronize access to the rest of the data structure 
between the user interface thread running the child window procedure, and the 
child's worker thread.

There is one general feature that is not obvious. When a user selects the 
Time Critical, Highest, or Above Normal thread priorities, all other threads 
in the application slow down immensely, to the point of almost stop executing. 
To prevent time critical, highest, and above normal priority threads from 
rendering IAThread useless, IAThread has a timeout mechanism that will reset 
their priorities after a while. The function that implements the timeout
check is SetWatchDog in MISC.C. 

When a time critical, highest, or above normal thread starts, it usually 
preempts the user interface thread before the Set Priority/Class dialog box 
goes away. Thus, the dialog stays on the screen although it doesn't do any 
more processing. Applications that want to dynamically adjust priorities and 
make sure that dialogs go away need to prevent threads with higher priorities 
than the thread handling the dialog from running until the dialog has been
ended (i.e. they should wait until calls to DialogBox have exited).  In
general, compute-bound threads should have a priority equal to the user-
interface thread or else the application will be unresponsive to the user.

Module Map

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
Iathread - Window procedure for the main application window.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
MDIChild - Creates the MDI client window and the MDI Children. Also it
           contains the window procedure for the MDI children.
Priority - Contains all functions related to the Priority dialog box.
