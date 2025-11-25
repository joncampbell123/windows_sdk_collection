# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

USE_UNICODE=0

!if $(USE_UNICODE)
UNICODE_DEFINES=-DUNICODE -D_UNICODE
conflags = $(lflags) -subsystem:console,$(APPVER) -entry:wmainCRTStartup
!else
UNICODE_DEFINES=
!endif

all: typeperf.exe

typeperf.obj: typeperf.c
    $(cc) $(cflags) $(UNICODE_DEFINES) $(cvars) typeperf.c

typeperf.res: typeperf.rc
    $(rc) $(rcflags) $(rcvars) typeperf.rc

typeperf.exe: typeperf.obj typeperf.res
    $(link) $(conflags) -out:typeperf.exe typeperf.obj typeperf.res $(conlibs) pdh.lib
