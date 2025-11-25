rem @echo off
mode 80,300

@rem
@rem putlogs.cmd - grab HCT logs from user's machine
@rem

        echo.   
        echo Windows NT Hardware Compatibility Test
        echo --------------------------------------
        
        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage
        if "%4"=="" goto usage

        echo.
        echo.

        if "%3"=="HCT"  set _server=\\boss\logs
        if "%3"=="HCT"  echo Posting logs from the HCT lab to %_server%...
        if "%3"=="VHCT" set _server=\\scotland\logs
        if "%3"=="VHCT" echo Posting logs from the Virtual lab to %_server%...

        echo.
        echo.
        rem echo === Ignore any error messages ===

        net use z: /d
        net use z: %_server%

        z:
        if "%3"=="VHCT" cd \vlablogs
        md %4
        cd %4

        del %2.sum
        
        if "%5"=="MIPS" goto MIPS
        if %PROCESSOR_ARCHITECTURE% == MIPS goto MIPS
        if "%5"=="ALPHA" goto ALPHA
        if %PROCESSOR_ARCHITECTURE% == ALPHA goto ALPHA
        if "%5"=="PPC" goto PPC
        if %PROCESSOR_ARCHITECTURE% == PPC goto PPC
        echo.
        echo Decoding summary log...
        \slog -d %1 -h -v
        \slog -l %2.sum -s %3 -v 
        echo.
        echo Ok

:remainder
        echo.
        echo Copying individual logs
        echo.
        md %2
        copy %1\hct\logs %2

        rem peteg 6/13/94: add check for existance of logs
	if not exist %2.sum goto copyerror

	rem a-gregb 7/7/94: Grab config info using autocfg.exe
	echo.
	echo Gathering configuration information
	echo.
	\%PROCESSOR_ARCHITECTURE%\autocfg > %2.CFG
        goto done

:copyerror

@echo   *********************************

@echo   EEEE  RRRR   RRRR    OOO   RRRR
@echo   E     R   R  R   R  O   O  R   R
@echo   EE    RRRR   RRRR   O   O  RRRR
@echo   E     R   R  R  R   O   O  R  R
@echo   EEEE  R   R  R  R    OOO   R  R

@echo   Problem recording logs, please re-return results

@echo   *********************************
pause

:done
        %1

        net use z: /d
        goto end

:mips
        echo.
        echo Posting summary log...
        echo.

	       \mips\slog -d %1 -h -v
        \mips\slog -l %2.sum -s %3 -v 

        goto remainder

:alpha
        echo.
        echo Posting summary log...
        echo.

	\alpha\slog -d %1 -h -v
        \alpha\slog -l %2.sum -s %3 -v

        goto remainder

:ppc
        echo.
        echo Posting summary log...
        echo.

	       \ppc\slog -d %1 -h -v
        \ppc\slog -l %2.sum -s %3 -v

        goto remainder


:usage
        echo.
        echo Usage:     putlogs [HCT drive] [MachineID] [lab] [build] [MIPS|ALPHA|PPC]
        echo.
        echo where [HCT drive] is the drive you ran the HCTs from
        echo       [MachineID] is the test machine name (8 chars or less)
        echo       [lab]       is either HCT or VHCT
        echo       [build]     is the build # (318, etc)
        echo       [MIPS|ALPHA|PPC]      only if on R4000|Alpha AXP|PowerPC
        echo.
        echo For example:
        echo.
        echo       putlogs c: BobH486 HCT 318
        echo.


:end
