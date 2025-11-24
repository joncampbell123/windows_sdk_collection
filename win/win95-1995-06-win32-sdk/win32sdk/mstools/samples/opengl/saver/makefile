
TARGETOS=WINNT

!include <ntwin32.mak>

all: ss3dfo.scr

.c.obj:
    $(cc) $(cdebug) $(cflags) $(cvars) $(scall) $*.c

ss3dfo.res: ss3dfo.rc 
    $(rc) -r ss3dfo.rc

ss3dfo.scr: ss3dfo.res \
            gendrop.obj     \
            genexpld.obj    \
            genlem.obj      \
            genstrip.obj    \
            gentex.obj      \
            genwin.obj      \
            image.obj       \
            math.obj        \
            mesh.obj        \
            ss3dfo.obj
  $(link) $(linkdebug) $(savlflags) -machine:$(CPU) -out:$*.scr $** $(guilibs) opengl32.lib glu32.lib glaux.lib scrnsave.lib comdlg32.lib
