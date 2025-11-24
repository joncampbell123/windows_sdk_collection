#############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
#
#   makefile
#
#   constructs joystick device driver
#
#   to build a debug version:
#       NMAKE
#   to build a non debug version:
#       NMAKE DEBUG=NO
#
############################################################################

!if "$(DDKROOT)" == ""
!error Environment variable DDKROOT must be set
!else
INCLUDES=-I..\VXD -I$(DDKROOT)\mmedia\samples\include
!endif

NAME    = msjstick
EXT     = drv
GOALS   = $(NAME).$(EXT) $(NAME).sym
OBJ1    = $(NAME).obj vjoydacc.obj wep.obj
OBJS    = libinit.obj $(OBJ1)
LIBS	= libw mmsystem mdllcew

!if "$(DEBUG)" == "NO"
COPT    =
AOPT    =
LOPT    = 
ROPT    =
!else
COPT    =-Zi -DDEBUG
AOPT    =-Zi -DDEBUG
LOPT    =/CO/LI
ROPT    =-DDEBUG
!endif

CC      =cl
LINK    =link
ASM	=ml
RC      =rc

CFLAGS  =-nologo -c -W3 -AMnw -G3s -Oxws -Z7pd $(COPT) $(INCLUDES)
LFLAGS  =/MAP/LI/NOPACKCODE/ONERROR:NOEXE/NOLOGO/NOE/NOD/AL:16$(LOPT)
AFLAGS  =-DMASM6 -I. /Zm /c $(DEF) -Cx $(AOPT) $(INCLUDES)
RCFLAGS =$(ROPT) -z $(INCLUDES)

goal: $(GOALS)

libinit.obj:
        $(ASM) $(AFLAGS) -DSEGNAME=_TEXT -Ta $*.asm

vjoydacc.obj:
        $(ASM) $(AFLAGS) -DSEGNAME=_TEXT -Ta $*.asm

wep.obj:
        $(ASM) $(AFLAGS) -DSEGNAME=_TEXT -Ta $*.asm

$(NAME).obj:
	$(CC) $(CFLAGS) -NT _TEXT $*.c

$(NAME).res:
	$(RC) $(RCFLAGS) -r -fo$@ $*.rc

##################################################
$(NAME).$(EXT) $(NAME).sym:	\
		$(OBJS) $(NAME).res $(NAME).def
	@$(LINK) @<<
        libinit.obj +
        $(OBJ1),
        $*.$(EXT) $(LFLAGS) ,
        $*.map,
        $(LIBS),
        $*.def
<<
	$(RC) $(RCFLAGS) $*.res $*.$(EXT)
	mapsym $*.map

clean:
        -del *.obj
        -del *.res
        -del *.map
        -del *.sym
        -del $(NAME).$(EXT)
        -del *.pdb
