# sample make file for building vnetbios virtual device for Windows/386

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

all:    vnetbios.386

netbuff.obj netbuff.lst: netbuff.asm ..\include\shell.inc \
	 ..\include\shellfsc.inc ..\include\v86mmgr.inc nblocal.inc \
	 ..\include\debug.inc ..\include\vmm.inc

netdebug.obj netdebug.lst: netdebug.asm nblocal.inc ..\include\debug.inc \
	 ..\include\vmm.inc

vnetbios.obj vnetbios.lst: vnetbios.asm ..\include\vnetbios.inc \
	 ..\include\opttest.inc ..\include\shell.inc ..\include\shellfsc.inc \
	 ..\include\vdmad.inc ..\include\sysinfo.inc ..\include\v86mmgr.inc \
	 ..\include\int2fapi.inc nblocal.inc ..\include\dosmgr.inc \
	 ..\include\debug.inc ..\include\vmm.inc


netstr.obj netstr.lst: $(RES_DIR)\netstr.asm \
	 ..\include\vmm.inc
        $(MASMOBJ) $(RES_DIR)\$*, $*.obj;
        $(MASMLST) $(RES_DIR)\$*, $*.lst;

OBJS =  netbuff.obj netdebug.obj netstr.obj vnetbios.obj 

vnetbios.386: iclean vnetbios.def $(OBJS)
        link386 @vnetbios.lnk
        addhdr vnetbios.386
        mapsym32 vnetbios

iclean: 
        del netstr.* 

