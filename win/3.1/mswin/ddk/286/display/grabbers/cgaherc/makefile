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
# Define default inference rules
#


#
# Define dependencies
#
cgaherc:  parent $(DEST)\bepaint.obj $(DEST)\blockdev.obj $(DEST)\edsave.obj \
          $(DEST)\gsmode.obj $(DEST)\setswap.obj $(DEST)\srscr.obj

parent:
			cd ..
			$(MAKE) AFLAGS2=$(AFLAGS2) -f grabbers
			cd cgaherc

$(DEST)\bepaint.obj:		bepaint.asm
		$(AS) $(AFLAGS) $(AFLAGS2) bepaint.asm,$(DEST)\;

$(DEST)\blockdev.obj:		blockdev.asm
		$(AS) $(AFLAGS) $(AFLAGS2) blockdev.asm,$(DEST)\;

$(DEST)\edsave.obj:		edsave.asm
		$(AS) $(AFLAGS) $(AFLAGS2) edsave.asm,$(DEST)\;

$(DEST)\gsmode.obj:		gsmode.asm
		$(AS) $(AFLAGS) $(AFLAGS2) gsmode.asm,$(DEST)\;

$(DEST)\setswap.obj:		setswap.asm
		$(AS) $(AFLAGS) $(AFLAGS2) setswap.asm,$(DEST)\;

$(DEST)\srscr.obj:		srscr.asm
		$(AS) $(AFLAGS) $(AFLAGS2) srscr.asm,$(DEST)\;

