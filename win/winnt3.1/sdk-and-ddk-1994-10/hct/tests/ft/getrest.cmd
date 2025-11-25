@rem
@rem getrest server build#
@rem

@if "%1"=="" goto usage

copy \\%1\freebins.%2\nt\mstools\test*
copy \\%1\freebins.%2\nt\mstools\lineedit.dll

if exist \\%1\freebins.%2\nt\mstools\imagehlp.dll copy \\%1\freebins.%2\nt\mstools\imagehlp.dll
if exist \\%1\freebins.%2\nt\system32\imagehlp.dll copy \\%1\freebins.%2\nt\system32\imagehlp.dll

copy \\%1\freebins.%2\nt\idw\tee.exe
copy \\%1\freebins.%2\nt\idw\shutdown.exe
copy \\%1\freebins.%2\nt\idw\sleep.exe
copy \\%1\freebins.%2\nt\idw\simbad.exe

copy \\%1\freebins.%2\nt\system32\ntsd.exe

copy \\%1\freepub.%2\sdk\inc\mstest.inc

copy \\%1\chkbins.%2\nt\system32\drivers\simbad.sys %systemroot%\system32\drivers

@goto end

:usage
@echo.
@echo getrest ^<server^> ^<build#^>

:end
