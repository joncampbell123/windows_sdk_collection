# International mods
#NOTE: LANG is an external macros set by international
!IFNDEF LANG
RES_DIR=.\messages\usa
!ELSE
RES_DIR=.\messages\$(LANG)
!ENDIF

device: msnet.drv

libinit.obj:    libinit.asm
    masm libinit;

msnetdev.obj: msnetdev.asm
    masm msnetdev;

wnetcap.obj:   wnetcap.asm wnet.inc
    masm wnetcap;

wnetuser.obj:   wnetuser.asm
    masm wnetuser;

wnetcons.obj:   wnetcons.asm
    masm wnetcons;

wnetprnt.obj:   wnetprnt.asm
    masm wnetprnt;

msnet.obj:  msnet.asm
    masm msnet;

netiface.obj:      netiface.asm
    masm netiface;

utils.obj:  utils.asm
    masm utils;

wnetpoll.obj:   wnetpoll.asm
    masm wnetpoll;

wnetlock.obj: wnetlock.asm
    masm wnetlock;

wnetprnq.obj: wnetprnq.asm
    masm wnetprnq;

stubs.obj:  stubs.asm
    masm stubs;

# International mods
!IFNDEF LANG
msnet.res: $(RES_DIR)\msnet.rc
    rc -r -Fomsnet.res $(RES_DIR)\msnet.rc
!ELSE
msnet.res: $(RES_DIR)\msnet.res
    copy $(RES_DIR)\msnet.res
!ENDIF

msnet.drv: iclean \
        libinit.obj msnetdev.obj wnetcap.obj stubs.obj      \
        wnetuser.obj wnetcons.obj wnetprnt.obj                  \
        msnet.obj netiface.obj utils.obj wnetpoll.obj           \
        wnetlock.obj wnetprnq.obj msnet.res msnet.def msnet.lnk
    link /NOD @msnet.lnk
    rc msnet.res msnet.drv
    mapsym msnet

# International mods
iclean:
    del *.res
