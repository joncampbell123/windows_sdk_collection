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
AFLAGS2 	= -dVGACOLOR
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
# Define dependencies
#

vgacolor:  parent vgacolor.2gr

parent:
                        cd ..
			$(MAKE) AFLAGS2=$(AFLAGS2) DEST=VGACOLOR
			cd vgacolor

vgacolor.obj:		vgacolor.asm	\
		     ..\vgaoem.inc 	\
		     ..\vgaabs0.inc	\
		     ..\vgaic.inc      	\
		     ..\dc.inc		\
		     ..\vga.inc

vgacolor.2gr:		vgacolor.obj	\
		  ..\..\bin2hex.obj	\
		       vgablock.obj	\
		  ..\..\edgrab.obj	\
		  ..\..\entry.obj	\
		       vgetinfo.obj	\
		       vgagrbit.obj	\
		  ..\..\grabscr.obj	\
		  ..\..\inout.obj	\
		  ..\..\inquire.obj	\
		       vgaoem.obj 	\
		  ..\..\prompt.obj	\
		  ..\..\rcolor.obj	\
		       bepaint.obj	\
		       blockdev.obj	\
		       devinit.obj	\
		       edsave.obj	\
		       vgamisc.obj	\
		       gsmode.obj	\
		       setswap.obj	\
		       srfontf.obj	\
		       srscrf.obj	\
		       srscr.obj	\
		       vgacolor.lnk
