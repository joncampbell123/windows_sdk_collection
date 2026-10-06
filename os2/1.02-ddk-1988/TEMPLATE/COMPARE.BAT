echo off
REM
REM compare - compare template binaries with those produced after a DDDK build
REM
REM This batch files is to be run after all DDDK components have been built.
REM The purpose of the batch file is to verify the created build environment.
REM
echo compare binaries from DDDK's build
comp disk01.sys ..\src\dev\diskdd\disk01.sys
comp disk01d.sys ..\src\dev\diskdd\disk01d.sys
comp disktest.com ..\src\dev\diskdd\disktest.com
comp comtest.exe ..\src\dev\comsam\comtest\comtest.exe
comp comsam1.sys ..\src\dev\comsam\level1\comsam1.sys
comp comsam1d.sys ..\src\dev\comsam\level1\comsam1d.sys
comp comsam2.sys ..\src\dev\comsam\level2\comsam2.sys
comp comsam2d.sys ..\src\dev\comsam\level2\comsam2d.sys
