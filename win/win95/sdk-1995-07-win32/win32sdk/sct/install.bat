echo off
@rem
@rem  This command file is used to move the 16-bit tools to your hard drive
@rem
@rem  install c:\winnt <x86|mips|alpha|ppc> <debug|retail|parmprof|windbg|ntsd>
@rem

@rem  Make sure root directory is correct

if "%1" == "" goto error1
if not exist %1\system32\progman.exe goto error1

@rem    Make sure the machine type is correct

if "%2" == "" goto error2
set who=xxx
if %2 == x86  set who=i386
if %2 == X86  set who=i386
if %2 == mips set who=mips
if %2 == Mips set who=mips
if %2 == MIPS set who=mips
if %2 == alpha set who=alpha
if %2 == Alpha set who=alpha
if %2 == ALPHA set who=alpha
if %2 == ppc set who=ppc
if %2 == Ppc set who=ppc
if %2 == PPC set who=ppc
if %who% == xxx  goto error2

@rem    Make sure the type of debug support is correct

if "%3" == "" goto error3
if %3 == debug  goto copydebug
if %3 == Debug  goto copydebug
if %3 == DEBUG  goto copydebug
if %3 == retail  goto copyretail
if %3 == Retail  goto copyretail
if %3 == RETAIL  goto copyretail
if %3 == parmprof goto copyparmprof
if %3 == Parmprof goto copyparmprof
if %3 == ParmProf goto copyparmprof
if %3 == PARMPROF goto copyparmprof
if %3 == WinDbg goto copywindbg
if %3 == windbg goto copywindbg
if %3 == Windbg goto copywindbg
if %3 == WINDBG goto copywindbg
if %3 == ntsd   goto copyntsd
if %3 == NTSD   goto copyntsd
goto error3

@rem
@rem    Copy the checkbins and make debugging active
@rem

:copydebug

@rem    Lets make sure we haven't done this already

@rem if exist %1\system32\wow32x.dll goto error10
if exist %1\system32\wow32d.dll goto error11

@rem    Make the directories

md %1\sct
md %1\sct\debug16

@rem    move regini.exe down if not there (also .ini files in debug16 and sct help file)

if not exist %1\system32\regini.exe copy .\debug16\%who%\regini.exe %1\system32
if not exist %1\sct\debug16\ntsdi386.ini copy .\debug16\ntsdi386.ini %1\sct\debug16
if not exist %1\sct\debug16\ntsdmips.ini copy .\debug16\ntsdmips.ini %1\sct\debug16
if not exist %1\sct\debug16\ntsdalph.ini copy .\debug16\ntsdalph.ini %1\sct\debug16
if not exist %1\sct\debug16\ntsdppc.ini copy .\debug16\ntsdppc.ini %1\sct\debug16
if not exist %1\sct\sct.hlp copy sct.hlp %1\sct

@rem    move pviewer.exe down if not there

if not exist %1\system32\pviewer.exe copy .\debug16\%who%\pviewer.exe %1\system32

echo.
echo.
echo.   Pviewer will start after you hit a key, make sure NTVDM is not running...
echo.
echo.
pause

%1\system32\pviewer.exe

@rem    move the remove and set debug command files for ease of use

if not exist %1\system32\remdebug.bat copy .\debug16\remdebug.bat %1\system32
if not exist %1\system32\setdebug.bat copy .\debug16\setdebug.bat %1\system32

@rem    rename the existing non-debug verison to save them

echo.
echo.                 
echo.   Renaming current files to save for remdebug...
echo.
echo.

if not exist %1\system32\wowexecx.exe ren %1\system32\wowexec.* wowexecx.*
if not exist %1\system32\wowexecx.exe goto error12
if not exist %1\system32\ntvdmx.exe ren %1\system32\ntvdm.*     ntvdmx.*
if not exist %1\system32\ntvdmx.exe goto error12
if not exist %1\system32\wow32x.dll ren %1\system32\wow32.*     wow32x.*
if not exist %1\system32\wow32x.dll goto error12
if not exist %1\system32\dosxx.exe ren %1\system32\dosx.*        dosxx.*
if not exist %1\system32\dosxx.exe goto error12
                            
if %who% == mips goto get286
if %who% == alpha goto get286
if %who% == ppc goto get286

if not exist %1\system32\krnl386x.exe ren %1\system32\krnl386.* krnl386x.*
if not exist %1\system32\krnl386x.exe goto error12

goto gotkrnl

:get286

if not exist %1\system32\krnl286x.exe ren %1\system32\krnl286.* krnl286x.*
if not exist %1\system32\krnl286x.exe goto error12

:gotkrnl

echo off

@rem    copy the debug version from their directory

echo.
echo.                 
echo.   Copying debug files to system32 directory...
echo.
echo.

copy .\debug16\%who%\*.sym     %1\system32
copy .\debug16\%who%\wow32.*   %1\system32
copy .\debug16\%who%\ntvdm.*   %1\system32
copy .\debug16\%who%\wowexec.* %1\system32
if exist %1\system32\krnl386x.exe copy .\debug16\%who%\krnl386.* %1\system32
if exist %1\system32\krnl286x.exe copy .\debug16\%who%\krnl286.* %1\system32
copy .\debug16\%who%\dosx.*    %1\system32
if exist .\debug16\%who%\bde.dll copy .\debug16\%who%\bde.dll %1\system32

echo.
echo.                 
echo.   Debugging of 16-bit applications is enabled now!
echo.
echo.

goto exit

@rem
@rem    Copy the Retail Sym files
@rem

:copyretail

echo.
echo.                 
echo.   Copying retail .Sym files to system32 directory...
echo.
echo.

copy .\retail16\%who%\*.sym     %1\system32

goto exit

@rem
@rem    Copy the parmprof and bring up the control panel for the path
@rem

:copyparmprof

echo.
echo.                 
echo.   Coping parmprof files to your hard drive...
echo.
echo.

md %1\sct
md %1\sct\parmprof
xcopy .\parmprof\*.* %1\sct\parmprof
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort
if errorlevel 0 goto parmprofdone
goto othererr           

:parmprofdone

echo.
echo.                 
echo.   Parmprof has now been copied to your hard drive.  Modify your path now.
echo.
echo.

goto exit

@rem
@rem    Copy Windbg
@rem


:copywindbg

if %who% == mips goto error20
if %who% == alpha goto error20
if %who% == ppc goto error20

echo.
echo.                 
echo.   Coping Windbg to your hard drive...
echo.
echo.

if not exist %1\mstools md %1\mstools
xcopy ..\mstools\bin\%who%\windbg.exe %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\windbgrm.exe %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\dm.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\emmip.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\emx86.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\emalp.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\shcv.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\symcvt.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\eecxxx86.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\eecxxmip.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\eecxxalp.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\tlloc.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\tlpipe.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\tlser.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\windbg.hlp %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\kdextx86.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\kdextmip.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\kdextalp.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\shcv.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\symcvt.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\symcvt.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\dmkdx86.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\dmkdmip.dll %1\mstools
if not errorlevel 0 goto windbgerr
xcopy ..\mstools\bin\%who%\dmkdalp.dll %1\mstools

if errorlevel 0 goto windbgdone

:windbgerr

if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort
if errorlevel 0 goto windbgdone
goto othererr           
                                  
:windbgdone

echo.
echo.                 
echo.   Windbg debugger has been copied to your hard drive.
echo.
echo.
goto exit

@rem
@rem    Copy ntsd
@rem

:copyntsd

echo.
echo.                 
echo.   Coping NTSD to your hard drive...
echo.
echo.

xcopy ..\mstools\bin\winnt\%who%\ntsd.exe %1\system32
if errorlevel 4 goto lowmemory
if errorlevel 2 goto abort
if errorlevel 0 goto ntsddone
goto othererr           

:ntsddone  

echo.
echo.                 
echo.   NTSD copied to hard drive
echo.
echo.

echo.
echo.                 
echo.   Modifing the registry
echo.
echo.

@set who4=%who%
@if %who% == alpha set who4=alph
.\debug16\%who%\regini.exe .\debug16\ntsd%who4%.ini

echo.
echo.                 
echo.   Modifing the registry complete...  NTSD is now setup.
echo.
echo.

goto exit

@rem
@rem    Error Messages
@rem

:othererr  

echo.
echo.
echo.   Other error code occurred!
echo.
echo.
goto exit

:lowmemory

echo.
echo.
echo.   Not enough memory or disk space
echo.
echo.
goto exit

:abort

echo.
echo.
echo.   User aborted
echo.
echo.
goto exit

:error1

echo.
echo.
echo.   Must tell install where your root of NT is (i.e. c:\winnt) as p1
echo.
echo.

:error2

echo.
echo.
echo.   Must tell install if a x86, mips, alpha, or ppc installation as p2
echo.
echo.                          

:error3

echo.
echo.
echo.   Must tell install if debug, parmprof, windbg, ntsd intallation as p3
echo.
echo.                          
goto exit

:error10

echo.
echo.
echo.   Debugging files have already been copied over and are active
echo.
echo.

goto exit

:error11

echo.
echo.
echo.   Debugging files have already been copied over use set debug
echo.
echo.

goto exit

:error12

echo.
echo.
echo.   Install failed due to either NTVDM still running or not having
echo.   enough privilege.  You must be logged in as an administrator to  
echo.   enable the debugging.  Log in as an administrator and try again.
echo.
echo.

goto exit

:error20

echo.
echo.
echo.   Can only install Windbg to a x86 machine
echo.
echo.

goto exit

:exit
 
