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
AFLAGS2 	= -dEGAMONO
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
egamono:        parent egamono.2gr

parent:
                        cd ..
			$(MAKE) AFLAGS2=$(AFLAGS2) DEST=EGAMONO
			cd egamono

egamono.obj:		egamono.asm

egamono.2gr:		egamono.obj	  \
			egamono.lnk	  \
		        bepaint.obj	  \
		        blockdev.obj	  \
		        devinit.obj	  \
		        edsave.obj	  \
		        egamisc.obj	  \
		        gsmode.obj	  \
		        setswap.obj	  \
		        srfontf.obj	  \
		        srscr.obj	  \
		        srscrf.obj	  \
		  ..\..\bin2hex.obj	  \
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
