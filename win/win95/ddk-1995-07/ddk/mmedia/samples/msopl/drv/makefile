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

NAME    =msopl
EXT     =drv
GOALS   =$(NAME).$(EXT) $(NAME).sym
LIBS    =mdllcew libw mmsystem
OBJS    =drvproc.obj fmmain.obj fmsynth.obj init.obj vxdiface.obj

AINCS =
CINCS =msopl.h resource.h

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

drvproc.obj: drvproc.c $(CINCS)
        $(CC) -NT MIDI $*.c

fmmain.obj:  fmmain.c $(CINCS)
        $(CC) -NT MIDI $*.c

fmsynth.obj: fmsynth.c $(CINCS)
        $(CC) -NT _TEXT $*.c

init.obj:    init.c $(CINCS)
        $(CC) -NT MIDI $*.c

vxdiface.obj: vxdiface.c $(CINCS)
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
