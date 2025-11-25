!include <win32.mak>

proj=stretch

mycflag=/I "..\misc"

all: $(proj).exe

$(proj).obj:  $(proj).cpp resource.h ..\misc\ddutil.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) $(proj).cpp
	
ddutil.obj:  ..\misc\ddutil.cpp ..\misc\ddutil.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) ..\misc\ddutil.cpp

$(proj).res: $(proj).rc resource.h
   rc -r -fo $(proj).res $(proj).rc

$(proj).exe: $(proj).obj ddutil.obj $(proj).def  \
           $(proj).res
    $(link) $(linkdebug) $(guiflags) -out:$(proj).exe      \
           $(proj).obj   ddutil.obj \
           $(proj).res   \
           $(guilibs) ddraw.lib
# Rules for cleaning out those old files
clean:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc

cleanall:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc *.dll *.exe *.lib
