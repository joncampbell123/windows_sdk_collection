!include <win32.mak>

mycflag=/I "..\misc" /I "..\fastfile"

proj=foxbear

objs=foxbear.obj gameproc.obj tile.obj plane.obj sprite.obj gfx.obj \
                bmp.obj ddraw.obj fbsound.obj dsutil.obj

all: $(proj).exe

$(proj).obj:  $(proj).c
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) $(proj).c

gameproc.obj:  gameproc.c gameproc.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) gameproc.c
tile.obj:  tile.c tile.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) tile.c
plane.obj:  plane.c plane.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) plane.c
sprite.obj:  sprite.c sprite.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) sprite.c
gfx.obj:  gfx.c gfx.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) gfx.c
bmp.obj:  bmp.c
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) bmp.c
ddraw.obj:  ddraw.c
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) ddraw.c
fbsound.obj:  fbsound.c fbsound.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) fbsound.c
dsutil.obj:  ..\misc\dsutil.c ..\misc\dsutil.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) ..\misc\dsutil.c

foxbear.res: foxbear.rc
   rc -r -fo foxbear.res foxbear.rc

$(proj).exe: $(objs) foxbear.res $(proj).def  
     cd ..\fastfile
     nmake -i -nologo
     cd ..\foxbear
     $(link) $(linkdebug) $(guiflags) -out:$(proj).exe      \
           $(objs) foxbear.res   \
           $(guilibs) ddraw.lib  dsound.lib winmm.lib ..\fastfile\fastfile.lib

# Rules for cleaning out those old files
clean:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc

cleanall:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc *.dll *.exe *.lib
