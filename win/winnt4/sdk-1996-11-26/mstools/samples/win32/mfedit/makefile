# some NT Win32 nmake macros

!IF  "$(TARGETLANG)"=="LANG_JAPANESE"
!IF  "$(OS)"=="Windows_NT"
APPVER = 3.51
!ENDIF
!ENDIF

!include <win32.mak>

!IF "$(APPVER)"=="4.0"
cvars=-DNT -DWIN -DWIN32 -DOPENGL_EMF
!ELSE
cvars=-DNT -DWIN -DWIN32
!ENDIF

obj=obj
mycflag = -Fo.\$(obj)\\

all: mfedit.exe

# Update the object file if necessary

$(obj)\mfedit.obj: mfedit.c mfedit.h precomp.h
    IF NOT EXIST $(obj) md obj
    $(cc) $(cdebug) $(cflags) $(mycflag) $(cvars) mfedit.c

# Update the resources if necessary

$(obj)\mfedit.res: mfedit.rc mfedit.h
    rc $(rcvars) -r -fo .\$(obj)\mfedit.res mfedit.rc

# Update the executable file if necessary, and if so, add the resource back in.

mfedit.exe: $(obj)\mfedit.obj \
	     $(obj)\mfedit.res
    $(link) $(linkdebug) $(guiflags) -out:mfedit.exe	 \
    $(obj)\mfedit.obj \
    $(obj)\mfedit.res $(guilibs) shell32.lib glaux.lib opengl32.lib
