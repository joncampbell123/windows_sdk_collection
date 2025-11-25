About SwapSkel
==============
SwapSkel -- Swapped version of the MultiSkel example application. (For
details about MultiSkel, see the MultiSkel readme.txt file.) SwapSkel
makes several minor changes to MultiSkel:  It is compiled as a swappable
application, and linked with the swapper and swappable runtime libraries. 
It contains a GrowZone procedure which calls the swapper's growzone 
procedure.  It sets the swapper's "code fence."

This application has been run in a partition as small as 27K, although
MultiSkel's help window can't handle the low memory conditions below about
30K (the window contains garbage.)  This size is subject to some fluctuation,
due to variance in the compiler and libraries as development work continues.

Note that in the SWAPSKEL.C file, the line

	SetMaxSwapSize(0);      /* Force App. to thrash. */

sets the code fence to zero. This permits only one segment in memory at a
time. You will want to use another value for SetMaxSwapSize() for more
efficient swapping. For more information on SetMaxSwapSize(), see Chapter
7, "Memory Management," of the Visual C++ for Macintosh Programmer's Guide.

For detailed information about the TRANSKEL, read the transkel.txt file.


Building from the IDE
=====================
1. From the <File> menu open the swapskel.mak project.  

2. From the <Project><Settings> dialog select Debug.  Change the remote
   executable path name to the name of your Macintosh folder where you
   want MSVC++ to copy swapskel.exe.

3. Make sure the MSVC++ File Utility app is running on your Macintosh.

4. Make sure the MSVC++ Debug Monitor is in your Macintosh control panel.

5. Build swapskel.exe - <Project><Build Swapskel.exe>

6. When the build is finished MSVC++ should copy the file to the Macintosh
   folder you specified in step 2.  Use the <Debug> commands to step into
   the application.

Building from the command line
==============================
1.  Set your enviroment variables to point to VC++ 2.0.

2a. To build the debug version, the command is:
      nmake /f swapskel.mak CFG="Macintosh Debug" 

    To copy swapskel.exe to a Macintosh, the command is:
      mfile copy /t APPL MacDebug\swapskel.exe :<Mac Hard Drive Name>:swapskel.exe

2b. To build the retail version, the command is:
      nmake /f swapskel.mak CFG="Macintosh Release" 

    To copy swapskel.exe to a Macintosh, the command is:
      mfile copy /t APPL MacRel\swapskel.exe :<Mac Hard Drive Name>:swapskel.exe

