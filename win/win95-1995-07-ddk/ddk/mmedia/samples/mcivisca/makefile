#===========================================================================;
#
#   MCIVISCA
#
#
#############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   Copyright (c) 1993 - 1995  Microsoft Corporation.  All Rights Reserved.
#
#############################################################################
#
#   to make a NON-DEBUG build, type the following line:
#       nmake DEBUG=NO
#   just typing 'nmake' will build a DEBUG build
#
#   You can also set the environment variable DEBUG to NO
#
#
#   Please note!  link.exe from MASM 6.11 is older than link.exe from
#   MSVC 1.0 or 1.5.  Be sure you are using the link.exe from MSVC
#   instead of the one from MASM when building this sample.
#
#===========================================================================;

TARGETBASE	=mcivisca
EXT		=drv
TARGETNAME	=$(TARGETBASE)
LOBJ		=.
LIBS		=libw mlibcew mmsystem
OBJS		=$(LOBJ)\libinit.obj $(LOBJ)\viscacom.obj $(LOBJ)\commtask.obj $(LOBJ)\viscamsg.obj $(LOBJ)\mcicmds.obj $(LOBJ)\mcidelay.obj $(LOBJ)\mcivisca.obj $(LOBJ)\common.obj
GOALS		=$(LOBJ)\$(TARGETNAME).$(EXT) $(LOBJ)\$(TARGETNAME).sym

!if "$(DEBUG)" == "NO"
DEF		=
CDEBUG		=$(DEF)
ADEBUG		=$(DEF)
LDEBUG		=
!else
DEF		=-DDEBUG
CDEBUG		=-Zi $(DEF)
ADEBUG		=-Zi $(DEF)
LDEBUG		=/CO/LI
!endif

TARGETWARN	=-W3 -WX
TARGETOPT	=-Ox
CFLAGS		=-AMnw $(TARGETOPT) -DUSECOMM -DWIN31 $(CDEBUG) -Fo$@ $(TARGETWARN) -Zdp -Gf3s -c -DIS_16 -DMTN -I..\include

AFLAGS		=-W2 -Zd -Mx -D?MEDIUM -D?QUIET $(ADEBUG)
RCFLAGS     =-z -r -I..\include $(RDEBUG) \
             /dDS_CONTEXTHELP^=0x2000L #from sdk\inc32\winuser.h
LFLAGS		=/L/MAP/NOPACKCODE/NOE/NOD/ONERROR:NOEXE/ALIGN:16$(LDEBUG)
RESFLAGS	=-t
RC		=rc -nologo
CC		=cl -c -nologo
ASM		=masm -t
LINK		=link /nologo
MFLAGS		=

goal:	makedirs $(GOALS)
	@echo ***** Finished making $(TARGETBASE) *****

makedirs:
!if !exists($(LOBJ))
	@md $(LOBJ) >nul
!endif

clean:
        @echo y | del $(LOBJ)\*.obj >nul
        @echo y | del $(LOBJ)\*.pdb >nul
        @echo y | del $(LOBJ)\*.res >nul
        @echo y | del $(LOBJ)\*.cod >nul
        @echo y | del $(LOBJ)\*.map >nul
        @echo y | del $(LOBJ)\$(TARGETNAME).$(EXT) >nul
        @echo y | del $(LOBJ)\$(TARGETNAME).sym >nul
        @echo ***** Finished cleaning $(TARGETBASE) *****

$(LOBJ)\libinit.obj: $$(@B).asm
	@echo $(@F)
	@$(ASM) $(AFLAGS) -DSEGNAME=_TEXT $(@B).asm,$@;

$(LOBJ)\viscacom.obj: $$(@B).c mcivisca.h viscadef.h
	@$(CC) -NT TEXT @<<
$(CFLAGS) $(@B).c
<<

$(LOBJ)\commtask.obj: $$(@B).c viscadef.h mcivisca.h
	@$(CC) -NT CONFIG_TEXT @<<
$(CFLAGS) $(@B).c
<<

$(LOBJ)\viscamsg.obj: $$(@B).c viscadef.h
	@$(CC) -NT CONFIG_TEXT @<<
$(CFLAGS) $(@B).c
<<

$(LOBJ)\mcicmds.obj: $$(@B).c mcivisca.h viscadef.h viscamsg.h
	@$(CC) -NT TEXT @<<
$(CFLAGS) $(@B).c
<<

$(LOBJ)\mcidelay.obj: $$(@B).c mcivisca.h viscadef.h viscamsg.h
	@$(CC) -NT TEXT @<<
$(CFLAGS) $(@B).c
<<

$(LOBJ)\mcivisca.obj: $$(@B).c mcivisca.h
	@$(CC) -NT CONFIG_TEXT @<<
$(CFLAGS) $(@B).c
<<

$(LOBJ)\common.obj: $$(@B).c
	@$(CC) -NT TEXT @<<
$(CFLAGS) $(@B).c
<<

$(LOBJ)\$(TARGETNAME).sym:	$$(@R).map
	@mapsym $*.map

$(LOBJ)\$(TARGETBASE).res: $$(@B).rc $$(@B).rcv cnfgdlg.dlg cnfgdlg.h
	@$(RC) $(RCFLAGS) -fo$@ $(@B).rc

$(LOBJ)\$(TARGETNAME).$(EXT) $(LOBJ)\$(TARGETNAME).map: $(OBJS) $$(@D)\$(TARGETBASE).res $$(@B).def
	@$(LINK) $(LFLAGS) @<<
$(OBJS),
$(@R).$(EXT),
$(@R).map,
$(LIBS),
$(@B).def
<<
	@$(RC) $(RESFLAGS) $(@D)\$(TARGETBASE).res $*.$(EXT)
