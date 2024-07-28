############################################################################
#
#   makefile
#
#   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
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
OBJ1    = mciqa.obj qasample.obj mcicmds.obj
OBJS    = libinit.obj $(OBJ1)
LIBS    = libw mmsystem sdllcew

!if "$(DEBUG)" == "NO"
DEF     =
CLOPT   = -Oas
MASMOPT =
LINKOPT =
!else
DEF     = -DDEBUG 
CLOPT   = -Zid -Oas
MASMOPT = -Zi
LINKOPT = /CO/LI
!endif

CC      = cl -c -nologo -W2 -Asnw -G2sw -Zp $(DEF) $(CLOPT)
ASM     = masm -Mx -t -D?QUIET $(DEF) $(MASMOPT)
LINK    = link /NOPACK/NOD/NOE/MAP/ALIGN:16 $(LINKOPT)

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

$(NAME).drv $(NAME).sym: $(OBJS) mciqa.def $(NAME).res
    $(LINK) @<<
    libinit.obj +
    $(OBJ1),
    $(NAME).drv,
    $(NAME).map,
    $(LIBS),
    mciqa.def
<<
    @rc -t $(DEF) $(NAME).res $(NAME).drv
!if "$(DEBUG)" != "NO"
    @cvpack -p $(NAME).drv
!endif
    @mapsym /n $(NAME).map

$(NAME).res: mciqa.rc mciqa.h
    @rc $(DEF) -r -z mciqa.rc

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

###################################
# START Dependencies 
mcicmds.obj: mcicmds.c mciqa.h qasample.h

libinit.obj: libinit.asm

mciqa.obj: mciqa.c mciqa.h

qasample.obj: qasample.c qasample.h

# END Dependencies 
