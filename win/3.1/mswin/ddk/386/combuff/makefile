# sample make file for building combuff virtual device for Windows/386

# comment this definition out with a "#", if building a non-debugging version
Debug=-DDEBUG

MASMOBJ=masm5 -p -w2 -Mx $(Debug) -I..\include 
MASMLST=masm5 -l -p -w2 -Mx $(Debug) -I..\include 

.asm.obj:
        $(MASMOBJ) $*;

.asm.lst:
        $(MASMLST) $*;

all:    combuff.386

cbtext.obj cbtext.lst: cbtext.asm ..\include\vmm.inc

combuff.obj combuff.lst: combuff.asm ..\include\vcd.inc ..\include\vpicd.inc \
	 ..\include\debug.inc ..\include\vmm.inc


OBJS =  cbtext.obj combuff.obj 

combuff.386: combuff.def $(OBJS)
        link386 @combuff.lnk
        addhdr combuff.386
        mapsym32 combuff

