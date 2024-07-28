# sample make file for building vcd virtual device for Windows/386

# Needed for international support 
!IFNDEF LANG 
RES_DIR=.\messages\usa 
!ELSE 
RES_DIR=.\messages\$(LANG) 
!ENDIF 

# comment this definition out with a "#", if building a non-debugging version
Debug=-DDEBUG
Version=-DSHARE_IRQ   

MASMOBJ=masm5 -p -w2 -Mx $(Version) $(Debug) -I..\include 
MASMLST=masm5 -l -p -w2 -Mx $(Version) $(Debug) -I..\include 

.asm.obj:
        $(MASMOBJ) $*;

.asm.lst:
        $(MASMLST) $*;

all:    vcd.386

vcd.obj vcd.lst: vcd.asm vcdsys.inc ..\include\vcd.inc ..\include\vmpoll.inc \
	 ..\include\shell.inc ..\include\shellfsc.inc ..\include\dosmgr.inc \
	 ..\include\debug.inc ..\include\vpicd.inc ..\include\vmm.inc

vcdint.obj vcdint.lst: vcdint.asm ibmcom.inc ..\include\int31.inc comdev.inc \
	 ..\include\vcd.inc ..\include\debug.inc ..\include\vmm.inc


vcdmsg.obj vcdmsg.lst: $(RES_DIR)\vcdmsg.asm \
	 ..\include\vmm.inc
        $(MASMOBJ) $(RES_DIR)\$*, $*.obj;
        $(MASMLST) $(RES_DIR)\$*, $*.lst;

OBJS =  vcd.obj vcdint.obj vcdmsg.obj 

vcd.386: iclean vcd.def $(OBJS)
        link386 @vcd.lnk
        addhdr vcd.386
        mapsym32 vcd

iclean: 
        del vcdmsg.* 

