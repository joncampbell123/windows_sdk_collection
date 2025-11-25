About Sacode
============
Sacode - Stand-alone code sample.  Once built and placed in your Macintosh 
Extensions folder this INIT application will produce five system beeps upon 
system startup.


Building from the IDE
=====================
1. From the <File> menu open the sacode.mak project.  

2. From the <Project><Settings> dialog select Debug.  Change the remote
   executable path name to the name of your Macintosh folder where you
   want MSVC++ to copy sacode.exe.  (Most likely your Extensions folder.)

3. Make sure the MSVC++ File Utility app is running on your Macintosh.

4. Make sure the MSVC++ Debug Monitor is in your Macintosh control panel.

5. Build sacode.exe - <Project><Build Sacode.exe>

6. When the build is finished MSVC++ should copy the file to the Macintosh
   folder you specified in step 2.  Restarting your system with the sacode 
   app in your Extensions folder will cause sacode to execute, resulting
   in five system beeps.


Building from the command line
==============================
1.  Set your enviroment variables to point to VC++ 2.0.

2a. To build the debug version, the command is:
      nmake /f sacode.mak CFG="Macintosh Debug" 

    To copy sacode.exe to a Macintosh, the command is:
      mfile copy /t INIT MacDebug\sacode.exe :<Mac Hard Drive Name>:sacode.exe

2b. To build the retail version, the command is:
      nmake /f sacode.mak CFG="Macintosh Release" 

    To copy sacode.exe to a Macintosh, the command is:
      mfile copy /t INIT MacRel\sacode.exe :<Mac Hard Drive Name>:sacode.exe

3.  Move sacode.exe to your Extensions folder.

4.  Restart your system.  sacode will be executed at startup resulting in
    five system beeps. 

