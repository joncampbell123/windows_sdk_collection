# sample make file for building vddcga virtual device for Windows/386

# Needed for international support 
!IFNDEF LANG 
RES_DIR=.\messages\usa 
!ELSE 
RES_DIR=.\messages\$(LANG) 
!ENDIF 

# comment this definition out with a "#", if building a non-debugging version
Debug=-DDEBUG

MASMOBJ=masm5 -p -w2 -Mx $(Debug) -I..\include 
MASMLST=masm5 -l -p -w2 -Mx $(Debug) -I..\include 

.asm.obj:
        $(MASMOBJ) $*;

.asm.lst:
        $(MASMLST) $*;

all:    vddcga.386

cgafont.obj cgafont.lst: cgafont.asm ..\include\debug.inc cga.inc \
	 ..\include\vmdacga.inc ..\include\vdd.inc ..\include\vmm.inc

cgainit.obj cgainit.lst: cgainit.asm ..\include\debug.inc ..\include\vdd.inc \
	 cga.inc ..\include\vmdacga.inc ..\include\vmm.inc

cgaint.obj cgaint.lst: cgaint.asm ..\include\debug.inc cga.inc \
	 ..\include\vmdacga.inc ..\include\vdd.inc ..\include\vmm.inc

cgamem.obj cgamem.lst: cgamem.asm ..\include\debug.inc cga.inc \
	 ..\include\vmdacga.inc ..\include\vdd.inc ..\include\vmm.inc

cgaproc.obj cgaproc.lst: cgaproc.asm ..\include\debug.inc \
	 ..\include\vddgrb.inc ..\include\shell.inc ..\include\shellfsc.inc \
	 cga.inc ..\include\vmdacga.inc ..\include\vdd.inc ..\include\vmm.inc

cgasave.obj cgasave.lst: cgasave.asm ..\include\vdd.inc ..\include\debug.inc \
	 cga.inc ..\include\vmdacga.inc ..\include\vmm.inc

cgastate.obj cgastate.lst: cgastate.asm ..\include\debug.inc cga.inc \
	 ..\include\vmdacga.inc ..\include\vdd.inc ..\include\vmm.inc

cgasvc.obj cgasvc.lst: cgasvc.asm ..\include\debug.inc ..\include\shell.inc \
	 ..\include\shellfsc.inc cga.inc ..\include\vmdacga.inc \
	 ..\include\vdd.inc ..\include\vmm.inc

cgatio.obj cgatio.lst: cgatio.asm ..\include\debug.inc ..\include\vdd.inc \
	 cga.inc ..\include\vmdacga.inc ..\include\vmm.inc


cgamsg.obj cgamsg.lst: $(RES_DIR)\cgamsg.asm \
	 ..\include\vmm.inc
        $(MASMOBJ) $(RES_DIR)\$*, $*.obj;
        $(MASMLST) $(RES_DIR)\$*, $*.lst;

OBJS =  cgafont.obj cgaint.obj cgamsg.obj cgasave.obj cgasvc.obj \
        cgainit.obj cgamem.obj cgaproc.obj cgastate.obj cgatio.obj 

vddcga.386: iclean vddcga.def $(OBJS)
        link386 @vddcga.lnk
        addhdr vddcga.386
        mapsym32 vddcga

iclean: 
        del cgamsg.* 

