@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctcdmnl.cmd - cmd file to run HCT MM MCI CD Audio Manual test
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

	mcigen -g -p .\mcicdmnl.pro
 goto donedebug
:DEBUG
 windbg -g mcigen -g -p .\mcicdmnl.pro
:DONEDEBUG

	copy mcicdmnl.log %HCTDIR%\logs
	del mcicdmnl.log

        goto end

:usage
        echo.
	echo Usage: hctcdmnl [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo  [reserved] parameters are not currently used but must have values
        echo.

:end


