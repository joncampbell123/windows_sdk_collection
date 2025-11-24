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

INCLUDES=-I..\..\..\include

!ifndef SUBDIR
SUBDIR = MSSNDSYS
!MESSAGE SUBDIR undefined, defaulting to build in SUBDIR=$(SUBDIR)
!else
!MESSAGE building in SUBDIR=$(SUBDIR)
!endif

!if "$(SUBDIR)" == "AZT16C"
DEF =$(DEF) -DNOVA -DAZTECH
!elseif "$(SUBDIR)" == "AZT16W"
DEF =$(DEF) -DWASHTON -DAZTECH
!elseif "$(SUBDIR)" == "MSSNDSYS"
DEF =$(DEF) -DMSSNDSYS
!else
!message usage: nmake SUBDIR=AZT16C | AZT16W | MSSNDSYS
!error SUBDIR must be defined to one of the above
!endif

BASE =mssndsys
NAME    =$(SUBDIR)
EXT     =drv
GOALS   =$(NAME).$(EXT) $(NAME).sym
LIBS    =mdllcew libw mmsystem commctrl setupx
OBJS    =commona.obj wave.obj wavefix.obj auxil.obj hardware.obj\
         hardfix.obj volume.obj assemf.obj assemu.obj init.obj\
         drvproc.obj mixer.obj controls.obj mix_objs.obj proppage.obj


AINCS =
CINCS =

!if "$(DEBUG)" == "NO"
COPT    = $(DEF) 
AOPT    = $(DEF)
LOPT    = 
!else
COPT    =-Zi -DDEBUG $(DEF)
AOPT    =-Zi -DDEBUG  $(DEF)
LOPT    =/CO
!endif

# rc -r cmd line is really close to the 128-byte limit, so changed 0x0004L to 4L
ROPT =  $(DEF) /dDS_3DLOOK^=4L /dDS_CONTEXTHELP^=0x2000L #from sdk\inc32\winuser.h

CC      =cl -c -W3 -nologo -Alnw -Oxws -Zp -Gs $(COPT) -DIS_16 $(INCLUDES)
ASM     =ml -c -Zm -D?QUIET -D?MEDIUM $(AOPT) -DIS_16 $(INCLUDES)
LINK    =link $(LOPT) /ALIGN:16/ONERROR:NOEXE/MAP
RC      =rc -r $(ROPT) -DIS_16 $(INCLUDES)
RCLINK  =rc $(ROPT) $(INCLUDES)

goal: makedir $(GOALS)

makedir:
	@md $(SUBDIR) >nul
	@cd $(SUBDIR)
        

init.obj        : ..\$*.c
        @$(CC) -NT INIT ..\$*.c

assemu.obj      : ..\$*.asm
        $(ASM) -DSEGNAME=INIT ..\$*.asm

volume.obj      : ..\$*.c
        $(CC) -NT COMMON ..\$*.c

drvproc.obj     : ..\$*.c
        $(CC) -NT COMMON ..\$*.c

commona.obj     : ..\$*.asm
        $(ASM) -DSEGNAME=COMMON ..\$*.asm

mixer.obj       : ..\$*.c
        $(CC) -NT MIXER ..\$*.c

mix_objs.obj    : ..\$*.c
        $(CC) -NT MIXER ..\$*.c

controls.obj    : ..\$*.c
        $(CC) -NT MIXER ..\$*.c

auxil.obj       : ..\$*.c
        $(CC) -NT WAVE ..\$*.c

hardware.obj    : ..\$*.c
        $(CC) -NT WAVE ..\$*.c

wave.obj        : ..\$*.c
        $(CC) -NT WAVE ..\$*.c

wavefix.obj     : ..\$*.c
        $(CC) -NT _TEXT ..\$*.c

hardfix.obj     : ..\$*.c
        $(CC) -NT _TEXT ..\$*.c

proppage.obj    : ..\$*.c
        @$(CC) -NT INIT ..\$*.c

assemf.obj      : ..\$*.asm
        $(ASM) -DSEGNAME=_TEXT ..\$*.asm

$(NAME).res:  ..\$(BASE).rc ..\$(BASE).rcv
        $(RC) -fo$@ ..\$(BASE).rc

$(NAME).$(EXT) $(NAME).map: $(OBJS) ..\$(NAME).def $(NAME).res
        $(LINK) @<<
$(OBJS: =+^
),
$(NAME).$(EXT),
$(NAME).map,
$(LIBS),
..\$(NAME).def
<<
        $(RCLINK) $*.res $*.$(EXT)

$(NAME).sym: $(NAME).map
        mapsym $(NAME).map


clean:
	-@cd $(SUBDIR)
        -@del *.obj
        -@del *.pdb
        -@del *.map
        -@del *.sym
        -@del $(NAME).$(EXT)
        -@del *.res
	-@cd..
	-@rd $(SUBDIR)
