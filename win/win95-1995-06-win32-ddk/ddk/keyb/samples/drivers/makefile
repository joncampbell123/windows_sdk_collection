#****************************************************************************
#                                                                           *
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
# PURPOSE.                                                                  *
#                                                                           *
# Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
#                                                                           *
#****************************************************************************

#Makefile for Windows 95 keyboard driver.

# The following drivers are built:
#	KBD.DRV (==> KEYBOARD.DRV on release) for IBM-compatible systems.

INC=.

OBJS = trap.obj init.obj toascii.obj xlatus.obj enable.obj vkscan.obj \
    setspeed.obj tabs.obj oemscan.obj getname.obj wep.obj mapvirt.obj


OPT = -DROM=0

# define assembler command
ASM = masm $(OPT)

# define some dependencies for DATAxx.obj assembly
DATAINCS = keyboard.inc vkwin.inc vkoem.inc

all:  tables kbd.drv

tables:
        cd layout
        nmake
        copy kbdus.kbd ..
        cd..

####### Assemble object files.. ####################################

setspeed.obj: setspeed.asm keyboard.inc
        $(ASM) -I$(INC) setspeed;

# INIT() -- INIT code
init.obj: init.asm keyboard.inc
       $(ASM) -I$(INC) init;

trap.obj: trap.asm keyboard.inc vkwin.inc vkoem.inc
    $(ASM) -I.\ -I$(INC)  trap;

toascii.obj: toascii.asm  keyboard.inc vkoem.inc vkwin.inc
    $(ASM) -I.\ -I$(INC)  -DDEAD toascii;

oemscan.obj: oemscan.asm  keyboard.inc
    $(ASM) -I.\ -I$(INC)  -DDEAD oemscan;

vkscan.obj: vkscan.asm  keyboard.inc
    $(ASM) -I.\ -I$(INC)  -DDEAD vkscan;

mapvirt.obj: mapvirt.asm  keyboard.inc
    $(ASM) -I.\ -I$(INC)  -DDEAD mapvirt;

xlatus.obj: xlat.asm xlatus.inc
    $(ASM) -I.\ -I$(INC) xlat,xlatus.obj;

ENABLEOPTS =

enable.obj: enable.asm vkwin.inc vkoem.inc keyboard.inc equate.inc
        $(ASM)  -I.\  -I$(INC)  $(ENABLEOPTS) enable;

tabs.obj: tabs.asm vkwin.inc vkoem.inc 
        $(ASM)  -I.\ -Itables -I$(INC) tabs;

getname.obj: getname.asm
    $(ASM) -I.\ -Itables -I$(INC) getname;

wep.obj: wep.asm
    $(ASM) -I.\  -I$(INC) wep;

#### link driver versions ########################################

# ALL the drivers are first linked as 'keyboard.exe'.
# RC is then run to mark them as executables.
# Keyboard.exe is then renamed to KBDx.DRV.
# Mapsym is run on KEYBOARD.MAP, and KEYBOARD.SYM is renamed to
# KBDx.SYM.
# IBM-compatible (includes XT, AT, Enhanced) keyboard driver KBD.DRV.

kbd.drv: kbd.def keyboard.lnk $(OBJS)
    copy kbd.def keyboard.def
    link @keyboard.lnk
    rc -r -v kbd.rc
    rc kbd.res keyboard.exe
    -del kbd.drv
    ren keyboard.exe kbd.drv
    mapsym keyboard
    copy keyboard.sym kbd.sym
    copy keyboard.map kbd.map
    -del keyboard.def
    -del keyboard.map
    -del keyboard.sym
    -del kbdus.kbd
