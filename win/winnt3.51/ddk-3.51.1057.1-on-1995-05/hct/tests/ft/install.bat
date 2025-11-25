@echo off

if "%1"=="" goto usage
if "%2"=="" goto usage

setlocal

set _cpu=%1
set _tgt=%2

md %_tgt%\ft

copy ..\..\bin\%_cpu%\ntsd.exe				%_tgt%\ft
copy ..\..\bin\%_cpu%\t3bas32.dll %_tgt%\ft
copy ..\..\bin\%_cpu%\t3ctrl32.dll %_tgt%\ft
copy ..\..\bin\%_cpu%\t3dde32.dll %_tgt%\ft
copy ..\..\bin\%_cpu%\t3dlgs32.dll %_tgt%\ft
copy ..\..\bin\%_cpu%\t3host32.dll %_tgt%\ft
copy ..\..\bin\%_cpu%\t3rcrd32.dll %_tgt%\ft
copy ..\..\bin\%_cpu%\t3run32.dll %_tgt%\ft
copy ..\..\bin\%_cpu%\t3scrn32.dll %_tgt%\ft
copy ..\..\bin\%_cpu%\t3stat32.dll %_tgt%\ft
copy ..\..\bin\%_cpu%\t3trap32.dll %_tgt%\ft
copy ..\..\bin\%_cpu%\t3ui32.dll %_tgt%\ft
copy ..\..\bin\%_cpu%\ctl3d.dll %_tgt%\ft
copy ..\..\bin\%_cpu%\mscpydis.dll %_tgt%\ft

copy ..\..\bin\%_cpu%\mtrun.exe %_tgt%\ft

copy .\*.pcd						%_tgt%\ft
copy .\*.cmd						%_tgt%\ft
copy .\*.ini						%_tgt%\ft
copy .\*.0						%_tgt%\ft
copy .\*.1						%_tgt%\ft
copy .\*.1*      %_tgt%\ft
copy .\*.2						%_tgt%\ft
copy .\*.3						%_tgt%\ft
copy .\*.lan    %_tgt%\ft
copy .\*.run    %_tgt%\ft
copy .\*.reg    %_tgt%\ft
copy .\readme*  %_tgt%\ft
copy .\%_cpu%\*.*					%_tgt%\ft

goto end

:usage

echo	install [cpu] [drive]
echo.
echo	where
echo.
echo	cpu is Windows NT Platform : x86, MIPS, Alpha.  Defaults to x86
echo 	drive is hard disk path to install \ft onto

:end

endlocal

