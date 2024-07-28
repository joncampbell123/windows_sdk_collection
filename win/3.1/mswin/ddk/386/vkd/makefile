# sample make file for building vkd virtual device for Windows/386

# Needed for international support 
!IFNDEF LANG 
RES_DIR=.\messages\usa 
!ELSE 
RES_DIR=.\messages\$(LANG) 
!ENDIF 

# comment this definition out with a "#", if building a non-debugging version
Debug=-DDEBUG
Version=-DSupport_Reboot   

MASMOBJ=masm5 -p -w2 -Mx $(Version) $(Debug) -I..\include 
MASMLST=masm5 -l -p -w2 -Mx $(Version) $(Debug) -I..\include 

.asm.obj:
        $(MASMOBJ) $*;

.asm.lst:
        $(MASMLST) $*;

all:    vkd.386

vad.obj vad.lst: vad.asm ..\include\opttest.inc ..\include\vmd.inc \
	 ..\include\debug.inc vkdsys.inc ..\include\vkd.inc \
	 ..\include\vkdpaste.inc ..\include\vpicd.inc ..\include\vmm.inc

vkd.obj vkd.lst: vkd.asm ..\include\opttest.inc vkdsys.inc ..\include\vkd.inc \
	 ..\include\vkdpaste.inc ..\include\vdd.inc ..\include\vsd.inc \
	 ..\include\vpicd.inc ..\include\debug.inc ..\include\vmm.inc

vkdhk.obj vkdhk.lst: vkdhk.asm ..\include\opttest.inc vkdsys.inc \
	 ..\include\vkd.inc ..\include\vkdpaste.inc ..\include\vpicd.inc \
	 ..\include\debug.inc ..\include\vmm.inc

vkdio.obj vkdio.lst: vkdio.asm ..\include\opttest.inc vkdsys.inc \
	 ..\include\vkd.inc ..\include\vkdpaste.inc ..\include\vpicd.inc \
	 ..\include\debug.inc ..\include\vmm.inc

vkdphys.obj vkdphys.lst: vkdphys.asm ..\include\opttest.inc vkdsys.inc \
	 ..\include\vkd.inc ..\include\vkdpaste.inc ..\include\debug.inc \
	 ..\include\vpicd.inc ..\include\vmm.inc


vkdmsg.obj vkdmsg.lst: $(RES_DIR)\vkdmsg.asm \
	 ..\include\vmm.inc
        $(MASMOBJ) $(RES_DIR)\$*, $*.obj;
        $(MASMLST) $(RES_DIR)\$*, $*.lst;

OBJS =  vad.obj vkd.obj vkdhk.obj vkdio.obj vkdmsg.obj vkdphys.obj 

vkd.386: iclean vkd.def $(OBJS)
        link386 @vkd.lnk
        addhdr vkd.386
        mapsym32 vkd

iclean: 
        del vkdmsg.* 

