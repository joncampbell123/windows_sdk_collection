OBJS = netwatch.obj net.obj utils.obj

# Nmake macros for building Windows 32-Bit apps

TARGETOS=WINNT
APPVER=4.0

!include <win32.mak>

!IFNDEF NODEBUG
cflags = $(cflags) -D DEBUG
rcflags = $(rcflags) -d DEBUG
!ENDIF

all: netwatch.exe

# Update the resource if necessary

netwatch.res: netwatch.rc netwatch.h
    rc $(rcflags) -d WIN32 -r -fo netwatch.res netwatch.rc

# Update the object file if necessary

netwatch.obj: netwatch.c netwatch.h
    $(cc) $(cflags) $(cvars) $(cdebug) netwatch.c

utils.obj: utils.c netwatch.h
    $(cc) $(cflags) $(cvars) $(cdebug) utils.c

net.obj: net.c netwatch.h
    $(cc) $(cflags) $(cvars) $(cdebug) net.c

# Update the executable file if necessary, and if so, add the resource back in.

NetWatch.exe: $(OBJS) netwatch.res
    $(link) $(linkdebug) $(guilflags) /OUT:$*.exe $(OBJS) \
        netwatch.res netapi32.lib advapi32.lib shell32.lib \
        mpr.lib comctl32.lib $(guilibs)

clean:
    if exist *.obj del *.obj > nul
    if exist *.res del *.res > nul
    if exist *.exe del *.exe > nul
    if exist *.map del *.map > nul
    if exist *.sym del *.sym > nul
    if exist *.res del *.res > nul
    if exist *.sbr del *.sbr > nul
    if exist *.bsc del *.bsc > nul
