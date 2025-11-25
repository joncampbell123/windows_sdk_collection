@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctmidou.cmd - cmd file to run HCT MM Midi Output test
@rem

        set DEBUG=0

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        if "%2"=="-DEBUG" set DEBUG=1
        if "%3"=="-DEBUG" set DEBUG=1
        if "%4"=="-DEBUG" set DEBUG=1
        if "%5"=="-DEBUG" set DEBUG=1
        if "%6"=="-DEBUG" set DEBUG=1
        if "%7"=="-DEBUG" set DEBUG=1
        if "%8"=="-DEBUG" set DEBUG=1
        if "%9"=="-DEBUG" set DEBUG=1

 if %DEBUG%==1 goto DEBUG

	mouttst -g .\moutddk.pro
 goto donedebug
:DEBUG
 windbg -g mouttst -g .\moutddk.pro
:DONEDEBUG

	copy moutddk.log %HCTDIR%\logs
	del moutddk.log

        goto end

:usage
        echo.
	echo Usage: hctmidou [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end


