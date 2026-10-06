#
#**     makefile for OS/2 sample COM device driver
#
#       OS/2 DEVICE DRIVER KIT(DDK) VERSON
#

#       Definitions for assembler
ASM=masm
INC=    /I. /I..\..\..\DOS /I..\..\..\INC
AFLAGS= /Mx /t /P /z

#	Definitions for new style linker

LINK=link
LFLAGS4=/nod /noi /map
LIBD= $(LIB)\doscalls.lib

#       List of object files required


OBJS=   comsam1.obj 
DOBJS=  comsam1d.obj

comsam1.obj: comsam1.asm comdefs.inc com.inc ..\..\..\dos\error.inc \
	..\..\..\dos\devsym.inc ..\..\..\dos\devhdr.inc \
	..\..\..\dos\devhlp.inc
        $(ASM) $(AFLAGS) $(INC) comsam1.asm,comsam1.obj;

comsam1.sys: $(OBJS) $(LIBD) comsam1.def
	$(LINK) $(LFLAGS4) $(OBJS),comsam1.sys,,$(LIBD),comsam1.def;

comsam1d.obj: comsam1.asm comdefs.inc com.inc ..\..\..\dos\error.inc  \
	..\..\..\dos\devsym.inc ..\..\..\dos\devhdr.inc \
	..\..\..\dos\devhlp.inc
        $(ASM) $(AFLAGS) $(INC) /DCOMDEBUG comsam1.asm,comsam1d.obj;

comsam1d.sys: $(DOBJS) $(LIBD) comsam1.def
	$(LINK) $(LFLAGS4) $(DOBJS),comsam1d.sys,comsam1d.map,$(LIBD),comsam1.def;
        mapsym comsam1d
