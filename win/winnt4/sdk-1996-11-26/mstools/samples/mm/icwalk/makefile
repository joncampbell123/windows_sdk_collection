# Nmake macros for building Windows 32-Bit apps

!include <win32.mak>

!if "$(CPU)" == "i386" # .syms are useful for Win95
SYM = icwalk.sym
!endif

all: icwalk.exe $(SYM)

OBJS=icwalk.obj

OTHERCLOPTS=-DWIN32_LEAN_AND_MEAN -nologo -I. -I..\include
OTHERRCOPTS=-DWIN32_LEAN_AND_MEAN -I. -I..\include


# Update the resource if necessary

icwalk.res: icwalk.rc icwalk.h icwalk.rcv icwalk.ico
    $(rc) -r -DWIN32 $(OTHERRCOPTS) icwalk.rc


# Update the object file if necessary

icwalk.obj: icwalk.c icwalk.h
    $(cc) $(cdebug) $(cflags) $(cvars) $(OTHERCLOPTS) icwalk.c


# Update the executable file if necessary, and if so, add the resource back in.

icwalk.exe icwalk.map:  $(OBJS) icwalk.res
    $(link) $(linkdebug) $(guilflags) -out:icwalk.exe $(OBJS) icwalk.res $(guilibs) \
	 winmm.lib vfw32.lib -map:$*.map
    
icwalk.sym:	$*.map
	mapsym $*.map

clean:
    @if exist icwalk.exe del icwalk.exe
    @if exist *.obj del *.obj
    @if exist *.map del *.map
    @if exist *.sym del *.sym
    @if exist *.res del *.res
    @if exist *.pdb del *.pdb
    @if exist *.exp del *.exp
    @if exist *.lib del *.lib

