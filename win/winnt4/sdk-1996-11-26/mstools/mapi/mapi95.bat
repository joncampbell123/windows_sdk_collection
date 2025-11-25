@echo off

rem Set up long-menu Exchange client
if not exist win95\nodebug\usa\wmsui32.dll goto nowmsui
echo Setting up long-menu Exchange client...
@echo on
copy win95\nodebug\usa\wmsui32.dll %windir%\system
pdkin32 -r c:\
@echo off
goto mergeinf

:nowmsui
echo WMSUI32.DLL not found. The long-menu Exchange client was not set up.

:mergeinf
echo Setting up sample MAPI service providers...
@echo on
mergeini -q -m .\pdk95.inf
@echo off

:done
