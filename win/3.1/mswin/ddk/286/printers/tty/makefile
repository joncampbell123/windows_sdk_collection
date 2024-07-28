# TTY: makefile for TTY.DRV

###########################################################################
# Fralc Consultores (C), Mexico 1986-1989
# Copyright (C) 1989-1990, Microsoft Corporation. All rights reserved.
###########################################################################
# Microsoft history (latest first)
# 13 nov 89 peterbe     Separate out ttyhelp makefile
# 06 nov 89 peterbe     Add copyrights
# 23 oct 89 timg286     compile the helpfile too
# 22 oct 89 peterbe     added .H file dependencies
# 20 oct 89 peterbe     checked in, added VERSION.H to TTY.RES
# 13 nov 91   percyt        renamed VERSION.H to VERSTR.H
###########################################################################

#International mods
#NOTE: LANG is an external macros set for international builds
!IFNDEF LANG
RES_DIR=.\messages\usa
!ELSE
RES_DIR=.\messages\$(LANG)
!ENDIF

DEB=

target:  tty.drv

# International mods
!IFNDEF LANG
tty.res:  $(RES_DIR)\tty.rc $(RES_DIR)\tty.rcv ttyres.h verstr.h
    rc -r -e  -Fotty.res $(RES_DIR)\tty.rc
!ELSE
tty.res: $(RES_DIR)\$@
    copy $(RES_DIR)\$@
!ENDIF

reset.obj:   reset.c generic.h defs.h tty.h ttyres.h
    cl -u -c -Asnw -FPa -PLM -Gsw -Os -Zped -I. -NT _TEXT reset.c

chphys.obj:     chphys.c generic.h tty.h
    cl -u -c -Alnw -FPa -PLM -Gsw -Os -Zped -I. -NT _CHAR chphys.c

control.obj:    control.c tty.h generic.h
    cl -u -c -Asnw -FPa -PLM -Gsw -Os -Zped -I. -NT _TEXT control.c

data.obj: data.c tty.h generic.h
    cl -u -c -Asnw -FPa -PLM -Gsw -Os -Zpedi -I. -NT _CHAR data.c

devmode.obj:    devmode.c tty.h ttyres.h generic.h \
        defs.h
    cl -u -c -Asnw -FPa -PLM -Gsw -Os -Zped -I. -NT _DMOD devmode.c

dials.obj:    dials.c tty.h ttyres.h file.h \
    generic.h defs.h ttyhelp.h
    cl -u -c -Asnw -FPa -PLM -Gsw -Os -Zped -I. -NT _DMOD dials.c

devcaps.obj:    devcaps.c tty.h generic.h
    cl -u -c -Asnw -FPa -PLM -Gsw -Os -Zped -I. -NT _CAPS devcaps.c

file.obj:   file.c tty.h file.h generic.h ttyres.h
    cl -u -c -Asnw -FPa -PLM -Gsw -Os -Zped -I. -NT _FILE file.c

tty.obj:    tty.asm
    masm -ZI -I. tty.asm;

physical.obj:     physical.c tty.h generic.h ttyres.h
    cl -u -c -Asnw -FPa -PLM -Gsw -Od -Zped -I. -NT _TEXT physical.c

realize.obj:  realize.c  tty.h fonts.h file.h generic.h fonts.h defs.h
    cl -u -c -Alnw -FPa -PLM -Gsw -Od -Zped -I. -NT _CHAR realize.c

stubs.obj:     stubs.c tty.h generic.h
    cl -u -c -Asnw -FPa -PLM -Gsw -Od -Zped  -I. -NT _TEXT stubs.c

# debug library.
debug.obj: debug.c \
    debug.h
    cl -W2 -u -c -Asnw -PLM -Gsw -Oas -Zpe  -I. debug.c
    lib debug-+debug.obj;

tty.exe:  tty.lnk tty.def tty.obj devmode.obj dials.obj\
        physical.obj stubs.obj control.obj reset.obj realize.obj \
        chphys.obj data.obj file.obj devcaps.obj
    link $(DEB) @tty.lnk
    mapsym tty

tty.drv:   iclean tty.exe tty.res
    rc -e tty.res
    copy tty.exe tty.drv

iclean:
    del *.res
