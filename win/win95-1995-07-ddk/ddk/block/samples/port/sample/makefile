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

DEVICE = PORT
OBJS   = port.obj portaer.obj portreq.obj portisr.obj

AFLAGS= -DDEBUG_TRACE=1

######### Definitions for the debug level #########

!ifdef DEBUG
DDEBUG =-DDEBLEVEL=1 -DDEBUG
!else
DDEBUG =-DDEBLEVEL=0
!endif

########## Definitions for the Assembler ##########

ASM     = ml
AFLAGS  = -coff -DBLD_COFF $(AFLAGS) -DIS_32 -nologo -W3 -Zd -c -Cx -DMASM6
AFLAGS  = $(AFLAGS) -DINITLOG $(DDEBUG)
ASMENV  = ML

########## Definitions for linker #############

LFLAGS  = /VXD /NOD

########## Base inference rules ###################

#       MASM 6 only inference rules

.asm.obj:
	set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj $< 

############### VxD device section ####################

$(DEVICE).sym: $(DEVICE).map
	mapsym -s -o .\$(DEVICE).sym .\$(DEVICE).map > nul

$(DEVICE).map: $(DEVICE).pdr

$(DEVICE).pdr : $(OBJS)
        link @<<$(DEVICE).lnk
$(LFLAGS) 
/OUT:$(DEVICE).pdr
/MAP:$(DEVICE).map
$(OBJS)
<<
