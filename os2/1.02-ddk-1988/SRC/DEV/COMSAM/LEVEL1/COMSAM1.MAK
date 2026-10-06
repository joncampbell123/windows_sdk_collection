#
# makefile for MS OS/2 sample COM device driver
#
# Copyright (c) Microsoft Corporation,  1988
#
# Definitions for assembler


# 5.1 Tools
LINK=link
ASM=masm
INC=..\..\..\inc
LIB=..\..\..\lib
AFLAGS= -Mx -t -P -I. -I$(INC)
LFLAGS4=/nod /noi /map
LIBS4= $(LIB)\OS2.LIB

#	List of object files required

OBJS=	comsam1.obj 
DOBJS=	comsam1d.obj

comsam1.obj: comsam1.asm comdefs.inc com.inc $(INC)\error.inc \
	devsym.inc $(INC)\devhdr.inc $(INC)\devhlp.inc 
	$(ASM) $(AFLAGS) comsam1.asm,comsam1.obj;

comsam1.sys: $(OBJS) $(LIBS4) comsam1.def
	$(LINK) $(LFLAGS4) $(OBJS),comsam1.sys,,$(LIBS4),comsam1.def;

comsam1d.obj: comsam1.asm comdefs.inc com.inc $(INC)\error.inc \
	devsym.inc $(INC)\devhdr.inc $(INC)\devhlp.inc 
	$(ASM) $(AFLAGS) -DCOMDEBUG comsam1.asm,comsam1d.obj;

comsam1d.sys: $(DOBJS) $(LIBS4) comsam1.def
	$(LINK) $(LFLAGS4) $(DOBJS),comsam1d.sys,comsam1d.map,$(LIBS4),comsam1.def;
	mapsym comsam1d

