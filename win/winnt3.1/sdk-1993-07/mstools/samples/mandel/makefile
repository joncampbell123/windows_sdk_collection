# some NT Win32 nmake macros
# for now this resides in samples\inc

!include <ntwin32.mak>

cvars=-DNT -DWIN -DWIN32
obj=obj
!IF "$(CPU)" == "i386"
cpu=i386
!ELSE
cpu=mips
!ENDIF
objs=$(obj)\$(cpu)
mycflag = -Fo.\$(objs)\\

all: julia.exe

# Update the object file if necessary

$(objs)\julia.obj: julia.c julia.h jtypes.h bndscan.h dibmp.h
    IF NOT EXIST $(obj) md $(obj)
    cd $(obj)
    IF NOT EXIST $(cpu) md $(cpu)
    cd ..
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) julia.c

$(objs)\savebmp.obj: savebmp.c jtypes.h
    IF NOT EXIST $(obj) md $(obj)
    cd $(obj)
    IF NOT EXIST $(cpu) md $(cpu)
    cd ..
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) savebmp.c

$(objs)\loadbmp.obj: loadbmp.c jtypes.h
    IF NOT EXIST $(obj) md $(obj)
    cd $(obj)
    IF NOT EXIST $(cpu) md $(cpu)
    cd ..
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) loadbmp.c

$(objs)\dibmp.obj: dibmp.c jtypes.h dibmp.h
    IF NOT EXIST $(obj) md $(obj)
    cd $(obj)
    IF NOT EXIST $(cpu) md $(cpu)
    cd ..
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) dibmp.c

$(objs)\bndscan.obj: bndscan.c bndscan.h jtypes.h
    IF NOT EXIST $(obj) md $(obj)
    cd $(obj)
    IF NOT EXIST $(cpu) md $(cpu)
    cd ..
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) bndscan.c

$(objs)\printer.obj: printer.c printer.h
    IF NOT EXIST $(obj) md $(obj)
    cd $(obj)
    IF NOT EXIST $(cpu) md $(cpu)
    cd ..
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) printer.c

# Update the resources if necessary

$(objs)\julia.rbj: julia.rc julia.h jtypes.h bndscan.h dibmp.h
    rc -r -fo .\$(objs)\julia.res julia.rc
    cvtres -$(CPU) $(objs)\julia.res -o $(objs)\julia.rbj

# Update the executable file if necessary, and if so, add the resource back in.

julia.exe: $(objs)\julia.obj \
           $(objs)\loadbmp.obj \
           $(objs)\savebmp.obj \
           $(objs)\dibmp.obj   \
           $(objs)\bndscan.obj \
           $(objs)\printer.obj \
	     $(objs)\julia.rbj
    cd ..\mandel
    $(link) $(linkdebug) $(guiflags) -out:julia.exe	 \
    $(objs)\julia.obj \
    $(objs)\savebmp.obj \
    $(objs)\loadbmp.obj \
    $(objs)\dibmp.obj   \
    $(objs)\bndscan.obj \
    $(objs)\printer.obj \
    largeint.lib \
    $(objs)\julia.rbj $(guilibs) shell32.lib
