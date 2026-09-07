@Echo Off
If x"%1" == x"" Goto check_env
Set MsSdk=%1
Goto MsSdk_ok

:check_env
If x"%MsSdk%" == x"" Goto usage

:MsSdk_ok
If Not Exist %MsSdk%\SetCole.Bat Goto usage

:testwin95
If x%PROCESSOR_ARCHITECTURE% == x Goto win95
Set Path=%MsSdk%\Bin\Win64\Cole;%MsSdk%\Bin;%MsSdk%\Bin\WinNT;%MsSdk%\Bin\Win64;%Path%
Goto setwin64

:win95
doskey
Set Processor_Architecture=x86
Set Path=%MsSdk%\Bin\Win64\Cole;%MsSdk%\Bin;%MsSdk%\Bin\Win95;%MsSdk%\Bin\Win64;%path%
Goto setwin64

:setwin64

Rem we ship CRT, MFC & ATL headers & libs & bins until Win64 VC ships

Set Lib=%MsSdk%\Lib\Win64;%MsSdk%\Lib\Win64\CRT;%MsSdk%\Lib\Win64\MFC;%MsSdk%\Lib\Win64\ATL
Set Include=%MsSdk%\Include;%MsSdk%\Include\Win64\CRT;%MsSdk%\Include\Win64\MFC;%MsSdk%\Include\Win64\ATL;%Include%
Set BkOffice=%MsSdk%\
Set BaseMake=%MsSdk%\Include\BKOffice.Mak
Set DXSDKRoot=%MsSdk%
Set INetSDK=%MsSdk%
Set MsSdk=%MsSdk%
Set MsTools=%MsSdk%
Set LOG=NO

If "%PROCESSOR_ARCHITECTURE%" == "ALPHA" Goto Alpha
If "%PROCESSOR_ARCHITECTURE%" == "x86" Goto x86
If "%PROCESSOR_ARCHITECTURE%" == "ALPHA64" Goto AXP64
If "%PROCESSOR_ARCHITECTURE%" == "IA64" Goto IA64

Echo Problem detected in SetWin64.Bat.  Please check environment.

Rem Set up appropriate cross-compiler environment

:x86
Set Cpu=i386
Goto IA64

:Alpha

Rem Same as AXP64 so fall through

:Alpha64
:AXP64
Set Cpu=ALPHA64
Goto end

:IA64

Rem Cole doesn't support all IA64 settings so run in i386 mode

Set Cpu=i386

Goto end

:end

Set COLE_64=1

Echo.
Echo Setting SDK environment relative to %MsSdk% targeting %Cpu% for Win64 development.
Echo.

If x"%MSDevDir%" == x"" Goto c_warn
Goto final

:c_warn
Echo.
Echo  Warning: check your LIB, INCLUDE and PATH to verify that the
Echo   SDK Lib, Include, and Bin directories precede the compiler
Echo   directories in the environment.
Echo.
Echo   Microsoft Visual Studio provides VCVARS32.BAT to Set them.
Echo   You must run VCVARS32.BAT first and then run SETWIN64.BAT.
Echo.
Echo  Current settings:
Echo.
Echo   LIB=%LIB%
Echo   INCLUDE=%INCLUDE%
Echo   PATH=%PATH%
Echo.
Goto final

:usage
Echo.
Echo  To Set the test 64-bit SDK environment, run SETWIN64.BAT
Echo  from your Platform SDK installation directory.
Echo.
Echo  Usage:   SETWIN64 ^<MsSdk^>
Echo  Where:   ^<MsSdk^> specifies where the Platform SDK was installed
Echo.
Echo  Example: C:\MsSdk\SETWIN64 C:\MsSdk sets the environment relative to C:\MsSdk
Echo.

:final
