@ECHO OFF
echo MAKEALL.BAT results >makeall.log

cd ANGLE
echo *** ANGLE *** >>..\makeall.log
echo *** ANGLE ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd BOB
echo *** BOB *** >>..\makeall.log
echo *** BOB ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd BNDBUF
echo *** BNDBUF *** >>..\makeall.log
echo *** BNDBUF ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd BPOINTER
echo *** BPOINTER *** >>..\makeall.log
echo *** BPOINTER ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd CHECK_SD
echo *** CHECK_SD *** >>..\makeall.log
echo *** CHECK_SD ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd CMNDLG
echo *** CMNDLG *** >>..\makeall.log
echo *** CMNDLG ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd CONSOLEC
echo *** CONSOLEC *** >>..\makeall.log
echo *** CONSOLEC ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd DRIVES
echo *** DRIVES *** >>..\makeall.log
echo *** DRIVES ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd EVENT
echo *** EVENT *** >>..\makeall.log
echo *** EVENT ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd GETDEV
echo *** GETDEV *** >>..\makeall.log
echo *** GETDEV ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd GETSYS
echo *** GETSYS *** >>..\makeall.log
echo *** GETSYS ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd HOOKS
echo *** HOOKS *** >>..\makeall.log
echo *** HOOKS ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd INHERIT
echo *** INHERIT *** >>..\makeall.log
echo *** INHERIT ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd LOCALE
echo *** LOCALE *** >>..\makeall.log
echo *** LOCALE ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd LOGGING
echo *** LOGGING *** >>..\makeall.log
echo *** LOGGING ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd MDI_
echo *** MDI_ *** >>..\makeall.log
echo *** MDI_ ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd MSGTABLE
echo *** MSGTABLE *** >>..\makeall.log
echo *** MSGTABLE ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd SCRNSAVE
echo *** SCRNSAVE *** >>..\makeall.log
echo *** SCRNSAVE ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd MINREC
echo *** MINREC *** >>..\makeall.log
echo *** MINREC ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd NTSD
echo *** NTSD *** >>..\makeall.log
echo *** NTSD ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd OTHRPROC
echo *** OTHRPROC *** >>..\makeall.log
echo *** OTHRPROC ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd PATHS
echo *** PATHS *** >>..\makeall.log
echo *** PATHS ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd PLATFORM
echo *** PLATFORM *** >>..\makeall.log
echo *** PLATFORM ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd POLYDRAW
echo *** POLYDRAW *** >>..\makeall.log
echo *** POLYDRAW ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd PROCESS
echo *** PROCESS *** >>..\makeall.log
echo *** PROCESS ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd READWRIT
echo *** READWRIT *** >>..\makeall.log
echo *** READWRIT ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd REGIONS
echo *** REGIONS *** >>..\makeall.log
echo *** REGIONS ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd RESDLL
echo *** RESDLL *** >>..\makeall.log
echo *** RESDLL ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd SD_FLPPY
echo *** SD_FLPPY *** >>..\makeall.log
echo *** SD_FLPPY ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd SEMAPHOR
echo *** SEMAPHOR *** >>..\makeall.log
echo *** SEMAPHOR ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd SETINFO
echo *** SETINFO *** >>..\makeall.log
echo *** SETINFO ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd SHAREMEM
echo *** SHAREMEM *** >>..\makeall.log
echo *** SHAREMEM ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd SIMPLDLL
echo *** SIMPLDLL *** >>..\makeall.log
echo *** SIMPLDLL ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd STARTP
echo *** STARTP *** >>..\makeall.log
echo *** STARTP ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd STREBLT
echo *** STREBLT *** >>..\makeall.log
echo *** STREBLT ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd SUBCLASS
echo *** SUBCLASS *** >>..\makeall.log
echo *** SUBCLASS ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd TERMPROC
echo *** TERMPROC *** >>..\makeall.log
echo *** TERMPROC ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd THREADS
echo *** THREADS *** >>..\makeall.log
echo *** THREADS ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd TIMERS
echo *** TIMERS *** >>..\makeall.log
echo *** TIMERS ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd TLS
echo *** TLS *** >>..\makeall.log
echo *** TLS ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd VIRTMEM
echo *** VIRTMEM *** >>..\makeall.log
echo *** VIRTMEM ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd WALK
echo *** WALK *** >>..\makeall.log
echo *** WALK ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd WINNET
echo *** WINNET *** >>..\makeall.log
echo *** WINNET ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd MASKBLT
echo *** MASKBLT *** >>..\makeall.log
echo *** MASKBLT ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd GUIGREP
echo *** GUIGREP *** >>..\makeall.log
echo *** GUIGREP ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd UNIPUT
echo *** UNIPUT *** >>..\makeall.log
echo *** UNIPUT ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd WORLD
echo *** WORLD *** >>..\makeall.log
echo *** WORLD ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd cliptext
echo *** cliptext *** >>..\makeall.log
echo *** cliptext ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd cursor
echo *** cursor *** >>..\makeall.log
echo *** cursor ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd lowpass
echo *** lowpass *** >>..\makeall.log
echo *** lowpass ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd menu
echo *** menu *** >>..\makeall.log
echo *** menu ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd output
echo *** output *** >>..\makeall.log
echo *** output ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd owncombo
echo *** owncombo *** >>..\makeall.log
echo *** owncombo ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd reverse
echo *** reverse *** >>..\makeall.log
echo *** reverse ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd cpl
echo *** cpl *** >>..\makeall.log
echo *** cpl ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

ECHO MAKEALL.BAT completed.
