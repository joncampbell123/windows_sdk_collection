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
##**     04/26/93       created.
##**
##************************************************************************

!if "$(DDKROOT)" == ""
!error Environment variable DDKROOT must be set
!else
INCLUDES=-I$(DDKROOT)\mmedia\samples\include
!endif

NAME    =digitalv
EXT     =mci
GOALS   =$(NAME).$(EXT) $(NAME).sym
LIBS	=libw mlibcew
OBJS    =libinit.obj

!if "$(DEBUG)" == "NO"

LOPT    =
COPT    =
AOPT    =

!else

LOPT    =/CO /LI
COPT    =-Zi -DDEBUG
AOPT    =-Zi -DDEBUG
ROPT    =-v -DDEBUG

!endif

CC      =cl -nologo -c -W3 -AMnw -GD3s -Ox -Z7p $(COPT) -DWIN31 -DUSECOMM $(INCLUDES)
ASM     =masm -t -W2 -Zd -Mx -D?MEDIUM -D?QUIET $(AOPT) $(INCLUDES)
LINK    =link /NOLOGO /MAP/NOE/NOD/AL:16 $(LOPT)
RC      =rc -r $(ROPT) $(INCLUDES)
RCLINK  =rc $(ROPT) $(INCLUDES)


.c.obj:
        $(CC) $*.c

#
#  Explicit Rules
#

goal: $(GOALS)

$(NAME).$(EXT) $(NAME).map: $(OBJS) $*.res $*.def
        $(LINK) @<<
        $(OBJS),
        $*.$(EXT),
        $*.map,
        $(LIBS),
        $*.def
<<
        $(RCLINK) $*.res $*.$(EXT)

$(NAME).sym:    $$(@R).map
	mapsym	$*.map

#
#  Dependencies
#
$(NAME).res: dgv.rc $*.h $*.rc
        $(RC) -fo$@ dgv.rc

libinit.obj: $*.asm
        $(ASM) -DSEGNAME=_TEXT $(@B).asm,$@;

#
#  Clean up
#

clean:
        -@del *.obj
        -@del *.cod
        -@del $(NAME).$(EXT)
        -@del $(NAME).res
        -@del $(NAME).map
        -@del $(NAME).sym
