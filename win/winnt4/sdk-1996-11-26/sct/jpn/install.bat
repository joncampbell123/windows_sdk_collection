echo off
@rem
@rem  This command file is used to set up the 16-bit debugging environment
@rem
@rem  Please make sure this batch file is called by \sct\install.bat
@rem
@rem  Usage:  install [pc98]
@rem

echo.
echo.
echo.   SCT 16-bit debugging support installation script for Japan.
echo.            
echo.                 

@rem  Make sure we can locate the system32 directory
if not exist %windir%\system32 goto error1

@rem  Make sure we can identify the machine processor
set who=xxx
if %PROCESSOR_ARCHITECTURE% == x86  set who=i386
if %PROCESSOR_ARCHITECTURE% == X86  set who=i386
if %PROCESSOR_ARCHITECTURE% == mips set who=mips
if %PROCESSOR_ARCHITECTURE% == Mips set who=mips
if %PROCESSOR_ARCHITECTURE% == MIPS set who=mips
if %PROCESSOR_ARCHITECTURE% == alpha set who=alpha
if %PROCESSOR_ARCHITECTURE% == Alpha set who=alpha
if %PROCESSOR_ARCHITECTURE% == ALPHA set who=alpha
if %PROCESSOR_ARCHITECTURE% == ppc set who=ppc
if %PROCESSOR_ARCHITECTURE% == Ppc set who=ppc
if %PROCESSOR_ARCHITECTURE% == PPC set who=ppc
if "%1" == "" goto chkwho
if %1 == pc98 set who=pc98
if %1 == Pc98 set who=pc98
if %1 == PC98 set who=pc98
:chkwho
if %who% == xxx  goto error2

@rem  See if optional country name is valid
@rem if "%1" == ""  goto keepgoing
@rem goto error3


@rem:keepgoing

@rem echo.
@rem echo.                 
@rem echo.   Creating SCT support directories... ( dirctory was already made by US inatall )
@rem echo.                 
@rem echo.                 

@rem md %windir%\sct ( dirctory was already made by US inatall )
if not exist %windir%\sct          goto error4

@rem md %windir%\sct\debug16 ( dirctory was already made by US inatall )
if not exist %windir%\sct\debug16  goto error4

@rem md %windir%\sct\retail16 ( dirctory was already made by US inatall )
if not exist %windir%\sct\retail16 goto error4

@rem md %windir%\sct\parmprof ( dirctory was already made by US inatall )
if not exist %windir%\sct\parmprof goto error4

@rem md %windir%\symbols ( dirctory was already made by US inatall )
if not exist %windir%\symbols      goto error4

@rem md %windir%\symbols\dll ( dirctory was already made by US inatall )
if not exist %windir%\symbols\dll  goto error4

@rem md %windir%\symbols\exe ( dirctory was already made by US inatall )
if not exist %windir%\symbols\exe  goto error4

echo.
echo.                 
echo.   Copying 16-bit debugging support files...
echo.                 
echo.                 

xcopy /V .  %windir%\sct
  if errorlevel 4 goto lowmemory
  if errorlevel 2 goto abort
  if errorlevel 0 goto next1
  goto othererr           
:next1

xcopy /V .\debug16\%who%    %windir%\sct\debug16
  if errorlevel 4 goto lowmemory
  if errorlevel 2 goto abort
  if errorlevel 0 goto next2
  goto othererr           
:next2

xcopy /V .\retail16\%who%   %windir%\sct\retail16
  if errorlevel 4 goto lowmemory
  if errorlevel 2 goto abort
  if errorlevel 0 goto next3
  goto othererr           
:next3

@rem xcopy /V .\parmprof\*.*     %windir%\sct\parmprof  ( Japan doesn't support parmprof )
@rem  if errorlevel 4 goto lowmemory
@rem  if errorlevel 2 goto abort
@rem  if errorlevel 0 goto next4
@rem  goto othererr           
@rem :next4

xcopy /V .\debug16\%who%\vdmexts.dll  %windir%\system32
  if errorlevel 4 goto lowmemory
  if errorlevel 2 goto abort
  if errorlevel 0 goto next5
  goto othererr           
:next5

xcopy /V .\debug16\%who%\ntsd.exe %windir%\system32
  if errorlevel 4 goto lowmemory
  if errorlevel 2 goto abort
  if errorlevel 0 goto next6
  goto othererr           
:next6


@rem echo.                 
@rem echo.                 
@rem echo.   Modifing the registry...
@rem echo.
@rem echo.
@rem
@rem .\debug16\%who%\regini.exe  .\sctreg.ini ( this was already done by US inatall )

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
goto badinstall


:lowmemory

echo.
echo.
echo.   Not enough memory or disk space
echo.
echo.
goto badinstall


:abort

echo.
echo.
echo.   User aborted
echo.
echo.
goto badinstall


:error1

echo.
echo.   Unable to locate NT system components
echo.
goto badinstall


:error2

echo.
echo.   Unable to identify machine processor type
echo.
goto badinstall


:error3

echo.
echo.
echo.   Install requires no parameters.
echo.
echo.
goto badinstall


:error4

echo.
echo.
echo.   Unable to create required directories
echo.
echo.
goto badinstall




:alldone

echo.
echo.
echo.   Installation completed successfully.
echo.                 
echo.                 
echo.   Please reboot your system to enable 16-bit debugging capabilities.
echo.
echo.  
goto exit



:badinstall

echo.
echo.
echo.   An error occurred while installing SCT 16-bit debugging support.
echo.
echo.   The installation was NOT successful.
echo.


:exit
