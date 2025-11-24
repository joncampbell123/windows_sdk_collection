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

# sample make file for building vitd virtual device for Microsoft Windows

NAME = VITD

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
# to build sample.exe run ddkenv 16 and run nmake sample.exe
#
sample.exe: sample.obj
        link sample.obj,,,,, 

sample.obj: sample.asm itimer.inc
        ml -W2 -c -Cx -Zm -DMASM6 $(DDEBUG) sample.asm

#
# to build dosit.exe run ddkenv 16 and run nmake dosit.exe
#
dosit.exe: dosit.obj 
        link dosit.obj,,,,,

dosit.obj: dosit.asm dosit.inc itimer.inc 
        ml -W2 -c -Cx -Zm -DMASM6 $(DDEBUG) dosit.asm

vitd.obj: vitd.asm

OBJS = vitd.obj

$(NAME).vxd: $(NAME).def $(OBJS)
        link @<<$(NAME).lnk
$(LFLAGS) 
/OUT:$(NAME).vxd
/MAP:$(NAME).map
/DEF:$(NAME).def
$(OBJS)
<<
        mapsym -s -o $(NAME).sym $(NAME).map

