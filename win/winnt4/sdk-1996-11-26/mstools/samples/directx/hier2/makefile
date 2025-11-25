!include <win32.mak>

mycflag=/I "..\misc" -DD3DRMDEMO

proj=hier2

all: $(proj).exe

$(proj).obj:  $(proj).c
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) $(proj).c

..\misc\rmmain.obj:  ..\misc\rmmain.cpp
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) -Fo..\misc\rmmain.obj ..\misc\rmmain.cpp
	
..\misc\rmerror.obj:  ..\misc\rmerror.c
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) -Fo..\misc\rmerror.obj ..\misc\rmerror.c

..\misc\rmmain.res: ..\misc\rmmain.rc
   rc -i..\misc -r -fo  ..\misc\rmmain.res ..\misc\rmmain.rc
	

$(proj).exe: $(proj).obj  ..\misc\rmmain.obj ..\misc\rmerror.obj ..\misc\rmmain.res $(proj).def  
     $(link) $(linkdebug) $(guiflags) -out:..\media\$(proj).exe      \
           $(proj).obj    ..\misc\rmmain.obj ..\misc\rmerror.obj ..\misc\rmmain.res \
           $(guilibs) ddraw.lib d3drm.lib
