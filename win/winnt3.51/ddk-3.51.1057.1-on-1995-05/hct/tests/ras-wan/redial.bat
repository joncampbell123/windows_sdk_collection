@rem @echo off

@if "%2"=="" goto usage

@echo ReDial Log > %2
@echo. >> %2
@echo %3 >> %2
@ver >> %2
@echo. >> %2
@date < cr >> %2

:next
@echo. >> %2
time < cr >> %2
rasdial %1 administrator >> %2
rasdial >> %2
rasdial %1 /d >> %2
sleep 30
@rem type %windir%\system32\ras\device.log >> dev.log
@goto next

:usage
@echo.
@echo Usage  %0 ras_sever log_file [log_title_word]
@echo.
