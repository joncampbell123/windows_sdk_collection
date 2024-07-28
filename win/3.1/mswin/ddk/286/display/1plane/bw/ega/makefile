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

#   OPT =				    #NOP the options feature
OPT = -DPUBDEFS


#   Includes:
#
#	Define the include on up the display driver tree for this
#	particular directory.

INCS = -I. -I..\..\bitblt -I.. -I..\..



#   Define the default assemble command.  This command could actually
#   be overridden from the command line, but shouldn't be.

ASM = masm $(INCS) -ML -v $(OPT)				# MASM 4.01 & >
#   ASM = masm $(INCS) -ML -I\finc				# MASM 4.00



#   Define the default inference rules

.asm.obj:
	$(ASM) $**,$@;

#   Define the dependencies

OBJS:   pixel.obj cblt.obj egainit.obj output.obj scanlr.obj            \
        scanline.obj polyline.obj strblt.obj buildstr.obj smartfix.obj  \
        smartpro.obj ssb.obj 

ega:  $(OBJS)

pixel.obj:	pixel.asm

cblt.obj:	cblt.asm			\
		..\..\bitblt\genlocal.blt	\
		..\..\bitblt\genconst.blt	\
		..\..\bitblt\gendata.blt	\
		..\..\bitblt\ropdefs.blt	\
		..\..\bitblt\roptable.blt	\
			  ..\clrlocal.blt	\
			  ..\clrconst.blt	\
			  ..\clrdata.blt	\
			     devlocal.blt	\
			     devconst.blt	\
			     devdata.blt
	$(ASM) $*;

egainit.obj:	egainit.asm

output.obj:	output.asm

scanlr.obj:	scanlr.asm

scanline.obj:	scanline.asm

polyline.obj:	polyline.asm polyline.inc 
		$(ASM) $*;

strblt.obj:	strblt.asm   strblt.inc ..\..\fontseg.inc
		$(ASM) $*;

buildstr.obj:	buildstr.asm strblt.inc ..\..\fontseg.inc
		$(ASM) $*;

smartfix.obj:	smartfix.asm strblt.inc ..\..\fontseg.inc
		$(ASM) $*;

smartpro.obj:	smartpro.asm strblt.inc ..\..\fontseg.inc
		$(ASM) $*;

ssb.obj:	ssb.asm
		$(ASM) $*;




















					  
