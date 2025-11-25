SDK Samples
-----------

All SDK samples are designed to be built in the directory they are in.  
Just ensure that your Win32 development environment is set up, and you 
can go to any sample directory and do a make.

There is a main make files each sample directory:
MAKEFILE        - for use with Microsoft VC++ 2.0 (NMAKE)

There are 2 ways to build each sample, debug or retail.   To build a
sample as retail with VC++, just type:

nmake retail

From the SAMPLES directory, you may also run the following batch files:

MAKEALL.BAT     - makes all examples with Microsoft VC++

Setup
-----
We have included the source code to a sample setup program for a game
and DirectX.  This is found under the SETUP directory.   Note that to build 
your own custom setup program, all you need to do is edit the copy_list 
at the start of DINSTALL.C to be your list of files, and then search for 
"fox", and then for "bear", and change things appropriately.

Once you are done this, create a game directory that includes your files, 
the setup program you have built, and then xcopy /s the REDIST directory to
the root of your game directory, ie:

XCOPY /S GAMESDKCDROMDRIVE:\REDIST\*.* D:\FUNGAME

If you are building an autorun CD title, you can copy our AUTORUN.INF at 
the root of the Game SDK CD to the root of your game directory 
is called SETUP.EXE, you will not have to make any changes to this file.   
If it is something else, then you can edit AUTORUN.INF appropriately.


Notes for users of Watcom C/C++
-------------------------------
Watcom C/C++ v10.5 is required to compile the GDK samples.
v10.0 is not sufficient. The Microsoft Win32 SDK is also required
and is expected to be in \MSTOOLS on the same drive as your GDK
sample files. If the Win32 SDK is elsewhere, you can set the MSTOOLS
environment variable to point to its root directory (e.g. set
MSTOOLS=\MSTOOLS).

The Watcom makefiles expect the WATCOM environment variable
to be set, as it should have been by the Watcom installation procedure.

Only a subset of the samples have Watcom makefiles. These are:
ddex1 through ddex5, flipcube, donut and stretch.
CD into the appropriate directory and type 'wmake /f makefile.wat'.
There is also a makeallw.bat file in the sdk\samples directory which
you can run to automatically build all these samples under Watcom.

Watcom and Watcom C/C++ are trademarks of Powersoft, Watcom Division.
