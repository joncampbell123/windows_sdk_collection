echo off
@rem
@rem    This command file will make debugging NOT active.
@rem
@rem
@rem    Be in nt\system32 directory
@rem
@rem    cd \winnt\system32
@rem    ..\sct\debug16\remdebug
@rem

@rem    Try and put into system32 directory if we can

if not exist progman.exe cd %path%

@rem    Lets do some error checking

if not exist progman.exe goto error0
if exist wow32x.dll goto continue1
if exist wow32d.dll goto error1
goto error2

:continue1

echo.
echo.
echo.   Pviewer will start after you hit a key, make sure NTVDM is not running...
echo.
echo.
pause

pviewer.exe

@rem    rename the existing debug version to save them

if not exist wowexecd.exe  ren wowexec.* wowexecd.*
if not exist wowexecd.exe  goto error3
ren ntvdm.*   ntvdmd.*
ren wow32.*   wow32d.*
if exist krnl386.exe ren krnl386.* krnl386d.*
if exist krnl286.exe ren krnl286.* krnl286d.*
ren dosx.*    dosxd.*

@rem    rename the saved non-debug version to restore them to active

ren wow32x.*   wow32.*
ren ntvdmx.*   ntvdm.*
ren wowexecx.* wowexec.*
if exist krnl386x.exe ren krnl386x.* krnl386.*
if exist krnl286x.exe ren krnl286x.* krnl286.*
ren dosxx.*    dosx.*

echo.
echo.
echo.   Debugging for 16-bit applications has been removed
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
echo.   Debugging has already been removed!
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

goto exit

:exit
