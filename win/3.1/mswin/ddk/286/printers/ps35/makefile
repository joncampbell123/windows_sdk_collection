#----------------------------------------------------------------------------
# makefile to build PSCRIPT.DRV PostScript printer driver
#
# chrisg, 88-dec-03, under sanity rebuild
#
#----------------------------------------------------------------------------

!ifdef CVW
DEF  = -DPS_IGNORE  
OPTS = -c -W3 -u -Asnw -G2sw -Od -Zipd $(DEF)	# -Zdip for debugging
CC   = cl $(OPTS)
RES  = .\res
LINK = link /NOE/CO/LI/MAP
MASM = masm /Zi
!else
DEF  = -DPS_IGNORE  
OPTS = -c -W3 -u -Asnw -G2sw -Oseal -Zp $(DEF)
CC   = cl $(OPTS)
RES  = .\res
LINK = link /NOE/LI/MAP
MASM = masm 
!endif

!ifdef T1SNOOP
DEF  = $(DEF) -DT1SNOOP
!endif

.c.obj:
	$(CC) $*.c

.asm.obj:
	$(MASM) $*;

target:  iclean utils res pscript.drv

utils:
        cd utils
        $(MAKE)
        cd ..

res:
        cd res
	$(MAKE)
        cd ..

adobe.obj : adobe.c adobe.h  channel.h \
	 debug.h getdata.h paper.h printers.h pscript.h psdata.h \
	 printcap.h utils.h
	$(CC) -NT _ADOBE adobe.c

atstuff.obj : atstuff.c atprocs.h atstuff.h \
	  debug.h paper.h pscript.h psdata.h \
	 pserrors.h resource.h utils.h
	$(CC) -NT _AT atstuff.c

bitblt.obj : bitblt.c atstuff.h channel.h debug.h devmode.h driver.h \
	   gdidm.h paper.h printers.h pscript.h \
	 printcap.h utils.h
	$(CC) -NT _BITBLT bitblt.c

channel.obj : channel.c atprocs.h atstuff.h \
	  channel.h debug.h paper.h pscript.h \
	 spooler.h utils.h
	$(CC) -NT _IO channel.c

charwdth.obj : charwdth.c debug.h devmode.h driver.h   \
	 etm.h fonts.h paper.h printers.h pscript.h  \
	 printcap.h utils.h truetype.h
	$(CC) -NT _CHARWDTH charwdth.c

control.obj : control.c atprocs.h atstuff.h \
	  channel.h control2.h debug.h devmode.h \
	 driver.h   escapes.h etm.h exttext.h getdata.h paper.h \
	 printers.h profile.h pscript.h psdata.h pserrors.h pss.h resource.h \
	 printcap.h spooler.h strchblt.h text.h utils.h truetype.h
	$(CC) -NT _CNTRL control.c

control2.obj : control2.c  control2.h \
	 debug.h defaults.h devmode.h dmrc.h driver.h   \
	 getdata.h paper.h printers.h profile.h pscript.h psdata.h resource.h \
	 printcap.h utils.h
	$(CC) -NT _CNTRL2 control2.c

devcap.obj : devcap.c control2.h debug.h defaults.h devmode.h \
	 driver.h   getdata.h paper.h printers.h profile.h \
	 pscript.h psdata.h   printcap.h utils.h
	$(CC) -NT _DEVCAP $*.c

devmode.obj : devmode.c  control2.h psver.h \
	 debug.h devmode.h dmrc.h driver.h   getdata.h paper.h \
	 printers.h profile.h pscript.h printcap.h utils.h
	$(CC) -NT _DEVMODE $*.c

dialog.obj : dialog.c atprocs.h atstuff.h  \
	 debug.h devmode.h dmrc.h driver.h   getdata.h paper.h \
	 printers.h profile.h pscript.h psdata.h pserrors.h psoption.h \
	 resource.h printcap.h utils.h
	$(CC) -NT _DLG0 dialog.c

dibtodev.obj : dibtodev.c channel.h debug.h devmode.h driver.h \
	   getdata.h paper.h printers.h pscript.h resource.h \
	    \
	 printcap.h utils.h
	$(CC) -NT _DIB dibtodev.c

disable.obj : disable.c devmode.h driver.h   etm.h fonts.h \
	 paper.h printers.h pscript.h printcap.h
	$(CC) -NT _DSBL disable.c

enable.obj : enable.c  control2.h debug.h psver.h \
	 defaults.h devmode.h dmrc.h driver.h   etm.h fonts.h \
	 getdata.h paper.h printers.h profile.h pscript.h psdata.h resource.h \
	 printcap.h utils.h
	$(CC) -NT _ENBL enable.c

enum.obj : enum.c  debug.h enum.h etm.h truetype.h \
	 fonts.h paper.h pscript.h psdata.h resource.h utils.h
	$(CC) -NT _ENUM enum.c

exttext.obj : exttext.c  debug.h etm.h truetype.h \
	 exttext.h fonts.h paper.h pscript.h utils.h 
	$(CC) -NT _XSEG exttext.c

fonts.obj : fonts.c  debug.h devmode.h  truetype.h \
	   etm.h fonts.h paper.h printers.h profile.h pscript.h \
	 psdata.h resource.h printcap.h utils.h
	$(CC) -NT _FONTS fonts.c

getdata.obj : getdata.c  channel.h debug.h \
	 devmode.h   getdata.h paper.h printers.h profile.h \
	 pscript.h psdata.h resource.h printcap.h
	$(CC) -NT _GETDATA getdata.c

graph.obj : graph.c  channel.h debug.h \
	 devmode.h driver.h   gdidm.h paper.h printers.h \
	 pscript.h printcap.h utils.h
	$(CC) -NT _GSEG graph.c

kern.obj : kern.c etm.h kern.h paper.h pscript.h utils.h
	$(CC) -NT _KERN kern.c

mgesc.obj : mgesc.c mgesc.h
	$(CC) -NT _MGESC mgesc.c

profile.obj : profile.c  debug.h psver.h \
	 defaults.h devmode.h dmrc.h   getdata.h paper.h \
	 printers.h profile.h pscript.h psdata.h pserrors.h resource.h \
	 printcap.h utils.h
	$(CC) -NT _PROFILE profile.c

pserrors.obj : pserrors.c debug.h paper.h pscript.h psdata.h pserrors.h
	$(CC) -NT _ERROR pserrors.c

psinst.obj : psinst.c dmrc.h psdata.h resource.h
	$(CC) -NT _PSINST psinst.c

psoption.obj : psoption.c atprocs.h atstuff.h \
	  channel.h debug.h devmode.h dmrc.h \
	   getdata.h paper.h printers.h pscript.h psdata.h \
	 pserrors.h psoption.h resource.h  \
	   printcap.h utils.h
	$(CC) -NT _DLG1 psoption.c

#  psprompt.obj : psprompt.c paper.h pscript.h psdata.h \
#	   psprompt.h
#	$(CC) -NT _ERROR psprompt.c

pst1enc.obj : pst1enc.c pst1enc.h truetype.h
	$(CC) -NT _TEXT pst1enc.c

realize.obj : realize.c debug.h devmode.h driver.h   \
	 enum.h gdidm.h paper.h printers.h pscript.h  \
	   printcap.h utils.h truetype.h
	$(CC) -NT _RLZE realize.c

strchblt.obj : strchblt.c atstuff.h channel.h debug.h \
	 paper.h pscript.h strchblt.h utils.h
	$(CC) -NT _STRCHBLT strchblt.c

strlist.obj : strlist.c strlist.h
	$(CC) -NT _FONTS strlist.c

stubs.obj : stubs.c devmode.h driver.h   paper.h \
	 printers.h pscript.h printcap.h
	$(CC) -NT _STUBS stubs.c

text.obj : text.c adobe.h  channel.h debug.h \
	 devmode.h driver.h   etm.h fonts.h gdidm.h kern.h \
	 paper.h printers.h pscript.h printcap.h text.h utils.h truetype.h
	$(CC) -NT _TSEG text.c

ttfont.obj : ttfont.c pscript.h channel.h etm.h truetype.h resource.h \
         printers.h getdata.h utils.h pst1enc.h adobe.h
	$(CC) -NT _TEXT ttfont.c

utils.obj : utils.c atstuff.h  channel.h \
	 debug.h paper.h pscript.h psdata.h resource.h utils.h
        $(CC) -NT _TEXT utils.c

#---------------------------------------------------------------------------
# asm stuff
#

lmemcpy.obj: lmemcpy.asm

heap.obj: heap.asm
	masm -v heap.asm;

muldiv.obj: muldiv.asm

scale.obj: scale.asm
	masm -v scale.asm;

trig.obj: trig.asm vttable.h
	masm -v trig.asm;


# If the SIMULATE flag in TEXT.c is true remember to add the
# simulate.obj entry below.
#
pscript.drv: pscript.lnk pscript.def getdata.obj realize.obj \
 utils.obj text.obj bitblt.obj profile.obj enable.obj kern.obj \
 channel.obj dialog.obj graph.obj heap.obj scale.obj disable.obj \
 stubs.obj trig.obj control.obj control2.obj exttext.obj fonts.obj enum.obj \
 adobe.obj pserrors.obj psoption.obj charwdth.obj \
 strchblt.obj dibtodev.obj devmode.obj devcap.obj mgesc.obj \
 psinst.obj strlist.obj lmemcpy.obj ttfont.obj pst1enc.obj muldiv.obj
	$(LINK) @pscript.lnk
        copy $(RES)\pscript.res
	rc pscript.res pscript.drv
	mapsym /n pscript.map

iclean:
    del $(RES)\*.res
    del pscript.drv
