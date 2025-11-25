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

        setlocal

        if "%2"=="STRESS" goto stress

        echo STARTING Coprocessor HCT
        echo STARTING Coprocessor HCT > npx.log
        floater 20 10 > tmp1 > tmp2
        if errorlevel 1 goto failure
        echo PASS - Coprocessor HCT
        echo PASS - Coprocessor HCT >> npx.log

        copy npx.log \hct\logs
        goto end

:stress
        for %%i in (1 2 3 4 5) do call npxstrs.cmd

        copy npxstrs.log \hct\logs
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
        endlocal

