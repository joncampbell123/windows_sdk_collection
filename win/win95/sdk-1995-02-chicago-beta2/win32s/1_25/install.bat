@echo off
cls
echo °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
echo °                                                                       °
echo ° Win32s Development File Installation Script                           °
echo ° -------------------------------------------                           °
echo °                                                                       °
echo ° The Win32s development files consist of Setup Toolkit components      °
echo ° to assist creating Setup programs that will install your Win32        °
echo ° application and Win32s binaries onto Windows 3.1                      °
echo °                                                                       °
echo ° A debugging version of Win32s along with symbol files for both        °
echo ° the debug and nodebug versions of Win32s system components are        °
echo ° also installed.  These files are for testing and debugging.           °
echo °                                                                       °
echo ° These development files are intended for use with the Windows 3.1 SDK °
echo ° and products that bundle the Windows 3.1 SDK components.              °
echo °                                                                       °
echo °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°
if "%1" == "" goto usage


if exist %1\win32s\bin\switch.bat goto exist

pause
xcopy . %1\win32s\ /s /e
del %1\win32s\install.bat

goto end

:exist
echo °                                                                       °
echo ° Installation destination already exists. Please give another          °
echo ° destination, or remove it.                                            °
echo °                                                                       °
echo °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

goto end

:usage
echo °                                                                       °
echo ° Usage: install [drive letter:]                                        °
echo ° Example: install c:                                                   °
echo °          This installs the development files into C:\WIN32S           °
echo °                                                                       °
echo °°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°°

:end
