# sample make file for building vpd virtual device for Windows/386

# comment this definition out with a "#", if building a non-debugging version
Debug=-DDEBUG

MASMOBJ=masm5 -p -w2 -Mx $(Debug) -I..\include 
MASMLST=masm5 -l -p -w2 -Mx $(Debug) -I..\include 

.asm.obj:
        $(MASMOBJ) $*;

.asm.lst:
        $(MASMLST) $*;

all:    vpd.386

vpd.obj vpd.lst: vpd.asm ..\include\dosmgr.inc ..\include\shell.inc \
	 ..\include\shellfsc.inc ..\include\vpicd.inc ..\include\debug.inc \
	 ..\include\vmm.inc


OBJS =  vpd.obj 

vpd.386: vpd.def $(OBJS)
        link386 @vpd.lnk
        addhdr vpd.386
        mapsym32 vpd

