#
# OS/2 DEVICE DRIVER DEVELOPMENT KIT VERSION
#
# Copyright (c) Microsoft Corporation,  1988
#
#
#
#   Definitions

LIB=..\..\..\lib
CFLAGS   = -c -Ox -G2 -Zep -DINCL_DOSFILEMGR
LINKFLAGS = /nod 
# 5.1 tools
LINK=link
CC=cl
MASM=masm
BIND=bind
LIBS   = os2 slibcep

OBJS   = comtest.obj comsubs.obj

#   Rules

.asm.obj:
	$(MASM) $*.asm;

.c.obj:
	$(CC) $(CFLAGS) $*.c

comsubs.obj:	comsubs.asm
	$(MASM) $*.asm;

comtest.obj:	comtest.c 
	$(CC) $(CFLAGS) $*.c

comtest.exe:	$(OBJS)
	$(LINK) $(LINKFLAGS) $(OBJS),,,$(LIBS),comtest.def;
	$(BIND) $*.exe $(LIB)\os2.lib


