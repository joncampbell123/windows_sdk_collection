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

INCS = -I. -I..



#   Define the default assemble command.  This command could actually
#   be overridden from the command line, but shouldn't be.

ASM = masm $(INCS) -v -ML $(OPT)				# MASM 4.01 & >
#   ASM = masm $(INCS) -v -ML -I\finc $(OPT)			# MASM 4.00


#   Define the default inference rules

.asm.obj:
	$(ASM) $**,$@;

bw:  robject.obj enum.obj colorinf.obj


#   Define the dependencies

robject.obj:	robject.asm

enum.obj:	enum.asm

colorinf.obj:	..\colorinf.asm
	$(ASM) $**,$@;
