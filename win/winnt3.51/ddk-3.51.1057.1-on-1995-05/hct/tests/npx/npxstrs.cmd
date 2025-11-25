@echo off

@rem
@rem Windows/NT HCT
@rem
@rem npxstrs.cmd - runs coprocessor stress
@rem

set DEBUG=0

if "%1"=="-DEBUG" set DEBUG=1

        echo STARTING Coprocessor (Stress) HCT 
        echo STARTING Coprocessor (Stress) HCT >> npxstrs.log

if %DEBUG%==1 goto DEBUG
        floater 500 17 > tmp1 > tmp2
        goto DONEDEBUG
:DEBUG
        windbg -g floater 500 17 > tmp1 > tmp2
:DONEDEBUG
        if errorlevel 1 goto failure
        echo PASS - Coprocessor (Stress) HCT
        echo PASS - Coprocessor (Stress) HCT >> npxstrs.log

        goto end

:failure
        echo FAIL - Coprocessor (Stress) HCT
        echo FAIL - Coprocessor (Stress) HCT >> npxstrs.log

:end
