!ifdef CVW
OPTS = -c -W3 -u -Alnw -G2sw -Od -Zipd -Fc
LINK = link /NOD/NOE/CO/LI/MAP
MASM = ml -c -Zi -DMASM6
!else
OPTS = -c -W3 -u -Alnw -G2sw -Oasie -Zp 
LINK = link /NOD/NOE/LI/MAP
MASM = ml -c -DMASM6
!endif

CC   = cl $(OPTS)

default: hpplot.drv

init.obj: init.asm
    $(MASM) -I$(SDKINC16DIR) -D?MLIBW init.asm

build.obj: build.c build.h fill.h
    $(CC) -NT _OUTPUT build.c

color.obj: color.c color.h device.h driver.h object.h
    $(CC) -NT _COLOR color.c

control.obj: control.c control.h device.h driver.h
    $(CC) -NT _CONTROL control.c

data.obj: data.c device.h dialog.h driver.h data.h
    $(CC) -NT _STATDATA data.c

dialog.obj: dialog.c device.h dialog.h driver.h
    $(CC) -NT _DIALOG dialog.c

devmode.obj: devmode.c device.h dialog.h driver.h
    $(CC) -NT _DEVMODE devmode.c

devcaps.obj: devcaps.c device.h dialog.h driver.h
        $(CC)  -NT _DEVMODE devcaps.c

driver.obj: driver.c color.h control.h device.h dialog.h driver.h \
            object.h output.h print.h text.h
    $(CC) -NT _DRIVER driver.c

fill.obj: fill.c fill.h device.h build.h
    $(CC) -NT _OUTPUT fill.c

glib.obj: glib.asm
    $(MASM) -I$(SDKINC16DIR) -D?MLIBW glib.asm

initlib.obj: initlib.c initlib.h
    $(CC) -NT _INITLIB initlib.c

install.obj: install.c 
    $(CC) -NT _INSTALL install.c

object.obj:  object.c device.h driver.h object.h
    $(CC) -NT _OBJECT object.c

output.obj: output.c device.h dialog.h driver.h fill.h output.h
    $(CC) -NT _OUTPUT output.c

print.obj:  print.c device.h driver.h print.h
    $(CC) -NT _PRINT print.c

profile.obj:profile.c device.h dialog.h driver.h profile.h
    $(CC) -NT _PROFILE profile.c

text.obj:   text.c device.h driver.h control.h output.h print.h text.h
    $(CC) -NT _TEXT text.c

utils.obj:  utils.c utils.h 
    $(CC) -NT _UTILS utils.c

hpplot.res: hpplot.rc hpplot.rcv hpplot.dlg
    rc -r hpplot.rc

hpplot.drv:  driver.def utils.obj devmode.obj data.obj devcaps.obj \
             build.obj color.obj control.obj dialog.obj driver.obj \
             fill.obj glib.obj hpplot.res init.obj initlib.obj install.obj \
             object.obj output.obj profile.obj print.obj text.obj 
    $(LINK) @driver.lnk
    mapsym hpplot
    rc hpplot.res hpplot.exe
    copy hpplot.exe hpplot.drv
