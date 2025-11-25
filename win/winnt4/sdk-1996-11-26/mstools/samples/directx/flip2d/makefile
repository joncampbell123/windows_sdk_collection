!include <win32.mak>

proj=flipcube

all: $(proj).exe

$(proj).obj:  $(proj).cpp
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) $(proj).cpp

tri.obj:  tri.cpp 
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) tri.cpp

dumb3d.obj:  dumb3d.cpp dumb3d.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) dumb3d.cpp

$(proj).res: $(proj).rc
   rc -r -fo $(proj).res $(proj).rc
	

$(proj).exe: $(proj).obj  tri.obj dumb3d.obj $(proj).res 
     $(link) $(linkdebug) $(guiflags) -out:$(proj).exe      \
           $(proj).obj  tri.obj dumb3d.obj $(proj).res  \
           $(guilibs) ddraw.lib winmm.lib
# Rules for cleaning out those old files
clean:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc

cleanall:
    del *.bak *.pdb *.obj *.res *.exp *.map *.sbr *.bsc *.dll *.exe *.lib
