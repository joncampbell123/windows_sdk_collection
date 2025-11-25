!include <win32.mak>

mycflag=/I "..\misc" -DD3DRMDEMO

proj=oct1

objs=..\misc\d3dapp.obj ..\misc\ddcalls.obj ..\misc\d3dcalls.obj \
     ..\misc\texture.obj ..\misc\misc.obj ..\misc\d3dmain.obj \
     ..\misc\stats.obj ..\misc\d3dmath.obj ..\misc\lclib.obj 

all: $(proj).exe

$(proj).obj:  $(proj).c
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) $(proj).c

..\misc\d3dapp.obj:  ..\misc\d3dapp.c ..\misc\d3dapp.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) -Fo..\misc\d3dapp.obj ..\misc\d3dapp.c

..\misc\ddcalls.obj:  ..\misc\ddcalls.c 
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) -Fo..\misc\ddcalls.obj ..\misc\ddcalls.c

..\misc\d3dcalls.obj:  ..\misc\d3dcalls.c
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) -Fo..\misc\d3dcalls.obj ..\misc\d3dcalls.c

..\misc\texture.obj:  ..\misc\texture.c 
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) -Fo..\misc\texture.obj ..\misc\texture.c

..\misc\misc.obj:  ..\misc\misc.c 
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) -Fo..\misc\misc.obj ..\misc\misc.c

..\misc\d3dmain.obj:  ..\misc\d3dmain.cpp ..\misc\d3dmain.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) -Fo..\misc\d3dmain.obj ..\misc\d3dmain.cpp

..\misc\stats.obj:  ..\misc\stats.cpp 
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) -Fo..\misc\stats.obj ..\misc\stats.cpp

..\misc\d3dmath.obj:  ..\misc\d3dmath.c ..\misc\d3dmath.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) -Fo..\misc\d3dmath.obj ..\misc\d3dmath.c

..\misc\lclib.obj:  ..\misc\lclib.c ..\misc\lclib.h
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) -Fo..\misc\lclib.obj ..\misc\lclib.c

..\misc\d3dmain.res: ..\misc\d3dmain.rc
   rc -i..\misc -r -fo  ..\misc\d3dmain.res ..\misc\d3dmain.rc
	

$(proj).exe: $(proj).obj  $(objs) ..\misc\d3dmain.res $(proj).def  
     $(link) $(linkdebug) $(guiflags) -out:..\media\$(proj).exe      \
           $(proj).obj  $(objs) ..\misc\d3dmain.res  \
           $(guilibs) ddraw.lib d3drm.lib
