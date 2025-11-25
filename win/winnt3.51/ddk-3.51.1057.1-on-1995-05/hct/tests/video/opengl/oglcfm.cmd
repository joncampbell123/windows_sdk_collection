@echo off

@rem
@rem Windows/NT HCT
@rem
@rem oglcfm.cmd - cmd file to run video OpenGL test
@rem

        set DEBUG=0

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        set DEBUG=0

        if "%2"=="-DEBUG" set DEBUG=1
        if "%3"=="-DEBUG" set DEBUG=1
        if "%4"=="-DEBUG" set DEBUG=1
        if "%5"=="-DEBUG" set DEBUG=1
        if "%6"=="-DEBUG" set DEBUG=1
        if "%7"=="-DEBUG" set DEBUG=1
        if "%8"=="-DEBUG" set DEBUG=1
        if "%9"=="-DEBUG" set DEBUG=1

	del oglpixel.log

 if %DEBUG%==1 goto debug

	cmd /c oglpixel.bat
 goto debugdone
:DEBUG
 cmd /c oglpixel.bat -DEBUG
:DEBUGDONE
rem bug bug peteg 3/31/94: add logupr hack util. to manually move
rem                        fail cases to upper case.
rem                        tests should get fixed eventually!
	logupr oglpixel.log
	copy oglpixel.log %HCTDIR%\logs\oglpixel.log

        goto end

:usage
        echo.
        echo Usage: oglcfm.cmd [hct drive] [reserved] [reserved]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo.

:end

