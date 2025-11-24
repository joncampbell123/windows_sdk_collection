#****************************************************************************
#                                                                           *
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
# PURPOSE.                                                                  *
#                                                                           *
# Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
#                                                                           *
#****************************************************************************

# sample make file for building vmpages virtual device for Windows

NAME = VMPAGES

#       Definitions for the debug level 

!ifdef DEBUG
DDEBUG  =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG  =-DDEBLEVEL=0
!endif

#       Definitions for MASM 6 Assembler

ASM    = ml
AFLAGS = -coff -DBLD_COFF -DIS_32 -W2 -c -Cx -Zm -DMASM6 $(DDEBUG)
ASMENV = ML
LFLAGS = /VXD /NOD

#       MASM 6 only inference rules

.asm.obj:
	set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj $< 

all: $(NAME).vxd

#
# to build vmpgs.exe run ddkenv 16 and run nmake vmpgs.exe
#
vmpgs.exe : vmpgs.obj
        link vmpgs.obj;

vmpgs.obj : vmpgs.asm vmpages.inc
        ml -W2 -c -Cx -Zm -DMASM6 $(DDEBUG) vmpgs.asm

vmpages.obj: vmpages.asm

OBJS =  vmpages.obj 

$(NAME).vxd: $(NAME).def $(OBJS)
        link @<<$(NAME).lnk
$(LFLAGS) 
/OUT:$(NAME).vxd
/MAP:$(NAME).map
/DEF:$(NAME).def
$(OBJS)
<<
        mapsym -s -o $(NAME).sym $(NAME).map
