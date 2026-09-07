@echo off
IF "%OS%" == "Windows_NT" setlocal

set _CLEANMSIDIR=msi
set _BINDIR=WinNT\x86

IF "%OS%" == "Windows_NT" IF /i x%PROCESSOR_ARCHITECTURE%==xALPHA set _BINDIR=WinNT\Alpha
IF NOT "%OS%" == "Windows_NT" set _BINDIR=Win95

Echo Removing Platform SDK MSI keys (Pre-Win2K Beta3)...
call %_CLEANMSIDIR%\%_BINDIR%\msizap.exe T! {AB2365AB-80F2-11D2-989A-00C04F7978A9}
echo.

Echo Removing Platform SDK MSI keys (Win2K Beta3)...
call %_CLEANMSIDIR%\%_BINDIR%\msizap.exe T! {e4429075-F9CE-11D2-A0EC-009027342177}
echo.

Echo Removing Platform SDK MSI keys (Win2K Beta3 Web-Post)...
call %_CLEANMSIDIR%\%_BINDIR%\msizap.exe T! {1FCDACA0-0A5F-11D3-97F1-0000F81F5937}
echo.

Echo Removing Platform SDK MSI keys (Win2K RC1)...
call %_CLEANMSIDIR%\%_BINDIR%\msizap.exe T! {BD139179-168F-4021-B2FC-C7374B12B472}
echo.

Echo Removing Platform SDK MSI keys (Win2K RC2)...
call %_CLEANMSIDIR%\%_BINDIR%\msizap.exe T! {6299BF98-F26A-43A4-AFD4-4E3B099FE5C5}
echo.

Echo Removing Platform SDK MSI keys (Win2k RC3/Win2K RTM/Jan 2000 PSDK)...
call %_CLEANMSIDIR%\%_BINDIR%\msizap.exe T! {11B187BC-FE20-4903-9FDF-0882A471DBB9}
echo.

Echo Removing "Platform SDK" MSI keys (April 2000 PSDK)...
call %_CLEANMSIDIR%\%_BINDIR%\msizap.exe T! {A7FD291B-AAD6-493E-9B6D-7B64AE57667E}
echo.

Echo Removing "Platform SDK" MSI keys (Win64 pre-beta)...
call %_CLEANMSIDIR%\%_BINDIR%\msizap.exe T! {0F0F2BEA-FAE1-4DAB-AEDF-2CADC3135BB7}
echo.

Echo Removing "Platform SDK" MSI keys (Post Win2K)...
call %_CLEANMSIDIR%\%_BINDIR%\msizap.exe T! {363DBE23-B712-403D-9758-6CE4313002BB}
echo.

Echo Removing InProgress MSI keys
call %_CLEANMSIDIR%\%_BINDIR%\msizap.exe P
echo.

:: Cleanup and exit
:end
IF "%OS%" == "Windows_NT" endlocal