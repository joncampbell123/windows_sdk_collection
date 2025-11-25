@echo off
:loop
if "%1"=="" goto missing
if "%1"=="/m" goto multivol
if "%1"=="/M" goto multivol
if "%1"=="-m" goto multivol
if "%1"=="-M" goto multivol
if "%1"=="-i" goto index
if "%1"=="-I" goto index
if "%1"=="/i" goto index
if "%1"=="/I" goto index
goto make
:multivol
	shift
	if "%1"=="" goto missing
	nmk -C NAME=%1 IND_ONLY=Y MULTIVOL=Y %2 %3 %4 %5 %6 %7 %8 %9
	goto end

:index
	shift
	if "%1"=="" goto missing
	nmk -C NAME=%1 IND_ONLY=Y %2 %3 %4 %5 %6 %7 %8 %9
	goto end
:make
	nmk -C NAME=%1 %2 %3 %4 %5 %6 %7 %8 %9
goto end
:missing
echo Missing project name: "hlpindex [project]"
:end
