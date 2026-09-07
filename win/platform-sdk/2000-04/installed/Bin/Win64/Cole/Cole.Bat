@Echo Off
if "%1"=="-help" goto help

if "%2"=="" goto onearg
sfa-html.pl %1 %2
pushd %2
If Exist ColeReport.Htm Start ColeReport.Htm
popd
goto end

:onearg
Set _Arg1=%1
If "%_Arg1%"=="" Set _Arg1=%ColeLogFileName%
If "%_Arg1%"=="" Set _Arg1=.\ColeLog.Txt
sfa-html.pl %_Arg1%
If Exist ColeReport.Htm Start ColeReport.Htm
goto end

:help
echo Usage:
echo %0 {log file name} {html code directory}
goto end

:end
