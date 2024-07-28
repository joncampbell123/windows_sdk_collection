# sample make file for building int13 virtual device for Windows/386

# comment this definition out with a "#", if building a non-debugging version
Debug=-DDEBUG

MASMOBJ=masm5 -p -w2 -Mx $(Debug) -I..\include 
MASMLST=masm5 -l -p -w2 -Mx $(Debug) -I..\include 

.asm.obj:
        $(MASMOBJ) $*;

.asm.lst:
        $(MASMLST) $*;

all:    int13.386

i13debug.obj i13debug.lst: i13debug.asm ..\include\blockdev.inc \
	 ..\include\debug.inc ..\include\vmm.inc

i13trans.obj i13trans.lst: i13trans.asm ..\include\vmm.inc

int13.obj int13.lst: int13.asm ..\include\int13.inc ..\include\blockdev.inc \
	 ..\include\opttest.inc ..\include\debug.inc ..\include\vmm.inc


OBJS =  i13debug.obj i13trans.obj int13.obj 

int13.386: int13.def $(OBJS)
        link386 @int13.lnk
        addhdr int13.386
        mapsym32 int13

