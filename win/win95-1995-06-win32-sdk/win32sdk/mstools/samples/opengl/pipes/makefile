
TARGETOS=WINNT

!include <ntwin32.mak>

all: sspipes.scr

.c.obj:
    $(cc) $(cdebug) $(cflags) $(cvars) $(scall) $*.c

sspipes.res: sspipes.rc sspipes.h
    $(rc) -r sspipes.rc

sspipes.scr: sspipes.res \
                material.obj    \
                objects.obj     \
                pipes.obj       \
                texture.obj     \
                util.obj        \
                sspipes.obj
  $(link) $(linkdebug) $(savlflags) -machine:$(CPU) -out:$*.scr $** $(guilibs) opengl32.lib glu32.lib glaux.lib scrnsave.lib comdlg32.lib
