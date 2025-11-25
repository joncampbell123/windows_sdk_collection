About Skel
==========
Skel -- A Simple Application that uses the TransSkel code.

For detailed information about the TRANSKEL, read the transkel.txt file.


Building from the IDE
=====================
1. From the <File> menu open the skel.mak project.  

2. From the <Project><Settings> dialog select Debug.  Change the remote
   executable path name to the name of your Macintosh folder where you
   want MSVC++ to copy skel.exe.

3. Make sure the MSVC++ File Utility app is running on your Macintosh.

4. Make sure the MSVC++ Debug Monitor is in your Macintosh control panel.

5. Build skel.exe - <Project><Build Skel.exe>

6. When the build is finished MSVC++ should copy the file to the Macintosh
   folder you specified in step 2.  Use the <Debug> commands to step into
   the application.

Building from the command line
==============================
1.  Set your enviroment variables to point to VC++ 2.0.

2a. To build the debug version, the command is:
      nmake /f skel.mak CFG="Macintosh Debug" 

    To copy skel.exe to a Macintosh, the command is:
      mfile copy /t APPL MacDebug\skel.exe :<Mac Hard Drive Name>:skel.exe

2b. To build the retail version, the command is:
      nmake /f skel.mak CFG="Macintosh Release" 

    To copy skel.exe to a Macintosh, the command is:
      mfile copy /t APPL MacRel\skel.exe :<Mac Hard Drive Name>:skel.exe
