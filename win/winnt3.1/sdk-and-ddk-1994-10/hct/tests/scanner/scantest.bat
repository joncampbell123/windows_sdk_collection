@echo off
echo Cons32 Test >  scan.log
echo. >> scan.log
CONS32.EXE	 >> scan.log
echo. >> scan.log
echo Getcap Test >> scan.log
echo. >> scan.log
GETCAP.EXE	 >> scan.log
echo. >> scan.log
echo ioctl Test  >> Scan.log
echo. >> scan.log
IOCTL.EXE	 >> scan.log
rem LAMPON.EXE	>> scan.log
echo. >> scan.log
echo NTScanEx Test >> scan.log
echo. >> scan.log
NTSCANEX.EXE >> scan.log
rem READTEST.EXE >> scan.log
echo. >> scan.log
echo RWBuffer Test >> Scan.log
echo. >> scan.log
RWBUFFER.EXE >> scan.log
echo. >> scan.log
echo ScanPerf Test >> scan.log
echo. >> scan.log
SCANPERF.EXE >> scan.log
echo. >> scan.log
echo END OF TEST
echo END OF TEST >> scan.log
Echo. >> scan.log
