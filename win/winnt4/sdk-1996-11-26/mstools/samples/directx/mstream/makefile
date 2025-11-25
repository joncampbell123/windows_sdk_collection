!include <win32.mak>

proj=mstream

all: $(proj).exe

$(proj).obj:  $(proj).c resource.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) $(proj).c

debug.obj:  debug.c debug.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) debug.c

mstrconv.obj:  mstrconv.c 
    $(cc) $(debug) $(cflags) $(mycflag) $(cvars) mstrconv.c

mstrhelp.obj:  mstrhelp.c 
    $(cc) $(debug) $(cflags) $(mycflag) $(cvars) mstrhelp.c
	
$(proj).res: $(proj).rc resource.h
   rc -r -fo $(proj).res $(proj).rc


$(proj).exe: $(proj).obj  debug.obj mstrconv.obj mstrhelp.obj  $(proj).def  \
           $(proj).res
    $(link) $(linkdebug) $(guiflags) -out:$(proj).exe      \
           $(proj).obj   debug.obj mstrconv.obj mstrhelp.obj \
           $(proj).res   \
           $(guilibs) ddraw.lib winmm.lib comctl32.lib

# Rules for cleaning out those old files
clean:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc

cleanall:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc *.dll *.exe *.lib
