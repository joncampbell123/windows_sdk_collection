# sample make file for building wdctrl virtual device for Windows/386

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

all:    wdctrl.386

wdctrl.obj wdctrl.lst: wdctrl.asm ..\include\shell.inc ..\include\shellfsc.inc \
	 wdlocal.inc ..\include\vpicd.inc ..\include\int13.inc \
	 ..\include\blockdev.inc ..\include\opttest.inc ..\include\debug.inc \
	 ..\include\vmm.inc

wddebug.obj wddebug.lst: wddebug.asm wdlocal.inc ..\include\vpicd.inc \
	 ..\include\int13.inc ..\include\blockdev.inc ..\include\opttest.inc \
	 ..\include\debug.inc ..\include\vmm.inc

wdrminit.obj wdrminit.lst: wdrminit.asm ..\include\int2fapi.inc wdlocal.inc \
	 ..\include\int13.inc ..\include\blockdev.inc ..\include\opttest.inc \
	 ..\include\vmm.inc


wdtrans.obj wdtrans.lst: $(RES_DIR)\wdtrans.asm \
	 ..\include\vmm.inc
        $(MASMOBJ) $(RES_DIR)\$*, $*.obj;
        $(MASMLST) $(RES_DIR)\$*, $*.lst;

OBJS =  wdtrans.obj \
        wdctrl.obj wddebug.obj wdrminit.obj wdtrans.obj 

wdctrl.386: iclean wdctrl.def $(OBJS)
        link386 @wdctrl.lnk
        addhdr wdctrl.386
        mapsym32 wdctrl

iclean: 
        del wdtrans.* 

