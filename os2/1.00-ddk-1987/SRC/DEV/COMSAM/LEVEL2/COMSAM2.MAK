#
#**	makefile for OS/2 sample COM device driver
#
#	OS/2 DEVICE DRIVER KIT(DDK) VERSON
#

#	Definitions for assembler
ASM=masm
INC=    /I. /I..\..\..\DOS /I..\..\..\INC
AFLAGS= /Mx /t /P /z

#	Definitions for new style linker
LINK=link
LFLAG=/nod /noi /map
LIBD= $(LIB)\doscalls.lib


#	List of object files required

OBJS=	comsam2.obj 
DOBJS=	comsam2d.obj

comsam2.obj: comsam2.asm comdefs.inc com.inc ..\..\..\dos\error.inc \
	..\..\..\dos\devsym.inc ..\..\..\dos\devhdr.inc \
	..\..\..\dos\devhlp.inc
	$(ASM) $(AFLAGS) $(INC) comsam2.asm,comsam2.obj;

comsam2.sys: $(OBJS) $(LIBD) comsam2.def
	$(LINK) $(LFLAG) $(OBJS),comsam2.sys,comsam2.map,$(LIBD),comsam2.def;

comsam2d.obj: comsam2.asm comdefs.inc com.inc ..\..\..\dos\error.inc \
	..\..\..\dos\devsym.inc ..\..\..\dos\devhdr.inc \
	 ..\..\..\dos\devhlp.inc
	$(ASM) $(AFLAGS) $(INC) /DCOMDEBUG comsam2.asm,comsam2d.obj;

comsam2d.sys: $(DOBJS) $(LIBD) comsam2.def
	$(LINK) $(LFLAG) $(DOBJS),comsam2d.sys,comsam2d.map,$(LIBDD),comsam2.def;
	mapsym comsam2d
