# sample make file for building ebios virtual device for Windows/386

# comment this definition out with a "#", if building a non-debugging version
Debug=-DDEBUG

MASMOBJ=masm5 -p -w2 -Mx $(Debug) -I..\include 
MASMLST=masm5 -l -p -w2 -Mx $(Debug) -I..\include 

.asm.obj:
        $(MASMOBJ) $*;

.asm.lst:
        $(MASMLST) $*;

all:    ebios.386

ebios.obj ebios.lst: ebios.asm ..\include\arena.asm ..\include\v86mmgr.inc \
	 ..\include\sysinfo.inc ..\include\ebios.inc ..\include\debug.inc \
	 ..\include\vmm.inc

..\include\arena.obj ..\include\arena.lst: ..\include\arena.asm


OBJS =  ebios.obj 

ebios.386: ebios.def $(OBJS)
        link386 @ebios.lnk
        addhdr ebios.386
        mapsym32 ebios

