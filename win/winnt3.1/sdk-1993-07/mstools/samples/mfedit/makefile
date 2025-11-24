# some NT Win32 nmake macros
# for now this resides in samples\inc

!include <ntwin32.mak>

cvars=-DNT -DWIN -DWIN32
obj=obj
mycflag = -Fo.\$(obj)\\

all: mfedit.exe

# Update the object file if necessary

$(obj)\mfedit.obj: mfedit.c mfedit.h
    IF NOT EXIST $(obj) md obj
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) mfedit.c

# Update the resources if necessary

$(obj)\mfedit.rbj: mfedit.rc mfedit.h
    rc -r -fo .\$(obj)\mfedit.res mfedit.rc
    cvtres -$(CPU) $(obj)\mfedit.res -o $(obj)\mfedit.rbj

# Update the executable file if necessary, and if so, add the resource back in.

mfedit.exe: $(obj)\mfedit.obj \
	     $(obj)\mfedit.rbj
    $(link) $(linkdebug) $(guiflags) -out:mfedit.exe	 \
    $(obj)\mfedit.obj \
    $(obj)\mfedit.rbj $(guilibs) shell32.lib
