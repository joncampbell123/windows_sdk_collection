# sample make file for building pagefile virtual device for Windows/386

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

all:    pagefile.386

pagefile.obj pagefile.lst: pagefile.asm spoem.inc spart.inc \
	 ..\include\pageswap.inc ..\include\pagefile.inc \
	 ..\include\opttest.inc ..\include\int2fapi.inc ..\include\pdb.inc \
	 ..\include\smartdrv.inc ..\include\blockdev.inc ..\include\shell.inc \
	 ..\include\shellfsc.inc ..\include\v86mmgr.inc ..\include\dosmgr.inc \
	 ..\include\debug.inc ..\include\vmm.inc

pftext.obj pftext.lst: $(RES_DIR)\pftext.asm \
	 ..\include\vmm.inc
        $(MASMOBJ) $(RES_DIR)\$*, $*.obj;
        $(MASMLST) $(RES_DIR)\$*, $*.lst;

OBJS =  pftext.obj \
        pagefile.obj 

pagefile.386: iclean pagefile.def $(OBJS)
        link386 @pagefile.lnk
        addhdr pagefile.386
        mapsym32 pagefile

iclean: 
        del pftext.* 

