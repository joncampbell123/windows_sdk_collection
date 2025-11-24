# Nmake macros for building Windows 32-Bit apps

TARGETOS=WINNT

!include <ntwin32.mak>

all: snmputil.exe

snmputil.obj: snmputil.c
    $(cc) $(cflags) $(cvars) $(cdebug) snmputil.c

snmputil.exe: snmputil.obj
    $(link) $(linkdebug) $(conflags) -out:snmputil.exe snmputil.obj $(conlibsdll) \
    advapi32.lib snmp.lib mgmtapi.lib wsock32.lib
