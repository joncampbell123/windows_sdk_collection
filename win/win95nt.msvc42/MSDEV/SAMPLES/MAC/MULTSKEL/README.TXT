About MultSkel
==============
MultiSkel -- A Multi-Window Demo that uses the TransSkel code

For detailed information about the TRANSKEL, read the transkel.txt file.


Building from the IDE
=====================
1. From the <File> menu open the multskel.mak project.  

2. From the <Project><Settings> dialog select Debug.  Change the remote
   executable path name to the name of your Macintosh folder where you
   want MSVC++ to copy multskel.exe.

3. Make sure the MSVC++ File Utility app is running on your Macintosh.

4. Make sure the MSVC++ Debug Monitor is in your Macintosh control panel.

5. Build multskel.exe - <Project><Build Multskel.exe>

6. When the build is finished MSVC++ should copy the file to the Macintosh
   folder you specified in step 2.  Use the <Debug> commands to step into
   the application.

Building from the command line
==============================
1.  Set your enviroment variables to point to VC++ 2.0.

2a. To build the debug version, the command is:
      nmake /f multskel.mak CFG="Macintosh Debug" 

    To copy multskel.exe to a Macintosh, the command is:
      mfile copy /t APPL MacDebug\multskel.exe :<Mac Hard Drive Name>:multskel.exe

2b. To build the retail version, the command is:
      nmake /f multskel.mak CFG="Macintosh Release" 

3.  To copy multskel.exe to a Macintosh, the command is:
      mfile copy /t APPL MacRel\multskel.exe :<Mac Hard Drive Name>:swapskel.exe

