@ECHO OFF
cd SIMPLE
echo *** SIMPLE *** > ..\makeall.log
echo *** SIMPLE ***
nmake -a 1>>..\makeall.log 2>&1
cd ..

cd  PLAYSND
echo *** PLAYSND *** >> ..\makeall.log
echo *** PLAYSND ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd MYPAL
echo *** MYPAL *** >> ..\makeall.log
echo *** MYPAL ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd GENERIC
echo *** GENERIC *** >> ..\makeall.log
echo *** GENERIC ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd MAPI
echo *** MAPI *** >> ..\makeall.log
echo *** MAPI ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd GRIDFONT
echo *** GRIDFONT *** >> ..\makeall.log
echo *** GRIDFONT ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd LARGEINT
echo *** LARGEINT *** >> ..\makeall.log
echo *** LARGEINT ***
nmake -a 1>>..\makeall.log 2>&1
cd SAMPLE
echo *** SAMPLE *** >> ..\..\makeall.log
echo *** SAMPLE ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..\..

cd GDIDEMO
echo *** GDIDEMO *** >> ..\makeall.log
echo *** GDIDEMO ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd sdktools

cd PORTTOOL
echo *** PORTTOOL *** >> ..\..\makeall.log
echo *** PORTTOOL ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd walker
echo *** WALKER *** >> ..\..\makeall.log
echo *** WALKER ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd DDESPY 
echo *** DDESPY *** >> ..\..\makeall.log
echo *** DDESPY ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd ANIEDIT
echo *** ANIEDIT *** >> ..\..\makeall.log
echo *** ANIEDIT ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd DLGEDIT
echo *** DLGEDIT *** >> ..\..\makeall.log
echo *** DLGEDIT ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd FONTEDIT
echo *** FONTEDIT *** >> ..\..\makeall.log
echo *** FONTEDIT ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd IMAGEDIT
echo *** IMAGEDIT *** >> ..\..\makeall.log
echo *** IMAGEDIT ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd MC
echo *** MC *** >> ..\..\makeall.log
echo *** MC ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd NETWATCH 
echo *** NETWATCH *** >> ..\..\makeall.log
echo *** NETWATCH ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd PERFMON 
echo *** PERFMON *** >> ..\..\makeall.log
echo *** PERFMON ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd UCONVERT 
echo *** UCONVERT *** >> ..\..\makeall.log
echo *** UCONVERT ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd REMOTE
echo *** REMOTE *** >> ..\..\makeall.log
echo *** REMOTE ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd PVIEWER
echo *** PVIEWER *** >> ..\..\makeall.log
echo *** PVIEWER ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd WINAT 
echo *** WINAT *** >> ..\..\makeall.log
echo *** WINAT ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd WINDIFF 
echo *** WINDIFF *** >> ..\..\makeall.log
echo *** WINDIFF ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd ZOOMIN 
echo *** ZOOMIN *** >> ..\..\makeall.log
echo *** ZOOMIN ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..

cd SPY\DLL
echo *** SPY\DLL *** >> ..\..\..\makeall.log
echo *** SPY\DLL
nmake -a 1>>..\..\..\makeall.log 2>&1
cd ..\APP
echo *** SPY\APP *** >> ..\..\..\makeall.log
echo *** SPY\APP ***
nmake -a 1>>..\..\..\makeall.log 2>&1
cd ..\..

cd RSHELL\CLIENT
echo *** RSHELL\CLIENT *** >> ..\..\..\makeall.log
echo *** RSHELL\CLIENT ***
nmake -a 1>>..\..\..\makeall.log 2>&1
cd ..\SERVER
echo *** RSHELL\SERVER *** >> ..\..\..\makeall.log
echo *** RSHELL\SERVER ***
nmake -a 1>>..\..\..\makeall.log 2>&1
cd ..\..

cd IMAGE\IMAGEHLP
echo *** IMAGE\IMAGEHLP *** >> ..\..\..\makeall.log
echo *** IMAGE\IMAGEHLP ***
nmake -a 1>>..\..\..\makeall.log 2>&1
cd ..\DRWATSON
echo *** IMAGE\DRWATSON *** >> ..\..\..\makeall.log
echo *** IMAGE\DRWATSON ***
nmake -a 1>>..\..\..\makeall.log 2>&1
cd ..\SYMEDIT
echo *** IMAGE\SYMEDIT *** >> ..\..\..\makeall.log
echo *** IMAGE\SYMEDIT ***
nmake -a 1>>..\..\..\makeall.log 2>&1
cd ..\..

cd ..

cd MAZELORD
echo *** MAZELORD *** >> ..\makeall.log
echo *** MAZELORD ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd FLOPPY
echo *** FLOPPY *** >> ..\makeall.log
echo *** FLOPPY ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd INPUT
echo *** INPUT *** >> ..\makeall.log
echo *** INPUT ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd REGMPAD
echo *** REGMPAD *** >> ..\makeall.log
echo *** REGMPAD ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd MLTITHRD
echo *** MLTITHRD *** >> ..\makeall.log
echo *** MLTITHRD ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd WDBGEXTS
echo *** WDBGEXTS *** >> ..\makeall.log
echo *** WDBGEXTS ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd MULTIPAD
echo *** MULTIPAD *** >> ..\makeall.log
echo *** MULTIPAD ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd CDTEST
echo *** CDTEST *** >> ..\makeall.log
echo *** CDTEST ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd CONGUI
echo *** CONGUI *** >> ..\makeall.log
echo *** CONGUI ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd FONTVIEW
echo *** FONTVIEW *** >> ..\makeall.log
echo *** FONTVIEW ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd SHOWDIB
echo *** SHOWDIB *** >> ..\makeall.log
echo *** SHOWDIB ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd TAKEOWN
echo *** TAKEOWN *** >> ..\makeall.log
echo *** TAKEOWN ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd MEMORY
echo *** MEMORY *** >> ..\makeall.log
echo *** MEMORY ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd SELECT
echo *** SELECT *** >> ..\makeall.log
echo *** SELECT ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd DDEML\CLOCK
echo *** DDEML\CLOCK *** >> ..\..\makeall.log
echo *** DDEML\CLOCK ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..\CLIENT
echo *** DDEML\CLIENT *** >> ..\..\makeall.log
echo *** DDEML\CLIENT ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..\DDEMO
echo *** DDEML\DDEMO *** >> ..\..\makeall.log
echo *** DDEML\DDEMO ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..\DDEPROG
echo *** DDEML\DDEPROG *** >> ..\..\makeall.log
echo *** DDEML\DDEPROG ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..\SERVER
echo *** DDEML\SERVER *** >> ..\..\makeall.log
echo *** DDEML\SERVER ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..\DDEINST
echo *** DDEML\DDEINST *** >> ..\..\makeall.log
echo *** DDEML\DDEINST ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..\..

cd OLE\CLIDEMO
echo *** OLE\CLIDEMO *** >> ..\..\makeall.log
echo *** OLE\CLIDEMO ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..\SRVRDEMO
echo *** OLE\SRVRDEMO *** >> ..\..\makeall.log
echo *** OLE\SRVRDEMO ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..\..

cd  TTFONTS
echo *** TTFONTS *** >> ..\makeall.log
echo *** TTFONTS ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  PLGBLT
echo *** PLGBLT *** >> ..\makeall.log
echo *** PLGBLT ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  WXFORM
echo *** WXFORM *** >> ..\makeall.log
echo *** WXFORM ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  NAMEPIPE\NPSERVER
echo *** NAMEPIPE\NPSERVER *** >> ..\..\makeall.log
echo *** NAMEPIPE\NPSERVER ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd  ..\NPCLIENT
echo *** NAMEPIPE\NPCLIENT *** >> ..\..\makeall.log
echo *** NAMEPIPE\NPCLIENT ***
nmake -a 1>>..\..\makeall.log 2>&1 
cd ..\..

cd  SIDCLN
echo *** SIDCLN *** >> ..\makeall.log
echo *** SIDCLN ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  SPINCUBE
echo *** SPINCUBE *** >> ..\makeall.log
echo *** SPINCUBE ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  DYNDLG
echo *** DYNDLG *** >> ..\makeall.log
echo *** DYNDLG ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  SERVICE
echo *** SERVICE *** >> ..\makeall.log
echo *** SERVICE ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  WSOCK 
echo *** WSOCK *** >> ..\makeall.log
echo *** WSOCK ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  MFEDIT
echo *** MFEDIT *** >> ..\makeall.log
echo *** MFEDIT ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  CONSOLE
echo *** CONSOLE *** >> ..\makeall.log
echo *** CONSOLE ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  COMM
echo *** COMM *** >> ..\makeall.log
echo *** COMM ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  DEB
echo *** DEB *** >> ..\makeall.log
echo *** DEB ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  FILER
echo *** FILER *** >> ..\makeall.log
echo *** FILER ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  MANDEL
echo *** MANDEL *** >> ..\makeall.log
echo *** MANDEL ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  PRINTER
echo *** PRINTER *** >> ..\makeall.log
echo *** PRINTER ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  PDC
echo *** PDC *** >> ..\makeall.log
echo *** PDC ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  MCITEST
echo *** MCITEST *** >> ..\makeall.log
echo *** MCITEST ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  MIDIMON
echo *** MIDIMON *** >> ..\makeall.log
echo *** MIDIMON ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd  REGISTRY
echo *** REGISTRY *** >> ..\makeall.log
echo *** REGISTRY ***
nmake -a 1>>..\makeall.log 2>&1 
cd ..

cd SNMP\SNMPUTIL 
echo *** SNMP\SNMPUTIL *** >> ..\..\makeall.log
echo *** SNMP\SNMPUTIL ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..\TESTDLL 
echo *** SNMP\TESTDLL *** >> ..\..\makeall.log
echo *** SNMP\TESTDLL ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..\..

cd NWLINK\TESTLIB
echo *** NWLINK\TESTLIB *** >> ..\..\makeall.log
echo *** NWLINK\TESTLIB ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..

cd CONNECT
echo *** NWLINK\CONNECT *** >> ..\..\makeall.log
echo *** NWLINK\CONNECT ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..

cd DGRECV
echo *** NWLINK\DGRECV *** >> ..\..\makeall.log
echo *** NWLINK\DGRECV ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..

cd DGSEND
echo *** NWLINK\DGSEND *** >> ..\..\makeall.log
echo *** NWLINK\DGSEND ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..

cd PING
echo *** NWLINK\PING *** >> ..\..\makeall.log
echo *** NWLINK\PING ***
nmake -a 1>>..\..\makeall.log 2>&1
cd ..

cd LISTEN\NONBLOCK
echo *** NWLINK\LISTEN\NONBLOCK *** >> ..\..\..\makeall.log
echo *** NWLINK\LISTEN\NONBLOCK ***
nmake -a 1>>..\..\..\makeall.log 2>&1
cd ..

cd BLOCK
echo *** NWLINK\LISTEN\BLOCK *** >> ..\..\..\makeall.log
echo *** NWLINK\LISTEN\BLOCK ***
nmake -a 1>>..\..\..\makeall.log 2>&1
cd ..\..\..


