############################################################################
#
#   makefile
#
#   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
#
#   constructs adlib device driver
#
#   to build a debug version:
#       NMAKE
#   to build a non debug version:
#       NMAKE DEBUG=NO
#
############################################################################

NAME    = msadlib
OBJ1    = adliba.obj adlib.obj midimain.obj midic.obj
OBJ2    = init.obj drvproc.obj
OBJS    = libinit.obj $(OBJ1) $(OBJ2)
LIBS    = libw mmsystem mdllcew

!if "$(DEBUG)" == "NO"
DEF     =
CLOPT   = -Oxws
MASMOPT =
LINKOPT =
!else
DEF     = -DDEBUG
CLOPT   = -Oxws -Zid
MASMOPT = -Zi
LINKOPT = /CO/LI
!endif

# NOTE - this code is compiled *without* windows prolog/epilog
#        (no -Gw), so all exported routines must have _loadds

CC      = cl -c -nologo -W3 -Zp -G2s -Alnw -Fc $(DEF) $(CLOPT)
ASM     = masm -Mx -t -D?QUIET $(DEF) $(MASMOPT)
LINK    = link /NOPACK/NOD/NOE/MAP/ALIGN:16 $(LINKOPT)

.c.obj:
    @$(CC) -NT _TEXT $*.c

.asm.obj:
    @echo $(@B).asm
    @$(ASM) $*;

###################### goal ######################

goal: ..\bin\$(NAME).sym ..\bin\$(NAME).drv
    @echo ***** finished making $(NAME) *****

..\bin\$(NAME).drv: $$(@F)
    @copy $(@F) $@

..\bin\$(NAME).sym: $$(@F)
    @copy $(@F) $@

##################################################

$(NAME).drv $(NAME).sym: $(OBJS) adlib.def $(NAME).res
    @$(LINK) @<<
    libinit.obj+
    $(OBJ1)+
    $(OBJ2),
    $(NAME).drv,
    $(NAME).map,
    $(LIBS),
    adlib.def
<<
    @rc -t $(DEF) $(NAME).res $(NAME).drv
!if "$(DEBUG)" != "NO"
    @cvpack -p $(NAME).drv
!endif
    @mapsym /n $(NAME).map

$(NAME).res: msadlib.rc adlib.h adlib.bnk drumkit.bin
    @rc $(DEF) -r -z msadlib.rc

################## segmentation ##################
#
#   _TEXT is the init/exit and non-interrupt time segment
#   _FIX  is the interrupt time fixed segment

SEG  = $(CC) -NT TSEG $*.c
SEGA = $(ASM) -DSEGNAME=TSEG $*;

libinit.obj     : ; $(SEGA:TSEG=_TEXT)
init.obj        : ; @$(SEG:TSEG=_TEXT)
drvproc.obj     : ; @$(SEG:TSEG=_TEXT)
adliba.obj      : ; $(SEGA:TSEG=_FIX)
midic.obj       : ; @$(SEG:TSEG=_FIX)
midimain.obj    : ; @$(SEG:TSEG=_FIX)
adlib.obj       : ; @$(SEG:TSEG=_FIX)

##################### clean ######################

clean:
    -del $(NAME).drv
    -del $(NAME).res
    -del *.sym
    -del *.map
    -del *.obj
    -del *.cod

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
adlib.obj: adlib.c adlib.h

drvproc.obj: drvproc.c adlib.h

adliba.obj: adliba.asm

init.obj: init.c adlib.h

midic.obj: midic.c adlib.h

libinit.obj: libinit.asm

midimain.obj: midimain.c adlib.h

# END Dependencies 
