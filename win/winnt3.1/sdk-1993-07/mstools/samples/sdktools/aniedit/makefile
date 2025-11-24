!include <ntwin32.mak>

all: aniedit.exe

animsg.h animsg.rc msg00001.bin: animsg.mc
    mc -v animsg.mc

aniedit.rbj: animsg.rc msg00001.bin aniedit.rc
    rc -r -fo aniedit.res aniedit.rc 
    cvtres -$(CPU) aniedit.res -o aniedit.rbj

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $*.c

aniedit.exe: aniedit.rbj   \
	     aniedit.obj   \
	     anicmd.obj    \
	     anifile.obj   \
             anistr.obj
  $(link) $(linkdebug) $(guiflags) -out:$*.exe $** $(guilibs) advapi32.lib
