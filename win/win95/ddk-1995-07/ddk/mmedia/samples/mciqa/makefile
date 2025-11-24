
#############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
#
#   MAKEFILE
#
#   constructs a sample mci driver
#
#   to build a debug version:
#       NMAKE
#   to build a non debug version:
#       NMAKE DEBUG=NO
#
############################################################################

NAME    = mciqa
OBJS	= libinit.obj mciqa.obj qasample.obj mcicmds.obj
LIBS    = libw mmsystem sdllcew

!if "$(DEBUG)" == "NO"
DEF     =
CLOPT   =
MASMOPT =
LINKOPT =
!else
DEF     = -DDEBUG 
CLOPT   = -Zid
MASMOPT = -Zi
LINKOPT = /CO/LI
!endif

CC      = cl -c -nologo -Ox -W3 -WX -Asnw -G3Ds -Z7p $(DEF) $(CLOPT)
ASM     = masm -Mx -t -D?QUIET $(DEF) $(MASMOPT)
LINK    = link /NOPACKCODE/NOD/NOE/MAP/ALIGN:16 $(LINKOPT)

.c.obj:
    $(CC) $*.c

.asm.obj:
    $(ASM) $*.asm;

###################### goal ######################

goal: $(NAME).sym $(NAME).drv

##################################################

$(NAME).drv $(NAME).sym: $(OBJS) mciqa.def $(NAME).res
    $(LINK) @<<
    $(OBJS),
    $(NAME).drv,
    $(NAME).map,
    $(LIBS),
    mciqa.def
<<
    @rc -t $(DEF) $(NAME).res $(NAME).drv
    @mapsym /n $(NAME).map

$(NAME).res: mciqa.rc mciqa.h
    @rc $(DEF) -r -z -fo$@ -I..\include mciqa.rc

##################### clean ######################

clean:
    -del *.drv
    -del *.obj
    -del *.map
    -del *.sym
    -del *.res

##################### depend #####################

mcicmds.obj: mcicmds.c mciqa.h qasample.h

libinit.obj: libinit.asm

mciqa.obj: mciqa.c mciqa.h

qasample.obj: qasample.c qasample.h
