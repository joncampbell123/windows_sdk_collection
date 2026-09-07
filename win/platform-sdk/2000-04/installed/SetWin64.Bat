@ECHO OFF
REM -------------------------------------------------------------------
REM File    : SetWin64.Bat
REM
REM Usage   : SetWin64.Bat
REM
REM Abstract: This batch file sets the appropriate environment
REM           variables for the Platform SDK Win64 build envionment with
REM           respect to OS and platform type.
REM
REM -------------------------------------------------------------------

:Chk_MSSdk
REM Set the common env. variables
Goto Set_Common

:Chk_SetWin64
REM Verify that we have %MSSdk%\SetWin64.Bat
IF NOT EXIST "%MSSdk%\SetWin64.Bat" Goto ErrorBadPath

REM Check for the presence of MsVC5 or MsVC6.
goto Chk_MsVC
:MsVcExists

:Finish
Goto end


REM -------------------------------------------------------------------
REM Set common variables
:Set_Common
REM -------------------------------------------------------------------
Set MSSdk=C:\Program Files\Microsoft Platform SDK
Set Bkoffice=%MSSdk%\
Set Basemake=%MSSdk%\Include\BKOffice.Mak
Set DXSDKROOT=%MSSdk%
Set INETSDK=%MSSdk%
Set MSSdk=%MSSdk%
Set Mstools=%MSSdk%

Rem we ship CRT, MFC & ATL headers & libs & bins until Win64 VC ships
Set Include=%MSSdk%\Include;%MSSdk%\Include\Win64\CRT;%MSSdk%\Include\Win64\MFC;%MSSdk%\Include\Win64\ATL;%Include%
Set Lib=%MSSdk%\Lib\Win64;%MSSdk%\Lib\Win64\CRT;%MSSdk%\Lib\Win64\MFC;%MSSdk%\Lib\Win64\ATL

REM Set OS specific variables
IF "x%OS%x" == "xWindows_NTx" Goto Set_WinNT
Goto Set_Win9x


REM -------------------------------------------------------------------
REM Set Windows 9x specific variables
:Set_Win9x
REM -------------------------------------------------------------------
Echo Setting SDK environment relative to %MSSdk%. Targeting Windows 9x for Win64 development.
Set PROCESSOR_ARCHITECTURE=x86
Set CPU=IA64
Set CL=-Zs
Set Path="%MSSdk%\Bin\Win64;%MSSdk%\Bin;%MSSdk%\Bin\Win95;%path%"

doskey > Nul
Goto Chk_SetWin64


REM -------------------------------------------------------------------
REM Set Windows NT specific variables
:Set_WinNT
REM -------------------------------------------------------------------
REM Set up appropriate cross-compiler environment
IF /i "%PROCESSOR_ARCHITECTURE%" == "ALPHA64" (Set CPU=ALPHA64)
IF /i "%PROCESSOR_ARCHITECTURE%" == "ALPHA" (Set CPU=ALPHA64) 
IF /i "%PROCESSOR_ARCHITECTURE%" == "AXP64" (Set CPU=ALPHA64)
IF /i "%PROCESSOR_ARCHITECTURE%" == "x86" (Set CPU=IA64)
IF /i "%PROCESSOR_ARCHITECTURE%" == "IA64" (Set CPU=IA64)

REM IA64 in Platform SDK is only the front-end for syntax checking.  Force -Zs
IF /i "%CPU%" == "IA64" (Set CL=-Zs)

Echo.
Echo Setting SDK environment relative to %MSSdk%. Targeting Windows NT (%Cpu%) for Win64 development.
Echo.

Set Path=%MSSdk%\Bin\Win64;%MSSdk%\Bin;%MSSdk%\Bin\WinNT;%path%"

Goto Chk_SetWin64


REM -------------------------------------------------------------------
:Chk_MsVC
REM -------------------------------------------------------------------
If "x%MSVCDir%x" == "xx" Goto NotMsVcDirExists
If "x%MSDevDir%x" == "xx" Goto NotMsDevDirExists
REM We have MSDevDir and MSVCDir
goto MsVcExists

REM -------------------------------------------------------------------
:NotMsVcDirExists
REM -------------------------------------------------------------------
REM MSVCDir does not exist, does MSDevDir Exist?
IF "x%MSDevDir%x"=="xx" Goto ErrorVCEnv

REM MSDevDir Exists. Can we derive MSVCDir from MSDevDir?
REM Check for VC6
IF NOT EXIST "%MSDevDir%\..\..\vc98\." Goto ChkVC5_MsVCDir
set MSVCDir=%MSDevDir%\..\..\vc98
goto MsVcExists

REM Check for VC5
:ChkVC5_MsVCDir
IF NOT EXIST "%MSDevDir%\..\VC\." Goto ErrorVCEnv
set MSVCDir=%MSDevDir%\..\VC
goto MsVcExists

REM -------------------------------------------------------------------
:NotMsDevDirExists
REM -------------------------------------------------------------------
REM MSDevDir does not exist, does MSVCDir Exist?
IF "x%MSVCDir%x"=="xx" Goto ErrorVCEnv

REM MSVCDir Exists. Can we derive MSDevDir from MSVCDir?
REM Check for VC6
IF NOT EXIST "%MSVCDir%\..\Common\msdev98\." Goto ChkVC5_MSDevDir
set MSDevDir=%MSVCDir%\..\Common\msdev98
goto MsVcExists

REM Check for VC5
:ChkVC5_MSDevDir
IF NOT EXIST "%MSVCDir%\..\SharedIDE\." Goto ErrorVCEnv
set MSDevDir=%MSVCDir%\..\SharedIDE
goto MsVcExists


REM -------------------------------------------------------------------
:ErrorVCEnv
REM -------------------------------------------------------------------
Echo.
Echo Warning: The environmental variables MSDevDir and MSVCDir
Echo  were not found to exist. Check your Lib, Include and PATH to
Echo  verify that the SDK Lib, Include, and Bin directories precede
Echo  the compiler directories in the environment.
Echo.
Echo Note: Microsoft Visual Studio provides VCVARS32.BAT to Set them.
Echo  You must run VCVARS32.BAT first and then run SETWIN64.BAT.
Echo.
Echo Current settings:
Echo Lib=%Lib%
Echo Include=%Include%
Echo PATH=%PATH%
Echo.
Goto Finish


REM -------------------------------------------------------------------
:ErrorBadPath
REM -------------------------------------------------------------------
Echo.
Echo Error: The file "%MSSdk%\SetWin64.bat" does not appear to exist, or
Echo        an existing MSSdk env. variable does not match the expected
Echo        value encoded in this batch file by Platform SDK Setup.
Echo        Please check the path and an existing MSSdk variable
Echo        for correctness.
Echo.
Goto Finish


REM -------------------------------------------------------------------
:end
REM -------------------------------------------------------------------