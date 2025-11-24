OBJS = globals.obj netwatch.obj bitmap.obj net.obj utils.obj datetime.obj

# Nmake macros for building Windows 32-Bit apps

!include <ntwin32.mak>

all: netwatch.exe

# Update the resource if necessary

netwatch.res: netwatch.rc netwatch.h
    rc -d WIN32 -r -fo netwatch.res netwatch.rc

netwatch.rbj: netwatch.res
    cvtres -$(CPU) netwatch.res -o netwatch.rbj

# Update the object file if necessary

globals.obj: globals.c netwatch.h
    $(cc) $(cflags) $(cvars) $(cdebug) globals.c

netwatch.obj: netwatch.c netwatch.h
    $(cc) $(cflags) $(cvars) $(cdebug) netwatch.c

utils.obj: utils.c netwatch.h
    $(cc) $(cflags) $(cvars) $(cdebug) utils.c

bitmap.obj: bitmap.c netwatch.h
    $(cc) $(cflags) $(cvars) $(cdebug) bitmap.c

net.obj: net.c netwatch.h
    $(cc) $(cflags) $(cvars) $(cdebug) net.c

datetime.obj: datetime.c
    $(cc) $(cflags) $(cvars) $(cdebug) datetime.c

# Update the executable file if necessary, and if so, add the resource back in.

NetWatch.exe: $(OBJS) netwatch.rbj
    $(link) $(linkdebug) $(guiflags) /OUT:$*.exe $(OBJS) \
        netwatch.rbj netapi32.lib advapi32.lib shell32.lib $(guilibs) \
        /MAP:$*.map

clean:
    if exist *.obj del *.obj > nul
    if exist *.res del *.res > nul
    if exist *.exe del *.exe > nul
    if exist *.map del *.map > nul
    if exist *.sym del *.sym > nul
    if exist *.rbj del *.rbj > nul
    if exist *.sbr del *.sbr > nul
    if exist *.bsc del *.bsc > nul
