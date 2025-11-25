@echo off

@rem
@rem Windows/NT HCT
@rem
@rem multiser.cmd - cmd file to run x86 mulitport serial tests on COM1
@rem
        
        if "%4"=="" goto usage

        setlocal

        del *.log
        rats multi%4.rat
        copy multi%4.log \hct\logs\multiser.log

        goto end

:usage
        echo.
        echo Usage: multiser [hct drive] [reserved] [reserved] [#ports] 
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [port] is the # of com ports on board (4, 8, 16)
        echo       [reserved] parameters are not currently used but must have values
        echo. 
        echo NOTE: com3 must be first com port on board!
        echo. 

:end
        endlocal
