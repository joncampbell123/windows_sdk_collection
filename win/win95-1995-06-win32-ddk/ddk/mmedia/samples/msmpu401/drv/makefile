##************************************************************************
##**
##**  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
##**  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
##**  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
##**  A PARTICULAR PURPOSE.
##**
##**  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.
##**
##**  makefile
##**
##**  to build a debug version:
##**       NMAKE
##**  to build a non debug version:
##**       NMAKE DEBUG=NO
##**
##**  HISTORY:
##**     04/12/94       created.
##**
##************************************************************************

INCLUDES=-I..\..\include

NAME    =msmpu401
EXT     =drv
GOALS   =$(NAME).$(EXT) $(NAME).sym
LIBS    =mdllcew libw mmsystem
OBJS    =assemu.obj drvproc.obj init.obj mpu401.obj midia.obj midiin.obj \
         midiout.obj midifix.obj vxdiface.obj

AINCS =
CINCS =msmpu401.h resource.h

!if "$(DEBUG)" == "NO"
COPT    =
AOPT    =
LOPT    = 
!else
COPT    =-Zi -DDEBUG
AOPT    =-Zi -DDEBUG
LOPT    =/CO
!endif

CC      =cl -c -nologo -W3 -Alnw -Oxws -Zp -Gs $(COPT) $(INCLUDES)
ASM     =ml -c -Zm -D?QUIET -D?MEDIUM $(AOPT) $(INCLUDES)
LINK    =link $(LOPT) /ALIGN:16/ONERROR:NOEXE/MAP
RC      =rc -r $(ROPT) $(INCLUDES)
RCLINK  =rc $(ROPT) $(INCLUDES)

goal: $(GOALS)

drvproc.obj: drvproc.c
        $(CC) -NT MIDI $*.c

init.obj: init.c
        $(CC) -NT MIDI $*.c

assemu.obj: assemu.asm
        $(ASM) -DSEGNAME=MIDI $*.asm

mpu401.obj: mpu401.asm
        $(ASM) -DSEGNAME=_FIX $*.asm

midia.obj: midia.asm
        $(ASM) -DSEGNAME=MIDI $*.asm

midifix.obj: midifix.c
        $(CC) -NT _FIX $*.c

midiin.obj: midiin.c
        $(CC) -NT _FIX $*.c

midiout.obj: midiout.c
        $(CC) -NT MIDI $*.c

vxdiface.obj: vxdiface.c
        $(CC) -NT MIDI $*.c

$(NAME).res:  $(NAME).rc $(NAME).rcv
        $(RC) -fo$(NAME).res $(NAME).rc

$(NAME).$(EXT) $(NAME).map: $(OBJS) $(NAME).def $(NAME).res
        $(LINK) @<<
$(OBJS: =+^
),
$(NAME).$(EXT),
$(NAME).map,
$(LIBS),
$(NAME).def
<<
        $(RCLINK) $(NAME).res $(NAME).$(EXT)

$(NAME).sym: $(NAME).map
        mapsym $(NAME).map

clean:
        -@del *.obj
        -@del *.map
        -@del *.sym
        -@del $(NAME).$(EXT)
        -@del *.res
