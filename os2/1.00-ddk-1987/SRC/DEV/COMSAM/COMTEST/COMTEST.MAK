#	OS/2 DEVICE DRIVER KIT(DDK) VERSION
#
#   Definitions

CFLAGS   = -Ox -Zep -I$(INCLUDE)
LIBD     = $(LIB)

comsubs.obj:	comsubs.asm
	masm $*.asm;

comtest.exe:    comtest.c comsubs.obj
        cl $(CFLAGS) $*.c comsubs.obj
	bind $*.exe $(LIBD)\api.lib $(LIBD)\doscalls.lib
