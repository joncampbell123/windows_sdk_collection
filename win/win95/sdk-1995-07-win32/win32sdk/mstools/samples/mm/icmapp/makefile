# Nmake macros for building Windows 32-Bit apps

!include <win32.mak>

!if "$(CPU)" == "i386" # .syms are useful for Win95
SYM = icmapp.sym
!endif

all: icmapp.exe $(SYM)

OBJS=icmapp.obj pro.obj dib.obj

OTHERCLOPTS=-DWIN32_LEAN_AND_MEAN -nologo -I. -I..\include
OTHERRCOPTS=-DWIN32_LEAN_AND_MEAN -I. -I..\include


# Update the resource if necessary

icmapp.res: icmapp.rc icmapp.h pro.h icmapp.rcv icmapp.ico
    $(rc) -r -DWIN32 $(OTHERRCOPTS) icmapp.rc


# Update the object file if necessary

icmapp.obj: icmapp.c icmapp.h dib.h pro.h
    $(cc) $(cdebug) $(cflags) $(cvars) $(OTHERCLOPTS) icmapp.c

dib.obj: dib.c dib.h
    $(cc) $(cdebug) $(cflags) $(cvars) $(OTHERCLOPTS) dib.c

pro.obj: pro.c pro.h
    $(cc) $(cdebug) $(cflags) $(cvars) $(OTHERCLOPTS) pro.c


# Update the executable file if necessary, and if so, add the resource back in.

icmapp.exe icmapp.map:  $(OBJS) icmapp.res
    $(link) $(linkdebug) $(guilflags) -out:icmapp.exe $(OBJS) icmapp.res $(guilibs) \
	 winmm.lib vfw32.lib -map:$*.map
    
icmapp.sym:	$*.map
	mapsym $*.map

clean:
    @if exist icmapp.exe del icmapp.exe
    @if exist *.obj del *.obj
    @if exist *.map del *.map
    @if exist *.sym del *.sym
    @if exist *.res del *.res
    @if exist *.pdb del *.pdb
    @if exist *.exp del *.exp
    @if exist *.lib del *.lib

