@rem @echo off

@if "%4"=="" goto usage

@echo CpStress Log > %4
@echo. >> %4
@echo %5 >> %4
@ver >> %4
@echo. >> %4

:loop
echo -------------------------- >> %4
echotime /WbbO-D-CYbbH:M:S >> %4

@rem Copy source file to dest_dir\dest_file
copy %1 %2\%3 >> %4

@rem Copy dest_dir\dest_file to the current dir
copy %2\%3 . >> %4

@rem Compare the source file to the local dest_file
comp %1 %3 < n >> %4

@rem The dos version of comp does not return errorlevel. We never goto error.
@if errorlevel 1 goto error

@rem Delete both copies of the y3dialin
del %2\%3 >> %4
del %3 >> %4

goto loop

:error
@echo. >> %4
@echo !!! CPSTRESS ERROR !!! >> %4
@echo. >> %4
@echo.
@echo !!! CPSTRESS ERROR !!!
@echo.
@goto end

:usage
@echo Usage  %0  src_file  dest_dir  dest_file  log_file  [log_title_word]

:end
