!include <ntwin32.mak>

all: aniedit.exe

animsg.h animsg.rc msg00001.bin: animsg.mc
    mc -v animsg.mc

aniedit.res: animsg.rc msg00001.bin aniedit.rc
    rc -r -fo aniedit.res aniedit.rc 

.c.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $*.c

aniedit.exe: aniedit.res   \
	         aniedit.obj   \
	         anicmd.obj    \
	         anifile.obj   \
             anistr.obj
  $(link) $(linkdebug) $(guiflags) -machine:$(CPU) -out:$*.exe $** $(guilibs) advapi32.lib
