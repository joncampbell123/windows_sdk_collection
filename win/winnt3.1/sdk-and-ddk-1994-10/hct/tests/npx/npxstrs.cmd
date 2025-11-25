@echo off

@rem
@rem Windows/NT HCT
@rem
@rem npxstrs.cmd - runs coprocessor stress
@rem

        echo STARTING Coprocessor (Stress) HCT 
        echo STARTING Coprocessor (Stress) HCT >> npxstrs.log
        floater 500 17 > tmp1 > tmp2
        if errorlevel 1 goto failure
        echo PASS - Coprocessor (Stress) HCT
        echo PASS - Coprocessor (Stress) HCT >> npxstrs.log

        goto end

:failure
        echo FAIL - Coprocessor (Stress) HCT
        echo FAIL - Coprocessor (Stress) HCT >> npxstrs.log

:end

