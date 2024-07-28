#   Window's Sample Display Drivers.
#
#   Requirements:
#
#	MASM 4.01 or greater with the environment variable INCLUDE set to
#	the directories containing CMACROS.INC, GDIDEFS.INC, and WINDEFS.INC.
#
#	MASM 4.00 or greater with the ASM inference definition changed to
#	include the directories containing CMACROS.INC, GDIDEFS.INC, and
#	WINDEFS.INC.



#   Options:
#
#	The command line may define options to MASM by defining the OPT
#	macro.	By defining the OPT parameter in the make file, any
#	possible interaction with an environment definition is avoided.

OPT =				    #NOP the options feature


#   Includes:
#
#	Define the include on up the display driver tree for this
#	particular directory.

INCS = -I.\


#   Resolution is used to define if hires or lores dithering should take
#   Place.  It will be define when needed.


RES =


#   Define the default assemble command.  This command could actually
#   be overridden from the command line, but shouldn't be.

ASM = masm $(INCS) -v -ML  $(OPT)				# MASM 4.01 & >
#   ASM = masm $(INCS) -v -ML  $(OPT) -I\finc			# MASM 4.00



#   Define the default inference rules

.asm.obj:
        $(ASM) $**,$@;

OBJS=   inquire.obj setmode.obj disable.obj color.obj blkwhite.obj      \
        chkstk.obj cursors.obj control.obj fb.obj charwdth.obj          \
        lodither.obj hidither.obj sswitch.obj enable.obj

1plane:  $(OBJS)

#   Define the dependancies

inquire.obj:	inquire.asm

setmode.obj:	setmode.asm

enable.obj:	enable.asm

disable.obj:	disable.asm

color.obj:	.\color.asm

blkwhite.obj:	blkwhite.asm

chkstk.obj:	chkstk.asm

cursors.obj:	cursors.asm

control.obj:	control.asm

fb.obj: 	fb.asm

charwdth.obj:	charwdth.asm fontseg.inc
	$(ASM) $*;

lodither.obj:	dither.asm
	$(ASM) -DLORES $**,$@;

hidither.obj:	dither.asm
	$(ASM) -DHIRES $**,$@;

sswitch.obj:	sswitch.asm
