# sample make file for building EGA grabber for Windows/386

# comment this definition out with a "#", if building a non-debugging version
Debug=-DDEBUG

.asm.obj:
        masm5 -w1 -Mx -t $(Debug) -I. -I..\include -p .\$*.asm;

all: grabber.lib grbega.res ega.3gr

grabber.lib: grabber.def
	implib grabber.lib grabber.def

grbega.res: grbega.rc .\grabber.rcv ..\include\version.h ..\include\ver.h \
	    ..\include\common.ver
	rc /r -X -I..\include $*.rc

ega.obj: ega.asm  ega.inc grabmac.inc ..\include\vmdaega.inc \
	 grabpnt.inc ..\include\statusfl.inc \
	 ..\include\selectio.inc ..\include\int2fapi.inc \
	 ..\include\vmda.inc ..\include\vddgrb.inc \
	 ..\include\windows.inc wcmacros.inc

egagrab.obj: egagrab.asm ega.inc grabmac.inc ..\include\vmdaega.inc \
	 grabpnt.inc ..\include\statusfl.inc \
	 ..\include\selectio.inc ..\include\int2fapi.inc \
	 ..\include\vmda.inc ..\include\vddgrb.inc \
	 ..\include\windows.inc wcmacros.inc

egagrfx.obj: egagrfx.asm ega.inc grabmac.inc ..\include\vmdaega.inc \
	 grabpnt.inc ..\include\statusfl.inc \
	 ..\include\selectio.inc \
	 ..\include\vmda.inc ..\include\vddgrb.inc \
	 ..\include\windows.inc wcmacros.inc

egamisc.obj: egamisc.asm ega.inc grabmac.inc ..\include\vmdaega.inc \
	 grabpnt.inc ..\include\statusfl.inc \
	 ..\include\selectio.inc ..\include\int2fapi.inc \
	 ..\include\vmda.inc ..\include\vddgrb.inc \
	 ..\include\windows.inc wcmacros.inc

egasel.obj: egasel.asm ega.inc grabmac.inc ..\include\vmdaega.inc \
	 grabpnt.inc ..\include\statusfl.inc \
	 ..\include\selectio.inc ..\include\int2fapi.inc \
	 ..\include\vmda.inc ..\include\vddgrb.inc \
	 ..\include\windows.inc wcmacros.inc

egatext.obj: egatext.asm ega.inc grabmac.inc ..\include\vmdaega.inc \
	 grabpnt.inc ..\include\statusfl.inc \
	 ..\include\selectio.inc \
	 ..\include\vmda.inc ..\include\vddgrb.inc \
	 ..\include\windows.inc wcmacros.inc

egautil.obj: egautil.asm ega.inc grabmac.inc ..\include\vmdaega.inc \
	 grabpnt.inc ..\include\statusfl.inc \
	 ..\include\selectio.inc \
	 ..\include\vmda.inc ..\include\vddgrb.inc \
	 ..\include\windows.inc wcmacros.inc

ega.3gr: ega.obj egagrfx.obj egatext.obj egautil.obj egasel.obj \
	egagrab.obj egamisc.obj grabber.def grbega.res
	link ega.obj egagrfx.obj egatext.obj egautil.obj egasel.obj \
	egagrab.obj egamisc.obj/MAP,ega.3gr,ega.map,libw /NOD,grabber.def
	rc /31 grbega.res ega.3gr
	copy ega.map grabber.map
	mapsym grabber
	copy grabber.sym ega.sym
	del grabber.map
	del grabber.sym
