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

!if "$(DDKROOT)" == ""
!error Environment variable DDKROOT must be set
!else
INCLUDES=-I..\..\include -I..\avwin
!endif

NAME	=avcapt
EXT	=drv
GOALS   =$(NAME).$(EXT) $(NAME).sym
LIBS	=mdllcew libw mmsystem ..\avwin\avwin
OBJS	=drvproc.obj initc.obj inita.obj vmsg.obj vxdiface.obj\
	 mapa.obj mapc.obj avdev.obj cap.obj muldiv.obj config.obj\
	 isr.obj rect.obj yuv.obj

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

ROPT = /dDS_3DLOOK^=0x0004L /dDS_CONTEXTHELP^=0x2000L #from sdk\inc32\winuser.h

CC	=cl -c -nologo -W3 -Alnw -Ox -Zp -GD -D_WINDLL $(COPT) $(INCLUDES) -Fd$*
ASM     =ml -c -Zm -D?QUIET -D?MEDIUM $(AOPT) $(INCLUDES)
LINK	=link $(LOPT) /ALIGN:16/ONERROR:NOEXE/MAP /NOE
RC      =rc -r $(ROPT) $(INCLUDES)
RCLINK  =rc $(ROPT) $(INCLUDES)

goal: $(GOALS)

drvproc.obj:	$$(@B).c
	$(CC) -NT _TEXT $(@B).c

vmsg.obj: $$(@B).c
	$(CC) -NT _TEXT $(@B).c

vxdiface.obj:  $$(@B).c
	$(CC) -NT INIT $(@B).c

mapc.obj:	$$(@B).c
	$(CC) -NT VCAP $(@B).c

avdev.obj:     $$(@B).c
	$(CC) -NT _TEXT $(@B).c

cap.obj:  $$(@B).c
	$(CC) -NT _TEXT $(@B).c

yuv.obj:	$$(@B).c
	$(CC) -NT _TEXT $(@B).c

config.obj:	$$(@B).c
	$(CC) -NT VCAP $(@B).c

rect.obj:	$$(@B).c
	$(CC) -NT VCAP $(@B).c

initc.obj:	$$(@B).c mmdebug.h
	@$(CC)	-NT INIT $(@B).c

inita.obj: $$(@B).asm
	$(ASM) -DSEGNAME=INIT $*.asm

isr.obj: $$(@B).asm
	$(ASM) -DSEGNAME=_TEXT $*.asm

mapa.obj: $$(@B).asm
	$(ASM) -DSEGNAME=_TEXT $*.asm

muldiv.obj: $$(@B).asm
	$(ASM) -DSEGNAME=_TEXT $*.asm

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
