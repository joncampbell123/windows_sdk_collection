goto begin
REM ************************************************************************
REM MASTER.BAT - Control running of the MAC Conformance Test Tool scripts.
REM
REM     This batch file will run all of the scripts in the directories
REM     specified on the command line or it will run the scripts for all
REM     of the directories.
REM
REM     USAGE:  MASTER [REGS] [GDT] [subdirs ...]
REM ************************************************************************

:begin
    call cleanlog
    set all=
    set regs=
    set gdts=

REM If the first parms are "REGS" or "GDT" then set the register preservation 
REM and GDT testing flags on, skip the parm, and jump back to here.
:start
    if "%1"=="REGS" goto setregs
    if "%1"=="regs" goto setregs
    goto chkgdt

:setregs
    set regs=/r
    shift
    goto start

:chkgdt
    if "%1"=="GDT" goto setgdts
    if "%1"=="gdt" goto setgdts
    goto normal

:setgdts
    set gdts=/g
    shift
    goto start


REM If parm 1 is blank, then run all directories.  Otherwise treat parm 1
REM as a label.
:normal
    if "%1"=="" set all=yes
    if "%1"=="" goto all
    goto %1

:loop
    shift
    if "%1"=="" goto end
    goto %1

:all
:addmcadr
    set current=AddMulticastAddress
    cd addmcadr
    if exist addmcadr.log copy *.log *.old
    if exist addmcadr.log del addmcadr.log
    ..\..\mtdos %regs% %gdts% addmc001 addmcadr
    ..\..\mtdos %regs% %gdts% addmc002 addmcadr
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto close
    goto loop

:close
    set current=CloseAdapter
    cd close
    if exist close.log copy *.log *.old
    if exist close.log del close.log
    ..\..\mtdos %regs% %gdts% close001 close
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto delmcadr
    goto loop

:delmcadr
    set current=DeleteMulticastAddress
    cd delmcadr
    if exist delmcadr.log copy *.log *.old
    if exist delmcadr.log del delmcadr.log
    ..\..\mtdos %regs% %gdts% delmc001 delmcadr
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto interupt
    goto loop

:interupt
    set current=Interrupt
    cd interupt
    if exist interupt.log copy *.log *.old
    if exist interupt.log del interupt.log
    ..\..\mtdos %regs% %gdts% int001 interupt
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto mactable
    goto loop

:mactable
    set current=Check MAC Tables
    cd mactable
    if exist mactable.log copy *.log *.old
    if exist mactable.log del mactable.log
    ..\..\mtdos mactb001 mactable
    if exist abort.log goto abort
    ..\..\mtdos %regs% %gdts% mactb002 mactable
    if exist abort.log goto abort
    ..\..\mtdos %regs% %gdts% mactb003 mactable
    if exist abort.log goto abort
    ..\..\mtdos %regs% %gdts% mactb004 mactable
    if exist abort.log goto abort
    ..\..\mtdos %regs% %gdts% mactb005 mactable
    if exist abort.log goto abort
    ..\..\mtdos %regs% %gdts% mactb006 mactable
    if exist abort.log goto abort
    ..\..\mtdos %regs% %gdts% mactb007 mactable
    if exist abort.log goto abort
    ..\..\mtdos %regs% %gdts% mactb008 mactable
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto open
    goto loop

:open
    set current=OpenAdapter
    cd open
    if exist open.log copy *.log *.old
    if exist open.log del open.log
    ..\..\mtdos %regs% %gdts% open001 open
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto setpkfil
    goto loop

:setpkfil
    set current=SetPacketFilter
    cd setpkfil
    if exist setpkfil.log copy *.log *.old
    if exist setpkfil.log del setpkfil.log
    ..\..\mtdos %regs% %gdts% setpf001 setpkfil
    if exist abort.log goto abort
    ..\..\mtdos %regs% %gdts% setpf002 setpkfil
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto stnadr
    goto loop

:stnadr
    set current=SetStationAddress
    cd stnadr
    if exist stnadr.log copy *.log *.old
    if exist stnadr.log del stnadr.log
    ..\..\mtdos %regs% %gdts% stn001 stnadr
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto initdiag
    goto loop

:initdiag
    set current=InitiateDiagnostics
    cd initdiag
    if exist initdiag.log copy *.log *.old
    if exist initdiag.log del initdiag.log
    ..\..\mtdos %regs% %gdts% inidg001 initdiag
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto updstats
    goto loop

:updstats
    set current=UpdateStatistics
    cd updstats
    if exist updstats.log copy *.log *.old
    if exist updstats.log del updstats.log
    ..\..\mtdos %regs% %gdts% updst001 updstats
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto clrstats
    goto loop

:clrstats
    set current=ClearStatistics
    cd clrstats
    if exist clrstats.log copy *.log *.old
    if exist clrstats.log del clrstats.log
    ..\..\mtdos %regs% %gdts% clrst001 clrstats
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto reset
    goto loop

:reset
    set current=ResetMAC
    cd reset
    if exist reset.log copy *.log *.old
    if exist reset.log del reset.log
    ..\..\mtdos %regs% %gdts% reset001 reset
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto setlook
    goto loop

:setlook
    set current=SetLookahead
    cd setlook
    if exist setlook.log copy *.log *.old
    if exist setlook.log del setlook.log
    ..\..\mtdos %regs% %gdts% slook001 setlook
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto xmitchn
    goto loop

:xmitchn
    set current=TransmitChain
    cd xmitchn
    if exist xmitchn.log copy *.log *.old
    if exist xmitchn.log del xmitchn.log
    ..\..\mtdos %regs% %gdts% tran001 xmitchn
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto setfaddr
    goto loop

:setfaddr
    set current=SetFunctionalAddress
    cd setfaddr
    if exist setfaddr.log copy *.log *.old
    if exist setfaddr.log del setfaddr.log
    ..\..\mtdos %regs% %gdts% setfa001 setfaddr
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto rderrlog
    goto loop

:rderrlog
    set current=ReadErrorLog
    cd rderrlog
    if exist rderrlog.log copy *.log *.old
    if exist rderrlog.log del rderrlog.log
    ..\..\mtdos %regs% %gdts% rdlog001 rderrlog
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto indicate
    goto loop

:indicate
    set current=IndicationOnOff
    cd indicate
    if exist indicate.log copy *.log *.old
    if exist indicate.log del indicate.log
    ..\..\mtdos %regs% %gdts% indic001 indicate
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto handle0
    goto loop

:handle0
    set current=Handle0
    cd handle0
    if exist handle0.log copy *.log *.old
    if exist handle0.log del handle0.log
    ..\..\mtdos %regs% %gdts% handl001 handle0
    if exist abort.log goto abort
    cd..
    if "%all%"=="yes" goto end
    goto loop


:abort
    echo %current% aborted
    cd..

:end
    set all=
    set current=
    call result

