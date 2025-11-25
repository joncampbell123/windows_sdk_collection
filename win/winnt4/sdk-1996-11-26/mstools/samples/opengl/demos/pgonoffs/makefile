
TARGETOS=WINNT

!include <ntwin32.mak>

TARGET  = toffset

# application specific C defines

cf = -DNT -DWIN -DNOT_IMPLEMENTED $(scall)

# This line allows NMAKE to work as well

all: $(TARGET).exe

.c.obj:
    $(cc) $(cdebug) $(cflags) $(cvars) $(cf) $*.c

$(TARGET).exe: toffset.obj  trackbal.obj    
    $(link) $(linkdebug) $(savlflags) -machine:$(CPU) -subsystem:windows -entry:mainCRTStartup -out:$(TARGET).exe  $** $(guilibs) opengl32.lib glu32.lib glaux.lib comdlg32.lib
