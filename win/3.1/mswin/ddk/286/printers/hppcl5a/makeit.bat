echo off
echo Making HPPCL5A
if not %1.==. set LANG=%1
cd src
echo Making .OBJ files
nmake /f pclsrc
cd ..\rc
echo Making .RES file
nmake /f pclrc
cd ..
echo Linking
nmake /f hppcl5a
set LANG=
