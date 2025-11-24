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
##**  Please note!  link.exe from MASM 6.11 is older than link.exe from 
##**  MSVC 1.0 or 1.5.  Be sure you are using the link.exe from MSVC 
##**  instead of the one from MASM when building this sample.
##**
##************************************************************************

!if "$(DDKROOT)" == ""
!error Environment variable DDKROOT must be set
!else
INCLUDES=-I$(DDKROOT)\mmedia\samples\include
!endif

NAME	=avwin
EXT	=dll
GOALS	=$(NAME).$(EXT) $(NAME).sym $(NAME).lib
LIBS	=mdllcew libw
OBJS	=avwin.obj avwin2.obj avwin3.obj avwin4.obj avwin5.obj avwin6.obj \
	avwin7.obj avwin8.obj avwin9.obj avwina.obj bmtype.obj

AINCS =
CINCS =

!if "$(DEBUG)" == "NO"
COPT    =
AOPT    =
LOPT    = 
!else
COPT    =-Zi -DDEBUG
AOPT    =-Zi -DDEBUG
LOPT    =/CO
!endif

CC      =cl -c -nologo -W3 -D_WINDLL -AMw -GD -G3s -Ox -Zp $(COPT) $(INCLUDES)
ASM     =ml -c -Zm -D?QUIET -D?MEDIUM $(AOPT) $(INCLUDES)
LINK	=link $(LOPT) /ALIGN:16/ONERROR:NOEXE/MAP /NOE
RC      =rc -r $(ROPT) $(INCLUDES)
RCLINK	=rc $(ROPT) $(INCLUDES)
LB	=implib

goal: $(GOALS)

bmtype.obj \
avwin.obj \
avwin2.obj \
avwin3.obj \
avwin4.obj \
avwin5.obj \
avwin6.obj \
avwin7.obj \
avwin8.obj \
avwin9.obj:	$$(@B).c avwinrc.h avwin.h global.h
	@$(CC) $(CFLAGS) $(@B).c

libinit.obj avwina.obj:	$$(@B).asm
	@echo $(@B).asm
	@$(ASM) $(@B).asm

$(NAME).res:  $(NAME).rc $(NAME).rcv
	$(RC) -fo$(NAME).res $(NAME).rc

$(NAME).lib:  $(NAME).$(EXT)
	@$(LB) $(NAME).lib $(NAME).$(EXT)

$(NAME).$(EXT) $(NAME).map: $(OBJS) $(NAME).def $(NAME).res
        $(LINK) @<<
$(OBJS),
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
	-@del *.lib
