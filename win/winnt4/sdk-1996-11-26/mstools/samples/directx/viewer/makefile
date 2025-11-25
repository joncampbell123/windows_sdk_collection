!include <win32.mak>

proj=viewer

all: $(proj).exe

objs  = viewer.obj sel.obj file.obj color.obj rodcone.obj

$(proj).obj:  $(proj).cpp  $(proj).h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) $(proj).cpp
	
sel.obj:  sel.cpp  sel.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) sel.cpp

file.obj:  file.cpp  
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) file.cpp

color.obj: color.cpp  
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) color.cpp

rodcone.obj:  rodcone.cpp  rodcone.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) rodcone.cpp

$(proj).res: $(proj).rc 
   rc -i..\misc -r -fo  $(proj).res $(proj).rc

$(proj).exe: $(objs)    \
           $(proj).res
    $(link) $(linkdebug) $(guiflags) -out:..\media\$(proj).exe      \
           $(objs)   \
           $(proj).res   \
           $(guilibs) ddraw.lib  d3drm.lib comdlg32.lib winmm.lib msvcrt.lib

