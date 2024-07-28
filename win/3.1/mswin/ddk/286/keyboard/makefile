#makefile for Windows 3.0 keyboard drivers.

# The following drivers are built:
#	KBD.DRV (==> KEYBOARD.DRV on release) for IBM-compatible systems.
#	KBDOLI.DRV for some Olivetti systems,  Amstrad (enhanced kbd only).
#	KBDHP.DRV for some H-P systems
#	[ KBDNOKIA.DRV for older Ericsson systems]. (commented out of make
#			right now).  Note: If this driver is built, add -DNOKIA
#			to assembly lines in tables makefile so that type
#			5 and 6 tables are included in keyboard DLL's.

INC=.

# define object file lists for dependency lists

# objects for IBM-compatible keyboard driver
OBJS = trap.obj init.obj toascii.obj xlatus.obj enable.obj \
    setspeed.obj tabs.obj oemscan.obj getname.obj wep.obj


# objects for Olivetti M24, M240, AT&T 6300, 6300+ keyboard driver
OBJSO = trapo.obj inito.obj toasciio.obj xlatus.obj enableo.obj \
	setspdo.obj tabso.obj oemscano.obj getname.obj wep.obj


# objects for Nokia keyboard driver.
#OBJSN = trapn.obj initn.obj toasciin.obj \
# xlatus.obj enablen.obj setspdo.obj tabsn.obj oemscan.obj getname.obj wep.obj

# objects for Hewlett-Packard keyboard driver.
OBJSHP = traph.obj init.obj toascii.obj \
   xlatus.obj enableh.obj setspeed.obj tabs.obj oemscan.obj getname.obj wep.obj

# define assembler options
!ifdef ROM
OPT = -DROM=1
!else
OPT = -DROM=0
!endif

# define assembler command
ASM = masm $(OPT)

# define some dependencies for DATAxx.obj assembly
DATAINCS = keyboard.inc vkwin.inc vkoem.inc

target:  kbd.drv kbdhp.drv tables

tables:
        cd tables
        $(MAKE)
        cd ..

####### Assemble object files.. ####################################

###### SETSPEED ####################################

# SetSpeed()
setspeed.obj: setspeed.asm keyboard.inc
        $(ASM) -I$(INC) setspeed;

# SetSpeed() for Olivetti M24 -- no speed setting
setspdo.obj: setspeed.asm keyboard.inc
        $(ASM) -I$(INC) -DICO setspeed, setspdo;



###### INIT ####################################

# INIT() -- INIT code
init.obj: init.asm keyboard.inc date.inc
        $(ASM) -I$(INC) init;

#init for Olivetti
inito.obj: init.asm keyboard.inc date.inc
        $(ASM) -I$(INC) -DICO init, inito;

#init for Nokia
#initn.obj: init.asm keyboard.inc date.inc
#        $(ASM) -I$(INC) -DNOKIA init, initn;


###### TRAP -- hardware int. service ####################################

# Some tables, and the hardware interrupt service routine keybd_int().
# Enhanced keyboard version
trap.obj: trap.asm keyboard.inc vkwin.inc vkoem.inc
    $(ASM) -I.\ -I$(INC)  -DENHANCE trap;

# Olivetti 'Ico' version for M24/6300 83 and 102-key keyboards.
trapo.obj: trap.asm keyboard.inc vkwin.inc vkoem.inc date.inc
    $(ASM) -I.\ -I$(INC)  -DICO trap, trapo;

# Nokia version of trap.
#trapn.obj: trap.asm keyboard.inc vkwin.inc vkoem.inc date.inc
#    $(ASM) -I.\ -I$(INC)  -DNOKIA trap, trapn;

# Hewlett-Packard version of trap.
traph.obj: trap.asm keyboard.inc vkwin.inc vkoem.inc equate.inc date.inc
    $(ASM) -I.\ -I$(INC)  -DENVOY trap, traph;


###### TOASCII ####################################

# TOASCII as used by most drivers -- has dead key code.
toascii.obj: toascii.asm  keyboard.inc vkoem.inc vkwin.inc
    $(ASM) -I.\ -I$(INC)  -DDEAD toascii;

# TOASCII for Olivetti/AT&T M24/6300.
toasciio.obj: toascii.asm  keyboard.inc vkoem.inc vkwin.inc
    $(ASM) -I.\ -I$(INC)  -DDEAD -DICO toascii, toasciio;

# TOASCII for Nokia
#toasciin.obj: toascii.asm  keyboard.inc vkoem.inc vkwin.inc
#    $(ASM) -I.\ -I$(INC)  -DDEAD -DNOKIA toascii, toasciin;


###### OEMSCAN ####################################

# OemKeyScan()
oemscan.obj: oemscan.asm  keyboard.inc
    $(ASM) -I.\ -I$(INC)  -DDEAD oemscan;

# Dummy OemKeyScan() for Olivetti
oemscano.obj: oemscan.asm  keyboard.inc
    $(ASM) -I.\ -I$(INC)  -DDEAD -DICO oemscan, oemscano;

###### XLAT ####################################

# OemToAnsi(), AnsiToOem()
xlatus.obj: xlat.asm xlatus.inc
    $(ASM) -I.\ -I$(INC) -DEXTENDEDANSI xlat,xlatus.obj;

###### ENABLE ####################################

# common ifdef options for Enable.  NEWNMI added 30jan90
# NEWNMI removed 04feb90
ENABLEOPTS =

# Enable(), Disable(), Inquire(), ScreenSwitchEnable(),
enable.obj: enable.asm vkwin.inc vkoem.inc keyboard.inc equate.inc
        $(ASM)  -I.\  -I$(INC)  $(ENABLEOPTS) enable;

#Enable for Olivetti M24: SetOliLights()
enableo.obj: enable.asm vkwin.inc vkoem.inc keyboard.inc equate.inc
        $(ASM)  -I.\  -I$(INC)  -DICO $(ENABLEOPTS) enable, enableo;

# Enable for Hewlett-Packard: HPSysCall(), ExchParent()
enableh.obj: enable.asm vkwin.inc vkoem.inc keyboard.inc equate.inc
        $(ASM)  -I.\ -I$(INC)  -DENVOY $(ENABLEOPTS) enable, enableh;

# Enable for Nokia
#enablen.obj: enable.asm vkwin.inc vkoem.inc keyboard.inc equate.inc
#        $(ASM)  -I.\ -I$(INC)  -DNOKIA $(ENABLEOPTS) enable, enablen;


###### TABS ####################################

# Translation tables
tabs.obj: tabs.asm vkwin.inc vkoem.inc tables\trans.inc tab4.inc
        $(ASM)  -I.\ -Itables -I$(INC) -DENHANCE tabs;

# Olivetti -- has different default KeyType value
tabso.obj: tabs.asm vkwin.inc vkoem.inc tables\trans.inc tab4.inc
        $(ASM)  -I.\ -Itables -I$(INC) -DICO tabs, tabso;

# Translation tables for Nokia -- handles extended scancode translation
#tabsn.obj: tabs.asm vkwin.inc vkoem.inc tables\trans.inc tab4.inc
#        $(ASM)  -I.\ -Itables -I$(INC) -DNOKIA tabs, tabsn;

###### GETNAME -- GetKeyNameText() ####################################

getname.obj: getname.asm
    $(ASM) -I.\ -Itables -I$(INC) getname;

#### WEP -- Wep() ########################################

wep.obj: wep.asm
    $(ASM) -I.\  -I$(INC) wep;

#### link driver versions ########################################

# ALL the drivers are first linked as 'keyboard.exe'.
# RC is then run to mark them as 3.0 executables.
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

# Olivetti M24 & AT&T 6300 driver KBDOLI.DRV

kbdoli.drv: kbdoli.def kbdoli.lnk $(OBJSO)
    copy kbdoli.def keyboard.def
    link @kbdoli.lnk
    rc -r -v kbdoli.rc
    rc kbdoli.res keyboard.exe
    -del kbdoli.drv
    ren keyboard.exe kbdoli.drv
    mapsym keyboard
    copy keyboard.sym kbdoli.sym
    copy keyboard.map kbdoli.map
    -del keyboard.def
    -del keyboard.map
    -del keyboard.sym

# Hewlett-Packard Vectra, handles Envoy keyboard (KBDHP.DRV)

kbdhp.drv: kbdhp.def kbdhp.lnk $(OBJSHP)
    copy kbdhp.def keyboard.def
    link @kbdhp.lnk
    rc -r -v kbdhp.rc
    rc kbdhp.res keyboard.exe
    -del kbdhp.drv
    ren keyboard.exe kbdhp.drv
    mapsym keyboard
    copy keyboard.sym kbdhp.sym
    copy keyboard.map kbdhp.map
    -del keyboard.def
    -del keyboard.map
    -del keyboard.sym
