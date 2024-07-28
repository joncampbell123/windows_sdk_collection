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
vga:  parent $(DEST)\vgaoem.obj $(DEST)\vgablock.obj $(DEST)\vgagrbit.obj \
             $(DEST)\bepaint.obj $(DEST)\blockdev.obj $(DEST)\devinit.obj \
             $(DEST)\edsave.obj $(DEST)\vgamisc.obj $(DEST)\gsmode.obj    \
             $(DEST)\setswap.obj $(DEST)\srfontf.obj $(DEST)\srscrf.obj   \
             $(DEST)\srscr.obj $(DEST)\vgetinfo.obj
 
parent:
			cd ..
			$(MAKE) AFLAGS2=$(AFLAGS2) -f grabbers
			cd vga


$(DEST)\vgaoem.obj:		vgaoem.asm 	\
			vgaabs0.inc	\
			vgaoem.inc
		$(AS) $(AFLAGS) $(AFLAGS2) vgaoem.asm,$(DEST)\;

$(DEST)\vgablock.obj:		vgablock.asm	\
			vgaic.inc
		$(AS) $(AFLAGS) $(AFLAGS2) vgablock.asm,$(DEST)\;

$(DEST)\vgetinfo.obj:		vgetinfo.asm	\
			vgaic.inc
		$(AS) $(AFLAGS) $(AFLAGS2) vgetinfo.asm,$(DEST)\;

$(DEST)\vgagrbit.obj:		vgagrbit.asm	\
			vgaic.inc
		$(AS) $(AFLAGS) $(AFLAGS2) vgagrbit.asm,$(DEST)\;

$(DEST)\bepaint.obj:		bepaint.asm	\
			dc.inc		\
			vga.inc 	
		$(AS) $(AFLAGS) $(AFLAGS2) bepaint.asm,$(DEST)\;

$(DEST)\blockdev.obj:		blockdev.asm	\
		        vgaic.inc
		$(AS) $(AFLAGS) $(AFLAGS2) blockdev.asm,$(DEST)\;

$(DEST)\devinit.obj:		devinit.asm	\
			vga.inc
		$(AS) $(AFLAGS) $(AFLAGS2) devinit.asm,$(DEST)\;

$(DEST)\edsave.obj:		edsave.asm	\
		        vgaabs0.inc
		$(AS) $(AFLAGS) $(AFLAGS2) edsave.asm,$(DEST)\;

$(DEST)\vgamisc.obj:		vgamisc.asm	\
		        vgaabs0.inc	\
			dc.inc		\
			vga.inc 	\
			fileio.inc
		$(AS) $(AFLAGS) $(AFLAGS2) vgamisc.asm,$(DEST)\;

$(DEST)\gsmode.obj:		gsmode.asm	\
		        vgaabs0.inc	\
		        vgaic.inc  	\
			dc.inc		\
			vga.inc 	\
			fileio.inc
		$(AS) $(AFLAGS) $(AFLAGS2) gsmode.asm,$(DEST)\;

$(DEST)\setswap.obj:		setswap.asm	\
			dc.inc
		$(AS) $(AFLAGS) $(AFLAGS2) setswap.asm,$(DEST)\;

$(DEST)\srfontf.obj:		srfontf.asm	\
			dc.inc		\
			vga.inc 	\
			fileio.inc
		$(AS) $(AFLAGS) $(AFLAGS2) srfontf.asm,$(DEST)\;

$(DEST)\srscrf.obj:		srscrf.asm	\
		        vgaic.inc 		\
			dc.inc		\
			vga.inc 	\
			fileio.inc
		$(AS) $(AFLAGS) $(AFLAGS2) srscrf.asm,$(DEST)\;

$(DEST)\srscr.obj:		srscr.asm	\
		        vgaoem.inc 	\
		        vgaabs0.inc	\
		     	vgaic.inc		\
			dc.inc		\
			vga.inc 	\
			fileio.inc
		$(AS) $(AFLAGS) $(AFLAGS2) srscr.asm,$(DEST)\;
