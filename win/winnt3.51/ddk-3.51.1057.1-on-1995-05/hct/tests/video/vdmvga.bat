@echo off

@rem
@rem Windows/NT HCT
@rem
@rem vdmvga.cmd - cmd file to run vdm vga test
@rem
@rem

        del vdmvga.tmp vdmvga.log

	command /c vvideo1.bat 0 2
	command /c vvideo1.bat 1 2
	command /c vvideo1.bat 2 2
	command /c vvideo1.bat 3 2
	command /c vvideo1.bat 7 2

        @rem do video modes last (stay in windowed mode as long as possible)
        
	command /c vvideo2.bat 4 2
	command /c vvideo2.bat 5 2
	command /c vvideo2.bat 6 2
	command /c vvideo2.bat d 2
	command /c vvideo2.bat e 2
	command /c vvideo2.bat 11 2
	command /c vvideo2.bat 12 2
	command /c vvideo2.bat 13 2

	copy vdmvga.tmp vdmvga.log
	
        del *.tmp *.tst

        goto end

:usage
        echo.
        echo Usage: vdmvga.bat
        echo.

:end
