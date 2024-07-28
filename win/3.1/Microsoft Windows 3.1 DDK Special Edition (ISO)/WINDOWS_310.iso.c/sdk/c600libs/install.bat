@echo off
echo.
echo Build C Runtime Libraries for Windows
echo --------------------------------------
if %1.==. goto HELP
if %2.==. goto HELP
if not %3.==. goto HELP

:DOIT
echo Deleting old copies of C Runtime Libraries for Windows...
for %%i in ( s m c l ) do if exist %1\%%ilibcew.lib del %1\%%ilibcew.lib
for %%i in ( s m c l ) do if exist %1\%%idllcew.lib del %1\%%idllcew.lib

echo Copying NOCRT libraries
xcopy lib\?nocrt*.lib %1\

echo Copying header files for libraries
xcopy include\*.* %2\

echo Building C Runtime Libraries for Windows
for %%i in ( s m c l ) do lib %1\%%ilibcew+lib\%%ilibcw.lib+lib\libh.lib+lib\%%ilibfpw.lib;
for %%i in ( s m c l ) do lib %1\%%idllcew+lib\%%idllcw.lib+lib\libh.lib+lib\%%ilibfpw.lib;

echo Done!
goto DONE

:HELP
echo.
echo This batch file installs the Windows versions of the C runtime libraries.
echo To use it, follow these installation instructions
echo.
echo            install "lib" "include"
echo.
echo where:
echo            "lib" = directory for runtime libraries
echo            "include" = directory for runtime header files
echo.
echo.
echo example:
echo            install c:\windev\lib c:\windev\include
echo.
echo note:
echo            LIB.EXE must be on the current path.
echo.
goto DONE

:DONE
