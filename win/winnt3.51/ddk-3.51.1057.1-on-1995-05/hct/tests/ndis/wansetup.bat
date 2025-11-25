md %1

if errorlevel 1 goto Err

copy .\wancli.bat %1
copy .\wansrv.bat %1
copy .\mw*.bat %1
copy hapi.ini %1

md %1\scripts
md %1\scripts\wan

copy .\scripts\wan\* %1\scripts\wan\

goto end
:Err
@echo    Error - cannot make the directory specified

:end
