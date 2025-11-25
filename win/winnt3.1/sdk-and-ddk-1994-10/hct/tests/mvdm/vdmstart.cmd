@echo off

@echo.
@rem
@rem Windows NT HCT
@rem
@rem vdmstart.cmd - test ability to start a VDM
@rem

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
        echo FAIL > %1\hct\logs\vdmstart.log
        
        %1
        cd \hct
        command /c dir > %1\hct\logs\vdmstart.log
        
        if errorlevel 1 goto ERROR
        
        echo PASS >> %1\hct\logs\vdmstart.log
        echo.
        echo Testing complete
        
        goto EXIT

:ERROR
        echo Error occured starting a VDM...
        echo FAILED >> %1\hct\logs\vdmstart.log

:EXIT
