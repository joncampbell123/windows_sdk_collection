@echo off

@rem
@rem Windows/NT HCT
@rem
@rem npx.cmd - cmd file to run coprocessor tests
@rem
@rem %1 - hct drive
@rem %2 - reserved
@rem %3 - reserved
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

rem        setlocal

        set DEBUG=0

        if "%2"=="-DEBUG" set DEBUG=1
        if "%3"=="-DEBUG" set DEBUG=1
        if "%4"=="-DEBUG" set DEBUG=1
        if "%5"=="-DEBUG" set DEBUG=1
        if "%6"=="-DEBUG" set DEBUG=1
        if "%7"=="-DEBUG" set DEBUG=1
        if "%8"=="-DEBUG" set DEBUG=1
        if "%9"=="-DEBUG" set DEBUG=1

        if "%2"=="STRESS" goto stress

        echo STARTING Coprocessor HCT
        echo STARTING Coprocessor HCT > npx.log
        if %debug%==1 goto WINDBG
:NONDEBUG
        floater 20 10 > tmp1 > tmp2
        goto DONEDEBUG
:WINDBG
        windbg -g floater 20 10 > tmp1 > tmp2

:DONEDEBUG
        if errorlevel 1 goto failure
        echo PASS - Coprocessor HCT
        echo PASS - Coprocessor HCT >> npx.log

        copy npx.log %HCTDIR%\logs
        goto end

:stress
        if %debug%==1 goto WINDBGSTRS

:NONDEBUGSTRS
        for %%i in (1 2 3 4 5) do call npxstrs.cmd
        goto DONEDEBUGSTRS

:WINDBGSTRS
        for %%i in (1 2 3 4 5) do call npxstrs.cmd -DEBUG
:DONEDEBUGSTRS
        copy npxstrs.log %HCTDIR%\logs
        goto end

:usage
        echo.
        echo Usage: npx [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not used but are required.
        echo.
        goto end

:failure
        echo FAIL - Coprocessor HCT
        echo FAIL - Coprocessor HCT >> npx.log
        goto done

:end
        rem endlocal
