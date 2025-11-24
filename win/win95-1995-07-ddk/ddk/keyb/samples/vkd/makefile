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

# sample make file for building vkd virtual device for Windows/386

NAME = VKD
INCLUDE = $(INCLUDE);$(DDKROOT)\inc16;

#       Definitions for the debug level 

!ifdef DEBUG
DDEBUG  =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG  =-DDEBLEVEL=0
!endif

#       Definitions for the Assembler

ASM     = ml
AFLAGS  = -coff -DBLD_COFF -DMINIVDD -DIS_32 -nologo -W2 -Zd -c -Cx -DMASM6
AFLAGS  = $(AFLAGS) $(DDEBUG) -DSupport_Reboot
ASMENV  = ML
LFLAGS = /VXD /NOD

#       Base inference rules 

#       MASM 6 only inference rules

.asm.obj:
	set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj $< 

all:    vkd.vxd

vkd.obj: vkd.asm 

vad.obj: vad.asm 

vkdhk.obj: vkdhk.asm 

vkdphys.obj: vkdphys.asm

vkdmsg.obj: vkdmsg.asm

vkdio.obj: vkdio.asm 

OBJS =  vkd.obj vkdhk.obj vkdio.obj vkdmsg.obj vkdphys.obj vad.obj

$(NAME).vxd: $(NAME).def $(OBJS)
        link @<<$(NAME).lnk
$(LFLAGS) 
/OUT:$(NAME).vxd
/MAP:$(NAME).map
/DEF:$(NAME).def
$(OBJS)
<<
        mapsym -s -o $(NAME).sym $(NAME).map

