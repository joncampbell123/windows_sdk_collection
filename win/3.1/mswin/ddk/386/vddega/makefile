# sample make file for building vddega virtual device for Windows/386

# Needed for international support 
!IFNDEF LANG 
RES_DIR=.\messages\usa 
!ELSE 
RES_DIR=.\messages\$(LANG) 
!ENDIF 

# comment this definition out with a "#", if building a non-debugging version
Debug=-DDEBUG
Version=-DEGA   

MASMOBJ=masm5 -p -w2 -Mx $(Version) $(Debug) -I..\include 
MASMLST=masm5 -l -p -w2 -Mx $(Version) $(Debug) -I..\include 

.asm.obj:
        $(MASMOBJ) $*;

.asm.lst:
        $(MASMLST) $*;

all:    vddega.386

vddctl.obj vddctl.lst: vddctl.asm ..\include\debug.inc ..\include\pageswap.inc \
	 ega.inc ..\include\vmdaega.inc ..\include\vdd.inc \
	 ..\include\opttest.inc ..\include\vmm.inc

vdddata.obj vdddata.lst: vdddata.asm ega.inc ..\include\vmdaega.inc \
	 ..\include\vmm.inc

vddinit.obj vddinit.lst: vddinit.asm ..\include\debug.inc ..\include\vdd2.inc \
	 ..\include\vdd.inc ega.inc ..\include\vmdaega.inc \
	 ..\include\opttest.inc ..\include\vmm.inc

vddint.obj vddint.lst: vddint.asm ..\include\debug.inc ega.inc \
	 ..\include\vmdaega.inc ..\include\vdd.inc ..\include\opttest.inc \
	 ..\include\vmm.inc

vddmem.obj vddmem.lst: vddmem.asm ..\include\debug.inc ega.inc \
	 ..\include\vmdaega.inc ..\include\pageswap.inc ..\include\vdd.inc \
	 ..\include\opttest.inc ..\include\vmm.inc

vddproc.obj vddproc.lst: vddproc.asm ..\include\debug.inc \
	 ..\include\vddgrb.inc ..\include\shell.inc ..\include\shellfsc.inc \
	 ega.inc ..\include\vmdaega.inc ..\include\vdd.inc \
	 ..\include\opttest.inc ..\include\vmm.inc

vddsave.obj vddsave.lst: vddsave.asm ..\include\debug.inc vga.inc \
	 ..\include\vddgrb.inc ..\include\vdd.inc ..\include\shell.inc \
	 ..\include\shellfsc.inc ega.inc ..\include\vmdaega.inc \
	 ..\include\opttest.inc ..\include\vmm.inc

vddstate.obj vddstate.lst: vddstate.asm ..\include\debug.inc ega.inc \
	 ..\include\vmdaega.inc ..\include\vdd.inc ..\include\opttest.inc \
	 ..\include\vmm.inc

vddsvc.obj vddsvc.lst: vddsvc.asm ..\include\debug.inc ..\include\shell.inc \
	 ..\include\shellfsc.inc ega.inc ..\include\vmdaega.inc \
	 ..\include\vdd.inc ..\include\opttest.inc ..\include\vmm.inc

vddtio.obj vddtio.lst: vddtio.asm vga.inc ..\include\debug.inc \
	 ..\include\vdd.inc ega.inc ..\include\vmdaega.inc \
	 ..\include\opttest.inc ..\include\vmm.inc

vddvga.obj vddvga.lst: vddvga.asm vga.inc ..\include\debug.inc ega.inc \
	 ..\include\vmdaega.inc ..\include\opttest.inc ..\include\vmm.inc


vddmsg.obj vddmsg.lst: $(RES_DIR)\vddmsg.asm \
	 ..\include\vmm.inc
        $(MASMOBJ) $(RES_DIR)\$*, $*.obj;
        $(MASMLST) $(RES_DIR)\$*, $*.lst;

OBJS =  vddctl.obj vddinit.obj vddmem.obj vddproc.obj vddstate.obj vddtio.obj \
        vdddata.obj vddint.obj vddmsg.obj vddsave.obj vddsvc.obj vddvga.obj 

vddega.386: iclean vddega.def $(OBJS)
        link386 @vddega.lnk
        addhdr vddega.386
        mapsym32 vddega

iclean: 
        del vddmsg.* 

