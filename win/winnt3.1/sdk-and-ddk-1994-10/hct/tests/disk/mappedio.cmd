@echo off

@rem
@rem Windows/NT HCT
@rem
@rem mappedio.cmd - cmd file to run mapped i/o disk test
@rem

        if "%1"=="" goto usage
        if "%2"=="" goto usage
        if "%3"=="" goto usage

        if "%4"=="" goto usage

        rats maphct%4.rat
        copy maphct%4.log \hct\logs\mapiohct.log
        del %4:\mapio.dat

rem  later, replace rats with exe, start on up to 6 partitions at once

        goto end

:usage
        echo.
        echo Usage: mappedio [hct drive] [reserved] [reserved] [partition]
        echo.
        echo where [hct drive] is the drive that the HCTs live on
        echo       [reserved] parameters are not currently used but must have values
        echo       [partition] is the partition to stress and
        echo.

:end
