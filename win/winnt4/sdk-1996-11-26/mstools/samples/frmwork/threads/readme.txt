Thread Creation and Destruction


The THREADS sample is a simple thread demonstration. It divides the main 
window into 4 child windows and allows the user to control the creation and 
deletion of 4 threads. The threads are: 

    1. A simple count from 1 to the maximum int. 
    2. A calculation of the Greatest Common Divisor for two numbers. 
    3. A calculation of all the prime numbers between 1 and MAX_INT. 
    4. Drawing random rectangles. 

The key structure in the THREADS sample is ThreadInfo. An array of these 
structures contains the window handles of the respective threads, the height 
and width of the client area of the child windows, and the state flag of the 
thread. The state flag indicates whether the thread is running or not.

The threads themselves are not designed to do anything terribly interesting. 
If you were designing a multithreaded application, you wouldn't want to have 
threads that are as impolite as these. These threads loop while TRUE and 
never worry about setting priorities or synchronizing their execution.

This sample is based on the GEN32 sample. It is a 32-bit only sample.

Module Map
----------

Dispatch - Message dispatching routines.
WinMain  - Calls initialization functions and processes the message loop.
Threads  - Window procedure for the main application window. It also has
           the dispatch loops for messages and commands. This source
           module contains message handlers and command handlers.
Init     - Performs application and instance-specific initialization.
About    - Defines a standard About dialog box.
Misc     - Defines the application-specific commands not related to
           a specific module.
Threads  - Contains the dispatch loop for the child windows, the
           WM_THREADSTATE message handler, the WM_SIZE handler
           for the child windows, and the thread procedures.
