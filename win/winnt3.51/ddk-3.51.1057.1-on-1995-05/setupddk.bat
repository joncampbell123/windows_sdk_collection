@echo off

rem If the OS variable is not found, it is assumed we are not installing
rem under Windows NT.  If is also possible to force a manual install under
rem Windows NT by removing the OS variable from the environment.

if NOT "%OS%"=="Windows_NT" goto manual

rem If we find the inf file, run setup.  Otherwise, follow the manual path.

if exist %1\ddk\ddk.inf goto setup

set noinf=true
goto manual

:setup

setup /f /v /i %1\ddk\ddk.inf /s %1\
goto end

:manual

cls
echo.
echo.
echo                     Windows NT Device Driver Kit
echo                     ----------------------------
echo.
echo This script copies the device driver sources, header files, libraries
echo and tools to the drive and directory specified.  These files, when
echo used together with the Win32 SDK, create the complete development
echo environment necessary for developing Windows NT device drivers.
echo.
echo If you have not already done so, it is recommended that you read
echo "Getting Started" as well as the readme file(s) to familiarize yourself
echo with the various features of the Windows NT DDK.
echo.
echo.

rem If we don't have any params or the inf file was not found, show
rem appropriate usage text.

if "%noinf%"=="true" goto usage
if "%1"=="" goto manusage
if "%2"=="" goto manusage

:start

echo.
echo NOTE: The DDK tree has changed since the last release.  It is
echo       recommended that you install the Windows NT DDK to a new directory
echo       than the previous release.  See the release notes for details.
echo.
echo.
echo This script will copy the %2 DDK files to %1
echo.
echo.
echo Press Ctrl-C to interrupt this script . . . or . . .
pause
echo.
echo.

rem copy all files over

xcopy .\ddk\bin %1\bin\
xcopy .\ddk\bin\%2\free %1\bin\
xcopy .\ddk\bin\%2\free\sys %1\bin\free\sys\
xcopy .\ddk\bin\%2\free\symbols %1\bin\free\symbols\ /s /e
xcopy .\ddk\bin\%2\checked %1\bin\checked\ /s /e
xcopy .\ddk\doc %1\doc\
xcopy .\ddk\hlp %1\bin\
xcopy .\ddk\inc %1\inc\
xcopy .\ddk\lib\%2 %1\lib\%2\ /s /e
xcopy .\ddk\src %1\src\ /s /e

rem make top level dirs file

echo DIRS=src> %1\dirs

goto done

:usage

echo.
echo Usage: setupddk [source_directory]
echo.
echo   Example:  setupddk f:         installs DDK from f:\
echo   Example:  setupddk x:\ddk     installs DDK from x:\ddk
echo   Example:  setupddk            installs DDK from the root of current drive
echo.
echo.

set noinf=
goto end

:manusage

echo.
echo Usage: setupddk directory platform (platform=i386, mips, alpha, or ppc)
echo.
echo   Example:  setupddk c:\ddk mips
echo.
echo Installs the MIPS R4000 DDK tree into c:\ddk.  Note: There is no
echo requirement to install the Windows NT DDK and Win32 SDK on the
echo same partition.
echo.
echo.
goto end

:done

echo.
echo.
echo The Windows NT DDK installation is complete.
echo.
echo To build the driver sources, you must be running Windows NT
echo and have the Win32 SDK installed.  Switch to the %1 directory
echo that you specified via SETUPDDK.BAT:
echo.
echo   1) run "%1\bin\setenv.bat %1"
echo   2) type "build"
echo.
echo This will build all of the Windows NT device drivers provided
echo with this kit. This will assure successful installation.
echo.
echo.

:end
