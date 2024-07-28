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
AFLAGS		= -i.\ -i..\ -i..\..\ -v
AFLAGS2 	=
CFLAGS		=
LFLAGS		=
MFLAGS		=


#
# Define environment dependent and miscellaneous macros
#
LIBS		=
OBJECTS 	=
DEST            =.


#
# Define dependencies
#

ega:  parent $(DEST)\bepaint.obj $(DEST)\blockdev.obj $(DEST)\devinit.obj \
             $(DEST)\edsave.obj $(DEST)\egamisc.obj $(DEST)\gsmode.obj    \
             $(DEST)\setswap.obj $(DEST)\srfontf.obj $(DEST)\srscr.obj    \
             $(DEST)\srscrf.obj

parent:
			cd ..
			$(MAKE) AFLAGS2=$(AFLAGS2) -f grabbers
			cd ega

$(DEST)\bepaint.obj:		bepaint.asm
		$(AS) $(AFLAGS) $(AFLAGS2) bepaint.asm,$(DEST)\;

$(DEST)\blockdev.obj:		blockdev.asm
		$(AS) $(AFLAGS) $(AFLAGS2) blockdev.asm,$(DEST)\;

$(DEST)\devinit.obj:		devinit.asm
		$(AS) $(AFLAGS) $(AFLAGS2) devinit.asm,$(DEST)\;

$(DEST)\edsave.obj:		edsave.asm
		$(AS) $(AFLAGS) $(AFLAGS2) edsave.asm,$(DEST)\;

$(DEST)\egamisc.obj:		egamisc.asm
		$(AS) $(AFLAGS) $(AFLAGS2) egamisc.asm,$(DEST)\;

$(DEST)\gsmode.obj:		gsmode.asm
		$(AS) $(AFLAGS) $(AFLAGS2) gsmode.asm,$(DEST)\;

$(DEST)\setswap.obj:		setswap.asm
		$(AS) $(AFLAGS) $(AFLAGS2) setswap.asm,$(DEST)\;

$(DEST)\srfontf.obj:		srfontf.asm
		$(AS) $(AFLAGS) $(AFLAGS2) srfontf.asm,$(DEST)\;

$(DEST)\srscr.obj:		srscr.asm
		$(AS) $(AFLAGS) $(AFLAGS2) srscr.asm,$(DEST)\;

$(DEST)\srscrf.obj:		srscrf.asm
		$(AS) $(AFLAGS) $(AFLAGS2) srscrf.asm,$(DEST)\;

