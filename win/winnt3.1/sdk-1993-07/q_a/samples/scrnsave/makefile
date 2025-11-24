# some NT Win32 nmake macros
# for now this resides in samples\inc

!include <ntwin32.mak>

cvars=-DNT -DWIN -DWIN32
obj=obj
guiflags=-subsystem:windows -entry:mainCRTStartup

mycflag = -Fo.\$(obj)\\

all: fractal.scr

# Update the object file if necessary

$(obj)\fractal.obj: fractal.c fractal.h fractal.def
    IF NOT EXIST $(obj) md obj
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) fractal.c

# Update the resources if necessary

$(obj)\fractal.rbj: fractal.rc fractal.h
    rc -r -fo .\$(obj)\fractal.res fractal.rc
    cvtres -$(CPU) $(obj)\fractal.res -o $(obj)\fractal.rbj

# Update the executable file if necessary, and if so, add the resource back in.

fractal.scr: $(obj)\fractal.obj \
	     $(obj)\fractal.rbj
    $(link) $(linkdebug) $(guiflags) -out:fractal.scr	 \
    $(obj)\fractal.obj \
    $(obj)\fractal.rbj $(guilibs) scrnsave.lib
