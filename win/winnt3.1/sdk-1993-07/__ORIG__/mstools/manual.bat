@echo off
cls
echo Manual Win32 SDK For Windows NT Installer 
echo =========================================
echo.
echo Note: This batch file should be used only if you do not have
echo a Windows NT supported SCSI card, CD player or network and are
echo installing the Win32 SDK onto a 386/486 system.
echo.
echo This batch file installs the complete Win32 SDK, including
echo development tools, compiler, samples and help files.
echo.
if "%1" == "" goto usage
echo BE SURE TO DELETE ANY PREVIOUS SDK INSTALLATION! THIS BATCH FILE
echo OVERWRITES ALL FILES IN THE %1\MSTOOLS DIRECTORY!
echo.
pause

rem Get SDK
mkdir %1\mstools
mkdir %1\mstools\bin
mkdir %1\mstools\h
mkdir %1\mstools\h\sys
mkdir %1\mstools\init
mkdir %1\mstools\lib
mkdir %1\mstools\samples
mkdir %1\mstools\mssetup
mkdir %1\mstools\mstest
mkdir %1\mstools\mstest\include
mkdir %1\mstools\mstest\sample
mkdir %1\mstools\posix
mkdir %1\mstools\posix\h
mkdir %1\mstools\posix\lib
mkdir %1\mstools\posix\samples
mkdir %1\mstools\posix\samples\psxarc
mkdir %1\mstools\mfc
mkdir %1\mstools\mfc\doc
mkdir %1\mstools\ole20
mkdir %1\mstools\ole20\samples
mkdir %1\mstools\ole20\samples\lib
mkdir %1\mstools\ole20\samples\bin
mkdir %1\mstools\ole20\samples\ole2ui
mkdir %1\mstools\ole20\samples\ole2ui\build
mkdir %1\mstools\ole20\samples\ole2ui\build\ship
mkdir %1\mstools\ole20\samples\ole2ui\build\debug
mkdir %1\mstools\ole20\samples\ole2ui\debug


copy  readme.txt %1\mstools
xcopy .\bin %1\mstools\bin 
xcopy .\bin\i386 %1\mstools\bin /s /e
xcopy .\h %1\mstools\h 
xcopy .\h\sys %1\mstools\h\sys 
xcopy .\h\i386 %1\mstools\h /s /e
xcopy .\help %1\mstools\bin
xcopy .\init %1\mstools\init /s /e
xcopy .\lib\i386 %1\mstools\lib /s /e
xcopy .\samples %1\mstools\samples /s /e
xcopy .\samples\largeint\i386 %1\mstools\samples\largeint 
xcopy .\samples\sdktools\image\imagehlp\i386 %1\mstools\samples\sdktools\image\imagehlp 
xcopy .\samples\sdktools\image\drwatson\i386 %1\mstools\samples\sdktools\image\drwatson 
xcopy .\mfc %1\mstools\mfc 
xcopy .\mfc\doc %1\mstools\mfc\doc 
xcopy .\mfc\src %1\mstools\mfc\src /s/e/i 
xcopy .\mfc\include %1\mstools\mfc\include /s/e/i 
xcopy .\mfc\samples %1\mstools\mfc\samples /s/e/i 
xcopy .\mfc\lib %1\mstools\mfc\lib /i
xcopy .\mfc\lib\i386 %1\mstools\mfc\lib /s/e
xcopy .\mssetup %1\mstools\mssetup 
xcopy .\mssetup\source %1\mstools\mssetup\source /s/e/i 
xcopy .\mssetup\include %1\mstools\mssetup\include /s/e/i 
xcopy .\mssetup\bldcui %1\mstools\mssetup\bldcui /s/e/i 
xcopy .\mssetup\lib\i386 %1\mstools\mssetup\lib /s/e/i 
xcopy .\mssetup\sample %1\mstools\mssetup\sample /i 
xcopy .\mssetup\sample\i386 %1\mstools\mssetup\sample /s/e/i 
xcopy .\mssetup\bin\i386 %1\mstools\mssetup\bin /s/e/i 
xcopy .\mssetup\disklay %1\mstools\mssetup\disklay /i 
xcopy .\mssetup\disklay\i386 %1\mstools\mssetup\disklay /s/e/i 
xcopy .\mssetup\intldll %1\mstools\mssetup\intldll /i 
xcopy .\mssetup\intldll\setupexe %1\mstools\mssetup\intldll\setupexe /i 
xcopy .\mssetup\intldll\setupexe\i386 %1\mstools\mssetup\intldll\setupexe /i 
xcopy .\mstest %1\mstools\mstest
xcopy .\mstest\i386 %1\mstools\mstest
xcopy .\mstest\include %1\mstools\mstest\include /s/e
xcopy .\mstest\sample %1\mstools\mstest\sample /s/e
xcopy .\posix %1\mstools\posix 
xcopy .\posix\h %1\mstools\posix\h /s/e
xcopy .\posix\samples %1\mstools\posix\samples /s/e
xcopy .\posix\lib\i386 %1\mstools\posix\lib /s/e
xcopy .\ole20 %1\mstools\ole20 
xcopy .\ole20\i386 %1\mstools\ole20 
xcopy .\ole20\bin %1\mstools\ole20\bin /i
xcopy .\ole20\bin\i386 %1\mstools\ole20\bin /i
xcopy .\ole20\lib\i386 %1\mstools\ole20\lib /i
xcopy .\ole20\h %1\mstools\ole20\h /s/e/i
xcopy .\ole20\h\i386 %1\mstools\ole20\h /s/e/i
xcopy .\ole20\samples %1\mstools\ole20\samples /s/e/i
xcopy .\ole20\samples\i386 %1\mstools\ole20\samples /s/e/i
xcopy .\ole20\samples\bttncur\i386 %1\mstools\ole20\samples\bttncur /s/e/i
xcopy .\ole20\samples\gizmobar\i386 %1\mstools\ole20\samples\gizmobar /s/e/i
xcopy .\ole20\samples\ole2ui\i386 %1\mstools\ole20\samples\ole2ui /s/e/i
xcopy .\ole20\samples\outline\i386 %1\mstools\ole20\samples\outline /s/e/i

echo set Path=%1\mstools\bin;%1\mstools\mstest;%%path%%> %1\mstools\setenv.bat
echo set Lib=%1\mstools\lib;%1\mstools\mfc\lib>> %1\mstools\setenv.bat
echo set Include=%1\mstools\h;%1\mstools\mfc\include;%1\mstools\bin;%1\mstools\mstest\include >> %1\mstools\setenv.bat
echo set CPU=i386>> %1\mstools\setenv.bat

goto end

:usage
echo.
echo Usage: manual [drive letter:]
echo Example: manual c: installs the development files into C:\MSTOOLS
echo.

:end
