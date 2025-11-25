@echo off

@rem
@rem Windows/NT HCT
@rem
@rem hctwavin.cmd - cmd file to run HCT Wave Input test
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

	wavetst -g .\wavin.pro
 goto donedebug
:DEBUG
 windbg -g wavetst -g .\wavin.pro
:DONEDEBUG

	copy wavin.log %HCTDIR%\logs
	del wavin.log

        goto end

:usage
        echo.
	echo Usage: hctwavin [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end


