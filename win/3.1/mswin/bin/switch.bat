@echo off

set Is386=<<Is386>>
set WinPath=<<WindowsSystemDir>>
set DbgPath=<<SDKDebugDir>>
set NoDPath=<<SDKNoDebugDir>>

if (%NoDPath%)==(<<SDKNoDebugDir>>) goto ErrorChecking
    rem -----------------------------------
    rem  Display Out of Env. Space Message 
    rem -----------------------------------

    echo:
    echo: Error: Out of Environment Space !!
    echo:
    echo:        Increase the size of your environment using the 
    echo:        '/e' switch on COMMAND.COM.
    echo:        
    echo:        Example CONFIG.SYS:
    echo:
    echo:           SHELL=C:\COMMAND.COM /e:512
    echo:
    goto Done

:ErrorChecking
    if (%WinPath%)==() goto NoWinPath
    if (%1)==(N) goto NoDebug
    if (%1)==(D) goto Debug
    goto Usage

:NoWinPath
    rem -----------------------------
    rem  Display Set WinPath Message
    rem -----------------------------

    echo:
    echo: Error: Windows System Path not set !!
    echo:
    echo:       Set the "WinPath" environment variable in
    echo:       %0.BAT to your Windows System Directory.
    echo:       Also make sure you have copied NoDebug EXEs
    echo:       to your NoDebug directory (see readme.txt)
    echo:
    goto Done

:Usage
    rem --------------------------
    rem  Display SWITCH.BAT Usage
    rem --------------------------

    echo:
    echo: Usage:
    echo:        %0 {N or D}
    echo:
    echo: where  N = switch to nodebug version of windows
    echo:        D = switch to debug version of windows
    echo:
    echo: NOTE: %0 is normally called by N2D & D2N -- please use one of these
    echo:
    goto Done

:Debug
    rem ------------------------
    rem   Debug Initialization
    rem ------------------------

    echo Switching to Debug .EXEs, DLLs and .SYMs
    set NoDPath=
    set ToDir=%DbgPath%
    set DbgPath=
    set From=N
    goto SwitchFiles

:NoDebug
    rem ------------------------
    rem  NoDebug Initialization
    rem ------------------------

    echo Switching to NoDebug .EXEs, DLLs and .SYMs
    set DbgPath=
    set ToDir=%NoDPath%
    set NoDPath=
    set From=D

:SwitchFiles
    rem ------------------------------------
    rem  Group Loop Variable Initialization
    rem ------------------------------------

    set Grp=GDI

:GroupLoop
	rem -----------------------------------
	rem  Type Loop Variable Initialization
	rem -----------------------------------

	set Type=EXE

    :TypeLoop
	    echo     %Grp%.%Type%:

	    if exist %WinPath%\%Grp%%1.%Type%  goto ToFileExists

	    rem -----------------------------------
	    rem  Copy file directly to destination 
	    rem -----------------------------------

	    echo         %ToDir%\%Grp%.%Type%  to  %WinPath%\%Grp%.%Type%
	    copy %ToDir%\%Grp%.%Type% %WinPath%
	    goto DoneTypeLoop

	:ToFileExists
	    if not exist %WinPath%\%Grp%.%Type%        goto MoveToFile
	    if not exist %WinPath%\%Grp%%From%.%Type%  goto MoveDefault

	    rem ----------------------------
	    rem  Delete file to be replaced
	    rem ----------------------------

	    del %WinPath%\%Grp%.%Type%
	    goto MoveToFile

	:MoveDefault
	    rem ----------------------------
	    rem  Retain file to be replaced
	    rem ----------------------------

	    echo         %WinPath%\%Grp%.%Type%  to %WinPath%\%Grp%%From%.%Type%
	    rename %WinPath%\%Grp%.%Type% %Grp%%From%.%Type%

	:MoveToFile
	    rem ----------------------------
	    rem  Replace file with new file
	    rem ----------------------------

	    echo         %WinPath%\%Grp%%1.%Type% to %WinPath%\%Grp%.%Type%
	    rename %WinPath%\%Grp%%1.%Type% %Grp%.%Type%

	:DoneTypeLoop
	    rem ----------------------------
	    rem  Advance Type Loop Variable
	    rem ----------------------------

	    if %Type%==SYM goto DoneGroupLoop
	    if %Type%==EXE set Type=SYM
	    goto TypeLoop

    :DoneGroupLoop
	rem -----------------------------
	rem  Advance Group Loop Variable
	rem -----------------------------

	if %Grp%==KRNL386 goto MMGroupLoop
	if %Grp%==KRNL286 goto CheckFor386
	if %Grp%==USER    set Grp=KRNL286
	if %Grp%==GDI     set Grp=USER
	if %Grp%==MMSYSTEM set Grp=GDI
	goto GroupLoop


:MMGroupLoop
	rem ----------------------------------------------------------
	rem  Type Loop Variable Initialization for MMSYSTEM.DLL &.SYM
	rem ----------------------------------------------------------

	set Type=DLL

    :MMTypeLoop
	    echo     MMSYSTEM.%Type%:

	    if exist %WinPath%\MMSYSTE%1.%Type%  goto MMToFileExists

	    rem -----------------------------------
	    rem  Copy file directly to destination 
	    rem -----------------------------------

	    echo         %ToDir%\MMSYSTEM.%Type%  to  %WinPath%\MMSYSTEM.%Type%
	    copy %ToDir%\MMSYSTEM.%Type% %WinPath%
	    goto MMDoneTypeLoop

	:MMToFileExists
	    if not exist %WinPath%\MMSYSTEM.%Type%        goto MMMoveToFile
	    if not exist %WinPath%\MMSYSTE%From%.%Type%  goto MMMoveDefault

	    rem ----------------------------
	    rem  Delete file to be replaced
	    rem ----------------------------

	    del %WinPath%\MMSYSTEM.%Type%
	    goto MMMoveToFile

	:MMMoveDefault
	    rem ----------------------------
	    rem  Retain file to be replaced
	    rem ----------------------------

	    echo         %WinPath%\MMSYSTEM.%Type%  to %WinPath%\MMSYSTE%From%.%Type%
	    rename %WinPath%\MMSYSTEM.%Type% MMSYSTE%From%.%Type%

	:MMMoveToFile
	    rem ----------------------------
	    rem  Replace file with new file
	    rem ----------------------------

	    echo         %WinPath%\MMSYSTE%1.%Type% to %WinPath%\MMSYSTEM.%Type%
	    rename %WinPath%\MMSYSTE%1.%Type% MMSYSTEM.%Type%

	:MMDoneTypeLoop
	    rem ----------------------------
	    rem  Advance Type Loop Variable
	    rem ----------------------------

	    if %Type%==SYM goto Done
	    if %Type%==DLL set Type=SYM
	    goto MMTypeLoop


:CheckFor386
    rem -------------------------------------
    rem  Skip KRNL386 if machine isn't a 386
    rem -------------------------------------

    if (%Is386%)==(n) goto Done
    set Grp=KRNL386
    goto GroupLoop

:Done

set DbgPath=
set From=
set Grp=
set Is386=
set NoDPath=
set ToDir=
set Type=
set WinPath=
