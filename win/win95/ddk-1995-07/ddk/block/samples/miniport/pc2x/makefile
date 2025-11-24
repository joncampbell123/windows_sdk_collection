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

INCLUDE =.;$(INCLUDE)

DEVICE          = pc2x
OBJS            = pc2x.obj 

######### Definitions for the debug level #########

!ifdef DEBUG

DDEBUG =-DDEBLEVEL=1 -DDEBUG
CFLAGS =$(CFLAGS) -Oy1

!else

DDEBUG =-DDEBLEVEL=0

!endif

########## Definitions for the Assembler ##########

ASM     = ml
AFLAGS  = -coff -DBLD_COFF $(DDEBUG) -DIS_32 -nologo -W2 -Zd -c -Cx -DMASM6
ASMENV  = ML

########## Definitions for C compiler #############

CL      = cl
CFLAGS  = -nologo -Zdp -Gs -c -DIS_32 -Zl -Gz -DDBG $(CFLAGS) 
CFLAGS  = $(CFLAGS) $(DDEBUG) 

########## Definitions for linker #############

LINK 	 = link

########## Base inference rules ###################

#       MASM 6 only inference rules

.asm.obj:
	set $(ASMENV)=$(AFLAGS)
	$(ASM) -Fo$*.obj $< 

#       32 bit C compiler inference rules

.c.obj:
	set CL=$(CFLAGS)
	$(CL) -Fo$*.obj $<

############### VxD device section ####################

$(DEVICE).sym: $(DEVICE).map
	mapsym -s -o .\$(DEVICE).sym .\$(DEVICE).map > nul

$(DEVICE).map: $(DEVICE).mpd

$(DEVICE).mpd : $(OBJS) $(LIBS) $(STUB)
	$(LINK) @$(DEVICE).lnk
