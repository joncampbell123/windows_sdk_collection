!include <win32.mak>

mycflag=-DD3DRMDEMO

proj=rockem

all: $(proj).exe

CONTROL.obj: CONTROL.CPP
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) CONTROL.CPP

DIRECTX.obj: DIRECTX.CPP
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) DIRECTX.CPP

MIDI.obj: MIDI.CPP
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) MIDI.CPP

RM.obj: RM.CPP
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) RM.CPP

WINMAIN.obj: WINMAIN.CPP
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) WINMAIN.CPP

ROCKEM3D.res: ROCKEM3D.rc
   $(rc) -r -fo ROCKEM3D.res  ROCKEM3D.rc
	

$(proj).exe: CONTROL.obj  DIRECTX.obj MIDI.obj WINMAIN.obj RM.OBJ ROCKEM3D.res 
     $(link) $(linkdebug) $(guiflags) -out:$(proj).exe      \
           CONTROL.obj  DIRECTX.obj MIDI.obj WINMAIN.obj RM.OBJ ROCKEM3D.res \
           $(guilibs) ddraw.lib dsound.lib winmm.lib d3drm.lib


