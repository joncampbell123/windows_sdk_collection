echo off
@rem
@rem  Restores 16-bit debugging environment to use non-debug binaries & symbols
@rem
@rem

if not exist %windir%\sct                  goto error1
if not exist %windir%\sct\debug16          goto error1
if not exist %windir%\sct\retail16         goto error1
if not exist %windir%\symbols\dll          goto error1
if not exist %windir%\symbols\exe          goto error1
if not exist %windir%\sct\debug16\kill.exe goto error1

if not exist %windir%\system32 goto error2

@rem determine if wow32.dll can be terminated
echo nondebug will now kill all 16 bit applications
pause
%windir%\sct\debug16\kill -f ntvdm
del %windir%\system32\wow32.dll
if errorlevel 2 goto error3

xcopy /V %windir%\sct\retail16\wow32.dll %windir%\system32
  if errorlevel 4 goto lowmemory
  if errorlevel 2 goto abort
  if errorlevel 0 goto next1
  goto othererr
:next1

xcopy /V %windir%\sct\retail16\wow32.dbg %windir%\symbols\dll
  if errorlevel 4 goto lowmemory
  if errorlevel 2 goto abort
  if errorlevel 0 goto next2
  goto othererr
:next2

xcopy /V %windir%\sct\retail16\ntvdm.exe %windir%\system32
  if errorlevel 4 goto lowmemory
  if errorlevel 2 goto abort
  if errorlevel 0 goto next3
  goto othererr
:next3

xcopy /V %windir%\sct\retail16\ntvdm.dbg %windir%\symbols\exe
  if errorlevel 4 goto lowmemory
  if errorlevel 2 goto abort
  if errorlevel 0 goto next4
  goto othererr
:next4

xcopy /V %windir%\sct\retail16\wowexec.exe %windir%\system32
  if errorlevel 4 goto lowmemory
  if errorlevel 2 goto abort
  if errorlevel 0 goto next5
  goto othererr
:next5

xcopy /V %windir%\sct\retail16\wowexec.sym %windir%\system32
  if errorlevel 4 goto lowmemory
  if errorlevel 2 goto abort
  if errorlevel 0 goto next6
  goto othererr
:next6

goto alldone


@rem
@rem    Error Messages
@rem


:othererr

echo.
echo.
echo.   Xcopy error occurred!
echo.
echo.
goto nocando


:lowmemory

echo.
echo.
echo.   Not enough memory or disk space
echo.
echo.
goto nocando


:abort

echo.
echo.
echo.   User aborted
echo.
echo.
goto nocando


:error1

echo.
echo.
echo.   The SCT 16-bit debugging environment has not been installed correctly
echo.   or has been altered.
echo.   Please run the install.bat script from the SCT CD.
echo.   
echo.
goto nocando


:error2

echo.
echo.
echo.   Unable to locate NT system components.
echo.   
echo.
goto nocando


:error3

echo.
echo.
echo.   Unable to shut down the VDM.  You either have not run the SCT install
echo.   script or you do not have administrator privilege which is required
echo.   to run this script.
echo.
echo.
goto nocando


:alldone

echo.
echo.
echo.   16-bit RETAIL debugging components are now in place.
echo.
echo.
goto exit


:nocando

echo.
echo.
echo.   Unable to complete placement of 16-bit debugging components.
echo.
echo.


:exit
