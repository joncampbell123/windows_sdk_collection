############################################################################
#
#   makefile
#
#   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
#
#   constructs ibmjoy device driver
#
#   to build a debug version:
#       NMAKE
#   to build a non debug version:
#       NMAKE DEBUG=NO
#
############################################################################

NAME    = ibmjoy
OBJ1    = ibmjoy.obj poll.obj config.obj wep.obj
OBJ     = $(OBJ1)
LIBS    = libw mmsystem sdllcew

!if "$(DEBUG)"  == "NO"
DEF     =
CLOPT   = -Owx
MASMOPT =
LINKOPT =
!else
DEF     = -DDEBUG
CLOPT   = -Owx -Zid
MASMOPT = -Zi
LINKOPT = /CO/LI/NOPACK
!endif

CC      = cl -c -nologo -W2 -Asnw -G2sw -Zp $(DEF) $(CLOPT)
ASM     = masm -Mx -t -D?QUIET $(DEF) $(MASMOPT)
LINK    = link /NOD/NOE/MAP/ALIGN:16 $(LINKOPT)

.c.obj:
    $(CC) $*.c

.asm.obj:
    $(ASM) $*;

###################### goal ######################

goal: ..\bin\$(NAME).sym ..\bin\$(NAME).drv
    @echo ***** finished making $(NAME) *****

..\bin\$(NAME).drv: $$(@F)
    @copy $(@F) $@

..\bin\$(NAME).sym: $$(@F)
    @copy $(@F) $@

##################################################

$(NAME).drv $(NAME).sym: $(OBJ) ibmjoy.def $(NAME).res
    $(LINK) @<<
    $(OBJ1),
    $(NAME).drv,
    $(NAME).map,
    $(LIBS),
    ibmjoy.def
<<
    @rc -t $(DEF) $(NAME).res $(NAME).drv
!if "$(DEBUG)" != "NO"
    @cvpack -p $(NAME).drv
!endif
    @mapsym /n $(NAME).map

$(NAME).res: ibmjoy.rc ibmjoy.h
    rc $(DEF) -r -z ibmjoy.rc

##################### clean ######################

clean:
    -del *.drv
    -del *.obj
    -del *.map
    -del *.sym
    -del *.res

##################### depend #####################

depend:
    mv makefile makefile.old
    sed "/^# START Dependencies/,/^# END Dependencies/D" makefile.old > makefile
    -del makefile.old
    echo # START Dependencies >> makefile
    includes -l *.c *.asm >> makefile
    echo # END Dependencies >> makefile

##################################################
# START Dependencies 
config.obj: config.c ibmjoy.h

ibmjoy.obj: ibmjoy.c ibmjoy.h

poll.obj: poll.asm

wep.obj: wep.asm

# END Dependencies 
