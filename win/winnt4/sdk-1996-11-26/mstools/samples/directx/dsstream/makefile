!include <win32.mak>

proj=dsstream

all: $(proj).exe

$(proj).res: $(proj).rc resource.h
   rc -r -fo $(proj).res $(proj).rc

debug.obj: debug.c debug.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) debug.c

wassert.obj: wassert.c wassert.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) wassert.c 

dsstream.obj: dsstream.c dsstream.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) dsstream.c 

dstrenum.obj: dstrenum.c dsstream.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) dstrenum.c

dstrwave.obj: dstrwave.c dsstream.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) dstrwave.c

dstrtime.obj: dstrtime.c dsstream.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) dstrtime.c


$(proj).exe: debug.obj wassert.obj dsstream.obj  \
           dstrenum.obj dstrwave.obj dstrtime.obj   $(proj).def  \
           $(proj).res
    $(link) $(linkdebug) $(guiflags) -out:$(proj).exe      \
           debug.obj wassert.obj dsstream.obj  \
           dstrenum.obj dstrwave.obj dstrtime.obj \
           $(proj).res   \
           $(guilibs) winmm.lib dsound.lib comctl32.lib
# Rules for cleaning out those old files
clean:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc

cleanall:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc *.dll *.exe *.lib
