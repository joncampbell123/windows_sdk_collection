!include <win32.mak>

proj=palette

mycflag=/I "..\misc"

all: $(proj).exe

$(proj).obj:  $(proj).cpp resource.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) $(proj).cpp
	
LBPRINTF.obj:  ..\misc\LBPRINTF.c ..\misc\LBPRINTF.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) ..\misc\LBPRINTF.c

$(proj).res: $(proj).rc resource.h
   rc -r -fo $(proj).res $(proj).rc


$(proj).exe: $(proj).obj  LBPRINTF.obj $(proj).def  \
           $(proj).res
    $(link) $(linkdebug) $(guiflags) -out:$(proj).exe      \
           $(proj).obj LBPRINTF.obj   \
           $(proj).res   \
           $(guilibs) ddraw.lib

# Rules for cleaning out those old files
clean:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc

cleanall:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc *.dll *.exe *.lib
