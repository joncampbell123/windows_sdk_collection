# sample make file for building ldosnet virtual device for Windows/386

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

all:    ldosnet.386

dosnet.obj dosnet.lst: dosnet.asm ..\include\dosnet.inc ..\include\shell.inc \
	 ..\include\shellfsc.inc dnlocal.inc ..\include\debug.inc \
	 ..\include\vmm.inc


dntrans.obj dntrans.lst: $(RES_DIR)\dntrans.asm \
	 ..\include\debug.inc ..\include\vmm.inc
        $(MASMOBJ) $(RES_DIR)\$*, $*.obj;
        $(MASMLST) $(RES_DIR)\$*, $*.lst;

OBJS =  dntrans.obj \
        dosnet.obj 

ldosnet.386: iclean ldosnet.def $(OBJS)
        link386 @ldosnet.lnk
        addhdr ldosnet.386
        mapsym32 ldosnet

iclean: 
        del dntrans.* 

