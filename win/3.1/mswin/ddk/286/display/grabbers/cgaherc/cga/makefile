#
# Define alias macros for code generators
#
AS		= masm
CC		= cc
EXE2BIN 	= exe2bin
LINK		= link
MAPSYM		= mapsym


#
# Define flag macros
#
AFLAGS		= -i.\ -i..\ -i..\..\ -i..\..\..\ -v
AFLAGS2 	= -dCGA
CFLAGS		=
LFLAGS		=
MFLAGS		=


#
# Define environment dependent and miscellaneous macros
#
LIBS		=
OBJECTS 	=


#
# Define default inference rules
#
.asm.obj:
		$(AS) $(AFLAGS) $(AFLAGS2) $*;

.obj.2gr:
		$(LINK) $(LFLAGS) @$*.lnk
		$(EXE2BIN) $*.2gr
                copy $*.bin $*.2gr
                del $*.bin


#
# define dependencies
#

cga:  parent cga.2gr

parent:
                        cd ..
			$(MAKE) AFLAGS2=$(AFLAGS2) DEST=CGA
			cd cga

cga.obj:		cga.asm

cga.2gr:		cga.obj 	  \
			cga.lnk 	  \
		        bepaint.obj	  \
		        blockdev.obj	  \
		        edsave.obj	  \
		        gsmode.obj	  \
		        setswap.obj	  \
		        srscr.obj	  \
		  ..\..\block.obj	  \
		  ..\..\edgrab.obj	  \
		  ..\..\entry.obj	  \
		  ..\..\getinfo.obj	  \
		  ..\..\grabbits.obj	  \
		  ..\..\grabscr.obj	  \
		  ..\..\inout.obj	  \
		  ..\..\inquire.obj	  \
		  ..\..\oem.obj 	  \
		  ..\..\prompt.obj	  \
		  ..\..\rcolor.obj
