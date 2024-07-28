############################################################################
#
#   makefile
#
#   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
#
#   constructs mcipionr device driver
#
#   to build a debug version:
#       NMAKE
#   to build a non debug version:
#       NMAKE DEBUG=NO
#
############################################################################

NAME    = mcipionr
OBJS    = libinit.obj mcipionr.obj piondrvr.obj pioncnfg.obj
LIBS    = libw sdllcew mmsystem

!if "$(DEBUG)" == "NO"
DEF     =
CLOPT   = -Oxws $(DEF)
MASMOPT = -Zi
LINKOPT =
!else
DEF     = -DDEBUG
CLOPT   = -Zi -Oxws $(DEF)
MASMOPT = -Zi -DDEBUG
LINKOPT = /CO/LI
!endif

!if "$(STRICT)" == "YES"
TYPES   = -DSTRICT
!else
TYPES   =
!endif

CC      = cl -c -nologo -W3 -Asnw -G2s -Zp $(TYPES) $(CLOPT)
ASM     = masm -Mx -t -D?QUIET $(MASMOPT)
LINK    = link /NOPACK/NOD/NOE/MAP $(LINKOPT)

###################### goal ######################

goal: ..\bin\$(NAME).sym ..\bin\$(NAME).drv
    @echo ***** finished making $(NAME) *****

..\bin\$(NAME).drv: $$(@F)
    @copy $(@F) $@

..\bin\$(NAME).sym: $$(@F)
    @copy $(@F) $@

##################################################

$(NAME).drv $(NAME).sym: $(OBJS) mcipionr.def $(NAME).res
    @$(LINK) @<<
    $(OBJS),
    $(NAME).drv,
    $(NAME).map,
    $(LIBS),
    mcipionr.def
<<
    @rc -t $(DEF) $(NAME).res $(NAME).drv
!if "$(DEBUG)" != "NO"
    @cvpack -p $(NAME).drv
!endif
    @mapsym /n $(NAME).map

$(NAME).res: mcipionr.rc mcipionr.h pioncnfg.h pioncnfg.dlg
    @rc $(DEF) -r -z mcipionr.rc

##################### clean ######################

clean:
    -del $(NAME).drv
    -del $(NAME).res
    -del *.sym
    -del *.map
    -del *.obj

##################### depend #####################

libinit.obj: $$(@B).asm
    @echo $(@B).asm
    @$(ASM) -DSEGNAME=_TEXT $*,$@;

mcipionr.obj: $$(@B).c mcipionr.h
    @$(CC) -NT _TEXT $*.c

piondrvr.obj: $$(@B).c pioncnfg.h mcipionr.h
    @$(CC) -NT _TEXT $*.c

pioncnfg.obj: $$(@B).c $$(@B).h mcipionr.h
    @$(CC) -NT CONFIG_TEXT $*.c
