# sample make file for building pageswap virtual device for Windows/386

# comment this definition out with a "#", if building a non-debugging version
Debug=-DDEBUG

MASMOBJ=masm5 -p -w2 -Mx $(Debug) -I..\include 
MASMLST=masm5 -l -p -w2 -Mx $(Debug) -I..\include 

.asm.obj:
        $(MASMOBJ) $*;

.asm.lst:
        $(MASMLST) $*;

all:    pageswap.386

pageswap.obj pageswap.lst: pageswap.asm ..\include\pageswap.inc \
	 ..\include\blockdev.inc ..\include\opttest.inc \
	 ..\include\pagefile.inc ..\include\shell.inc ..\include\shellfsc.inc \
	 ..\include\v86mmgr.inc ..\include\dosmgr.inc ..\include\debug.inc \
	 ..\include\vmm.inc

pstext.obj pstext.lst: pstext.asm ..\include\vmm.inc


OBJS =  pageswap.obj pstext.obj 

pageswap.386: pageswap.def $(OBJS)
        link386 @pageswap.lnk
        addhdr pageswap.386
        mapsym32 pageswap

