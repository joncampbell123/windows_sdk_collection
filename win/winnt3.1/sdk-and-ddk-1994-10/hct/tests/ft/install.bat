@echo off

if "%1"=="" goto usage
if "%2"=="" goto usage

setlocal

set _cpu=%1
set _tgt=%2

md %_tgt%\ft

copy ..\..\bin\mst\%_cpu%\test*.*			%_tgt%\ft
copy ..\..\bin\mst\%_cpu%\lineedit.dll			%_tgt%\ft
copy ..\..\bin\mst\include\mstest.inc			%_tgt%\ft
copy ..\..\bin\%_cpu%\ntsd.exe				%_tgt%\ft

copy .\*.mst						%_tgt%\ft
copy .\*.inc						%_tgt%\ft
copy .\*.dat						%_tgt%\ft
copy .\*.cmd						%_tgt%\ft
copy .\*.ini						%_tgt%\ft
copy .\*.in						%_tgt%\ft
copy .\*.0						%_tgt%\ft
copy .\*.1						%_tgt%\ft
copy .\*.2						%_tgt%\ft
copy .\*.3						%_tgt%\ft
copy .\%_cpu%\*.*					%_tgt%\ft

goto end

:usage

echo	install [cpu] [drive]
echo.
echo	where
echo.
echo	cpu is Windows NT Platform : x86, MIPS.  Defaults to x86
echo 	drive is hard disk path to install \ft onto

:end

endlocal
