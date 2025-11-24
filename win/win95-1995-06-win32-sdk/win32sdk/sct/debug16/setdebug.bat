echo off
@rem
@rem    This command file will make debugging active.  Use after copdebug
@rem
@rem
@rem    Must be in nt\system32 directory
@rem
@rem    cd \winnt\system32
@rem    ..\sct\debug16\setdebug
@rem
@rem

@rem    Try and put into system32 directory if we can

if not exist progman.exe cd %path%

@rem    Lets do some error checking

if not exist progman.exe goto error0
if exist wow32d.dll goto continue1
if exist wow32x.dll goto error1
goto error2

:continue1

echo.
echo.
echo.   Pviewer will start after you hit a key, make sure NTVDM is not running...
echo.                                           
echo.                                           
pause

pviewer.exe

@rem    rename the nodebug versions to save them

if not exist wowexecx.exe  ren wowexec.* wowexecx.*
if not exist wowexecx.exe  goto error3
ren ntvdm.*  ntvdmx.*
ren wow32.*  wow32x.*
if exist krnl386.exe ren krnl386.* krnl386x.*
if exist krnl286.exe ren krnl286.* krnl286x.*
ren dosx.*    dosxx.*

@rem    rename the exiting debug versions to use them

ren wow32d.*   wow32.*
ren ntvdmd.*   ntvdm.*
ren wowexecd.* wowexec.*
if exist krnl386d.exe ren krnl386d.* krnl386.*
if exist krnl286d.exe ren krnl286d.* krnl286.*
ren dosxd.*    dosx.*


echo.
echo.
echo.   Debugging of 16-bit applications has been enabled
echo.
echo.

goto exit

:error0

echo.
echo.
echo.   Not in system32 directory!!!
echo.
echo.

goto exit

:error1

echo.
echo.
echo.   Debugging has already been setup!
echo.
echo.

goto exit

:error2

echo.
echo.
echo.   Debugging has not been copied to the system32 directory yet!
echo.
echo.

goto exit

:error3

echo.
echo.
echo.   Remove failed due to either NTVDM still running or not having
echo.   enough privilege.  You must be logged in as an administrator to  
echo.   enable the debugging.  Log in as an administrator and try again.
echo.
echo.

:exit
