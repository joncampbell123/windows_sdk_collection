@echo off
cls
echo Manual Win32 SDK For Windows NT Installer 
echo =========================================
echo.
echo Note: This batch file should be used only if you do not have
echo a supported SCSI card, CD player or network and are installing
echo the Win32 SDK onto an X86 system.
echo.
if "%1" == "" goto usage
echo BE SURE TO DELETE ANY PREVIOUS SDK INSTALLATION! THIS BATCH FILE
echo OVERWRITES ALL FILES IN THE %1\MSTOOLS DIRECTORY!
echo.
pause

rem Get SDK
mkdir %1\mstools
mkdir %1\mstools\bin
mkdir %1\mstools\bin\logger32
mkdir %1\mstools\include
mkdir %1\mstools\include\gl
mkdir %1\mstools\init
mkdir %1\mstools\lib
mkdir %1\mstools\help
mkdir %1\mstools\samples
mkdir %1\mstools\samples\win32
mkdir %1\mstools\mssetup
mkdir %1\mstools\mssetup\source  
mkdir %1\mstools\mssetup\include
mkdir %1\mstools\mssetup\bldcui 
mkdir %1\mstools\mssetup\lib 
mkdir %1\mstools\mssetup\sample 
mkdir %1\mstools\mssetup\bin 
mkdir %1\mstools\mssetup\disklay 
mkdir %1\mstools\mssetup\intldll  
mkdir %1\mstools\mssetup\intldll\setupexe  
mkdir %1\mstools\posix
mkdir %1\mstools\posix\h
mkdir %1\mstools\posix\lib
mkdir %1\mstools\posix\samples
mkdir %1\mstools\posix\samples\psxarc


xcopy .\bin %1\mstools\bin 
xcopy .\bin\i386 %1\mstools\bin /s /e
xcopy .\bin\winnt %1\mstools\bin 
xcopy .\bin\winnt\i386 %1\mstools\bin /s /e
xcopy .\bin\winnt\logger32\i386 %1\mstools\bin\logger32
xcopy .\include %1\mstools\include
xcopy .\include\gl %1\mstools\include\gl
xcopy .\include\i386 %1\mstools\include /s /e
xcopy .\include\winnt %1\mstools\include /s /e
xcopy .\help %1\mstools\help
xcopy .\init %1\mstools\init /s /e
xcopy .\lib\i386 %1\mstools\lib /s /e
xcopy .\lib\winnt\i386 %1\mstools\lib /s /e
xcopy .\samples %1\mstools\samples /s /e
xcopy .\samples\sdktools\image\imagehlp\i386 %1\mstools\samples\sdktools\image\imagehlp 
xcopy .\samples\sdktools\image\drwatson\i386 %1\mstools\samples\sdktools\image\drwatson 
xcopy .\samples\sdktools\image\lib\i386      %1\mstools\samples\sdktools\image\lib
xcopy .\samples\sdktools\image\pfmon\i386    %1\mstools\samples\sdktools\image\pfmon
xcopy .\mssetup %1\mstools\mssetup 
xcopy .\mssetup\source %1\mstools\mssetup\source /s/e 
xcopy .\mssetup\include %1\mstools\mssetup\include /s/e 
xcopy .\mssetup\bldcui %1\mstools\mssetup\bldcui /s/e
xcopy .\mssetup\lib\i386 %1\mstools\mssetup\lib /s/e
xcopy .\mssetup\sample %1\mstools\mssetup\sample 
xcopy .\mssetup\sample\i386 %1\mstools\mssetup\sample /s/e 
xcopy .\mssetup\bin\i386 %1\mstools\mssetup\bin /s/e
xcopy .\mssetup\disklay %1\mstools\mssetup\disklay 
xcopy .\mssetup\disklay\i386 %1\mstools\mssetup\disklay /s/e 
xcopy .\mssetup\intldll %1\mstools\mssetup\intldll 
xcopy .\mssetup\intldll\setupexe %1\mstools\mssetup\intldll\setupexe  
xcopy .\mssetup\intldll\setupexe\i386 %1\mstools\mssetup\intldll\setupexe 
xcopy .\posix %1\mstools\posix 
xcopy .\posix\h %1\mstools\posix\h /s/e
xcopy .\posix\samples %1\mstools\posix\samples /s/e
xcopy .\posix\lib\i386 %1\mstools\posix\lib /s/e

rem Create the setenv.bat file which sets the SDK build environment
echo set Path=%1\mstools\bin;%%path%%> %1\mstools\setenv.bat
echo set Lib=%1\mstools\lib;%1\mstools\mssetup\lib;%%lib%%>> %1\mstools\setenv.bat
echo set Include=%1\mstools\include;%1\mstools\mssetup\include;%%include%% >> %1\mstools\setenv.bat
echo set CPU=i386>> %1\mstools\setenv.bat
echo set MSTOOLS=%1\mstools>> %1\mstools\setenv.bat

rem Create (or add to) the msin.ini file for the browser.
echo [WIN32SDK.Settings]                     >>%windir%\msin.ini
echo licensed.name=WNT SDK User              >>%windir%\msin.ini
echo licensed.organization=                  >>%windir%\msin.ini
echo license_version=N/A                     >>%windir%\msin.ini
echo license_last_read=N/A                   >>%windir%\msin.ini
echo [WIN32SDK.MVB]                          >>%windir%\msin.ini
echo title = Win32 Software Development Kit  >>%windir%\msin.ini
echo LocalDir= %1\mstools\help               >>%windir%\msin.ini
echo Path= %1\mstools\help                   >>%windir%\msin.ini
echo [series]                                >>%windir%\msin.ini
echo WIN32SDK = WIN32SDK.MVB                 >>%windir%\msin.ini
echo [all titles]                            >>%windir%\msin.ini
echo WIN32SDK.MVB = WIN32SDK.MVB             >>%windir%\msin.ini

goto end

:usage
echo.
echo Usage: manual [drive letter:]
echo Example: manual c: installs the development files into C:\MSTOOLS
echo.

:end
