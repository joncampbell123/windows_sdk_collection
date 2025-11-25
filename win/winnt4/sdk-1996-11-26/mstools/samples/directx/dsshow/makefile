!include <win32.mak>

proj=dsshow

all: $(proj).exe

$(proj).res: $(proj).rc resource.h
   rc -r -fo $(proj).res $(proj).rc

wave.obj: wave.c wave.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) wave.c

shell.obj: shell.c shell.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) shell.c 

dsenum.obj: dsenum.c dsenum.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) dsenum.c 

wassert.obj: wassert.c wassert.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) wassert.c


$(proj).exe: wave.obj shell.obj dsenum.obj wassert.obj  $(proj).def  \
           $(proj).res
    $(link) $(linkdebug) $(guiflags) -out:$(proj).exe      \
           wave.obj shell.obj dsenum.obj wassert.obj \
           $(proj).res   \
           $(guilibs) dsound.lib winmm.lib msacm32.lib \
           ole32.lib uuid.lib


# Rules for cleaning out those old files
clean:
    del *.bak 
    del *.pdb 
    del *.obj
    del *.res
    del *.exp
    del *.map
    del *.sbr
    del *.bsc

cleanall:
    del *.bak
    del *.pdb
    del *.obj
    del *.res
    del *.exp
    del *.map
    del *.sbr
    del *.bsc
    del *.dll
    del *.exe
    del *.lib
