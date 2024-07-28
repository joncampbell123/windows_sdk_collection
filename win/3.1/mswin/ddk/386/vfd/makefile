# sample make file for building vfd virtual device for Windows/386

# comment this definition out with a "#", if building a non-debugging version
Debug=-DDEBUG

MASMOBJ=masm5 -p -w2 -Mx $(Debug) -I..\include 
MASMLST=masm5 -l -p -w2 -Mx $(Debug) -I..\include 

.asm.obj:
        $(MASMOBJ) $*;

.asm.lst:
        $(MASMLST) $*;

all:    vfd.386

vfd.obj vfd.lst: vfd.asm ..\include\opttest.inc ..\include\vpicd.inc \
	 ..\include\debug.inc ..\include\vdmad.inc ..\include\vtd.inc \
	 ..\include\vmm.inc

vfdmsg.obj vfdmsg.lst: vfdmsg.asm ..\include\vmm.inc


OBJS =  vfd.obj vfdmsg.obj 

vfd.386: vfd.def $(OBJS)
        link386 @vfd.lnk
        addhdr vfd.386
        mapsym32 vfd

