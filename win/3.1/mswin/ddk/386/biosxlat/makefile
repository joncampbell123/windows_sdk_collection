# sample make file for building biosxlat virtual device for Windows/386

# comment this definition out with a "#", if building a non-debugging version
Debug=-DDEBUG

MASMOBJ=masm5 -p -w2 -Mx $(Debug) -I..\include 
MASMLST=masm5 -l -p -w2 -Mx $(Debug) -I..\include 

.asm.obj:
        $(MASMOBJ) $*;

.asm.lst:
        $(MASMLST) $*;

all:    biosxlat.386

biosxlat.obj biosxlat.lst: biosxlat.asm ..\include\v86mmgr.inc \
	 ..\include\debug.inc ..\include\vmm.inc


OBJS =  biosxlat.obj 

biosxlat.386: biosxlat.def $(OBJS)
        link386 @biosxlat.lnk
        addhdr biosxlat.386
        mapsym32 biosxlat

