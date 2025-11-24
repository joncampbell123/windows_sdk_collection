!include <ntwin32.mak>

all: psxarc.exe

psxarc.obj: psxarc.c *.h

buf.obj: buf.c *.h

archive.obj: archive.c *.h

tar.obj: tar.c *.h

links.obj: links.c *.h

cpio.obj: cpio.c *.h

getopt.obj: getopt.c *.h

psxarc.exe: psxarc.obj buf.obj archive.obj tar.obj links.obj cpio.obj \
	    getopt.obj 
  $(link) $(linkdebug) $(psxflags) -out:$*.exe $** $(psxlibs)
.c.obj:
  $(cc) $(cflags) $(psxvars) $(cvars) $(cdebug) $*.c
