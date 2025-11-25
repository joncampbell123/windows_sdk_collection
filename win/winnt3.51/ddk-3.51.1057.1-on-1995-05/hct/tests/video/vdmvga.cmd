@echo off

@rem
@rem Windows/NT HCT
@rem
@rem vdmvga.cmd - cmd file to run vdm vga test
@rem
@rem %1 - reserved
@rem %2 - reserved
@rem %3 - reserved
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        del vdmvga.tmp vdmvga.log

	cmd /c vvideo1.bat 0 2
	cmd /c vvideo1.bat 1 2
	cmd /c vvideo1.bat 2 2
	cmd /c vvideo1.bat 3 2
	cmd /c vvideo1.bat 7 2

        @rem do video modes last (stay in windowed mode as long as possible)
        
	cmd /c vvideo2.bat 4 2
	cmd /c vvideo2.bat 5 2
	cmd /c vvideo2.bat 6 2
	cmd /c vvideo2.bat d 2
	cmd /c vvideo2.bat e 2
	cmd /c vvideo2.bat 11 2
	cmd /c vvideo2.bat 12 2
	cmd /c vvideo2.bat 13 2

        copy vdmvga.tmp %HCTDIR%\logs\vdmvga.log
        del *.log *.tmp

        goto end

:usage
        echo.
        echo Usage: vdmvga [HCT drive] [reserved] [reserved]
        echo.
        echo where
        echo       [HCT drive] is the drive the HCTs live on
        echo       [reserved] are not used but must have values
        echo.

:end
