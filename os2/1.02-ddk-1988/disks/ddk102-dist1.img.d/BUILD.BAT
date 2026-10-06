echo off

REM build the DDDK's contents
REM assume current directory is the starting point of the build

cd src\dev\comsam\comtest
make comtest.mak
cd ..\level1
make comsam1.mak
cd ..\level2
make comsam2.mak
cd ..\..\diskdd
make disk01.mak
make disktest.mak
echo DDDK build done
