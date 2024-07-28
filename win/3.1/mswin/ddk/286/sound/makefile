#
#  Standard command line definitions
#
as=masm -DROM=1

#
#  DOS 3.x inference rules
#
.asm.obj:
    $(as) $*.asm;

target:  sound.drv

sound.obj: sound.asm

mdd_core.obj: mdd_core.asm

mdd_beep.obj: mdd_beep.asm

sound.drv: sound.def sound.obj mdd_core.obj mdd_beep.obj
    link @sound.lnk
    rc -b sound.drv
    mapsym sound
