@echo off

@echo.
@rem
@rem   Windows NT HCT
@rem
@rem vdmstart.cmd - test ability to start a VDM
@rem
        set DEBUG=0

        if "%2"=="-DEBUG" set DEBUG=1
        if "%3"=="-DEBUG" set DEBUG=1
        if "%4"=="-DEBUG" set DEBUG=1
        if "%5"=="-DEBUG" set DEBUG=1
        if "%6"=="-DEBUG" set DEBUG=1
        if "%7"=="-DEBUG" set DEBUG=1
        if "%8"=="-DEBUG" set DEBUG=1
        if "%9"=="-DEBUG" set DEBUG=1

        if not _%1 == _ goto START
        if not _%2 == _ goto START
        if not _%3 == _ goto START

        echo Usage: vdmstart [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive where the HCTs are
        echo       [reserved] are not used but must be provided
        echo.

        goto EXIT

:START
        echo Starting a VDM...
        echo.
        echo FAIL > %HCTDIR%\logs\vdmstart.log
        
        %1
        cd %HCTDIR%

        if %DEBUG%==1 goto DEBUG

        command /c dir > %HCTDIR%\logs\vdmstart.log
        goto donedebug
:DEBUG
        windbg -g command /c dir > %HCTDIR%\logs\vdmstart.log
:DONEDEBUG
        
        if errorlevel 1 goto ERROR
        
        echo PASS >> %HCTDIR%\logs\vdmstart.log
        echo.
        echo Testing complete
        
        goto EXIT

:ERROR
        echo Error occured starting a VDM...
        echo FAILED >> %HCTDIR%\logs\vdmstart.log

:EXIT
