# sample make file for building vmd virtual device for Windows/386

# comment this definition out with a "#", if building a non-debugging version
Debug=-DDEBUG

MASMOBJ=masm5 -p -w2 -Mx $(Debug) -I..\include 
MASMLST=masm5 -l -p -w2 -Mx $(Debug) -I..\include 

.asm.obj:
        $(MASMOBJ) $*;

.asm.lst:
        $(MASMLST) $*;

all:    vmd.386

int33.obj int33.lst: int33.asm ..\include\arena.asm ..\include\opttest.inc \
	 ..\include\vdd.inc ..\include\v86mmgr.inc ..\include\int2fapi.inc \
	 ..\include\dosmgr.inc ..\include\vmd.inc ..\include\vdd.inc \
	 ..\include\debug.inc ..\include\vmm.inc

vmd.obj vmd.lst: vmd.asm ..\include\vmd.inc ..\include\vcd.inc \
	 ..\include\vpicd.inc ..\include\debug.inc ..\include\vmm.inc

vmdmsg.obj vmdmsg.lst: vmdmsg.asm ..\include\vmm.inc

..\include\arena.obj ..\include\arena.lst: ..\include\arena.asm


OBJS =  int33.obj vmd.obj vmdmsg.obj 

vmd.386: vmd.def $(OBJS)
        link386 @vmd.lnk
        addhdr vmd.386
        mapsym32 vmd

