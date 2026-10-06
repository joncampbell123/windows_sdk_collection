#
# Copyright (c) Microsoft Corporation,  1988
#
#
#**	makefile for MS OS/2 sample COM device driver


# 5.1 Tools
ASM=masm
LINK=link
INC=..\..\..\inc
LIB=..\..\..\lib
AFLAGS= -Mx -t -P -z -I. -I$(INC)
LFLAGS4=/nod /noi /map
LIBS4= $(LIB)\OS2.LIB


#	List of object files required

OBJS=	comsam2.obj 
DOBJS=	comsam2d.obj

comsam2.obj: comsam2.asm comdefs.inc com.inc $(INC)\error.inc \
	devsym.inc $(INC)\devhdr.inc $(INC)\devhlp.inc
	$(ASM) $(AFLAGS) comsam2.asm,comsam2.obj;

comsam2.sys: $(OBJS) $(LIBS4) comsam2.def
	$(LINK) $(LFLAGS4) $(OBJS),comsam2.sys,comsam2.map,$(LIBS4),comsam2.def;

comsam2d.obj: comsam2.asm comdefs.inc com.inc $(INC)\error.inc \
	devsym.inc $(INC)\devhdr.inc $(INC)\devhlp.inc
	$(ASM) $(AFLAGS) -DCOMDEBUG comsam2.asm,comsam2d.obj;

comsam2d.sys: $(DOBJS) $(LIBS4) comsam2.def
	$(LINK) $(LFLAGS4) $(DOBJS),comsam2d.sys,comsam2d.map,$(LIBS4),comsam2.def;
	mapsym comsam2d
