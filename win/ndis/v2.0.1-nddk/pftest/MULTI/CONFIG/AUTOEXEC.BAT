prompt [$p$g]
call c:\number
break on
set term=ibmans
PATH=c:\;C:\LANMAN.DOS\NETPROG;c:\bin;c:\;c:\jetbeui;c:\pf;
NETBIND
NET START RDR
net logon Wksta5
net use t: \\pinsrv\ddr
t:
cd multi\msbench
pause
dos
