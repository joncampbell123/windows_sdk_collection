
@ECHO OFF
echo MAKEALL.BAT results >makeall.log

cd KBD_ATTR
echo *** KBD_ATTR *** >>..\makeall.log
echo *** KBD_ATTR ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd MACADDR
echo *** MACADDR *** >>..\makeall.log
echo *** MACADDR ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd Q_DOSDEV
echo *** Q_DOSDEV *** >>..\makeall.log
echo *** Q_DOSDEV ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd SMPI
echo *** SMPI *** >>..\makeall.log
echo *** SMPI ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd SPTI
echo *** SPTI *** >>..\makeall.log
echo *** SPTI ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd WSHSMPLE
echo *** WSHSMPLE *** >>..\makeall.log
echo *** WSHSMPLE ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd INSTDRV
echo *** INSTDRV *** >>..\makeall.log
echo *** INSTDRV ***
nmake -a -f INSTDRV.MAK 1>>..\makeall.log 2>&1
cd ..

cd MAPMEM
echo *** MAPMEM *** >>..\makeall.log
echo *** MAPMEM ***
nmake -a -f MAPTEST.MAK 1>>..\makeall.log 2>&1
cd ..

cd MONO
echo *** MONO *** >>..\makeall.log
echo *** MONO ***
nmake -a -f MONOTEST.MAK 1>>..\makeall.log 2>&1
cd ..

cd PORTIO
echo *** PORTIO *** >>..\makeall.log
echo *** PORTIO ***
nmake -a -f GPTEST.MAK 1>>..\makeall.log 2>&1
cd ..

cd SIMPLE
echo *** SIMPLE *** >>..\makeall.log
echo *** SIMPLE ***
nmake -a -f GETHNDL.MAK 1>>..\makeall.log 2>&1
cd ..

ECHO MAKEALL.BAT completed.
