############################################################################
#
#   makefile
#
#   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
#
#   constructs sndblst device driver
#
#   to build a debug version:
#       NMAKE
#   to build a non debug version:
#       NMAKE DEBUG=NO
#
############################################################################

!ifndef LANG
RES_DIR=.\messages\usa
!else
RES_DIR=.\messages\$(LANG)
!endif

NAME    = sndblst2
OBJ1    = midiin.obj  midiout.obj midifix.obj midia.obj
OBJ2    = wavein.obj  waveout.obj wavefix.obj wavea.obj
OBJ3    = sndblst.obj inita.obj   initc.obj config.obj
OBJ4    = drvproc.obj commona.obj
OBJS    = libinit.obj $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4)
LIBS    = libw mmsystem

!if "$(DEBUG)" == "NO"
DEF     =
CLOPT   =
MASMOPT =
LINKOPT =
!else
DEF     = -DDEBUG
CLOPT   = -Zid
MASMOPT = -Zi
LINKOPT = /CO/LI
!endif

# NOTE - this code is compiled *without* windows prolog/epilog
#        (no -Gw), so all exported routines must have _loadds

CC      = cl -c -nologo -W3 -Alnw -G2s -Zp -Fc -Oxws $(DEF) $(CLOPT)
ASM     = masm -Mx -t -D?QUIET $(DEF) $(MASMOPT)
LINK    = link /NOPACK/NOD/NOE/MAP/ALIGN:16 $(LINKOPT)

.c.obj:
    $(CC) $*.c

.asm.obj:
    $(ASM) $*;

###################### goal ######################

goal: iclean ..\bin\$(NAME).sym ..\bin\$(NAME).drv
    @echo ***** finished making $(NAME) *****

..\bin\$(NAME).drv: $$(@F)
    @copy $(@F) $@

..\bin\$(NAME).sym: $$(@F)
    @copy $(@F) $@

iclean:
    del $(NAME).res

##################################################

$(NAME).drv $(NAME).sym: $(OBJS) sndblst.def $(NAME).res
    $(LINK) @<<
    libinit.obj+
    $(OBJ1)+
    $(OBJ2)+
    $(OBJ3)+
    $(OBJ4),
    $(NAME).drv,
    $(NAME).map,
    $(LIBS),
    sndblst.def
<<
    @rc -t $(DEF) -30 $(NAME).res $(NAME).drv
!if "$(DEBUG)" != "NO"
    @cvpack -p $(NAME).drv
!endif
    @mapsym /n $(NAME).map

!ifndef LANG
$(NAME).res: $(RES_DIR)\sndblst.rc sndblst.h
    rc $(DEF) -r -z -fo $(NAME).res $(RES_DIR)\sndblst.rc
!else
$(NAME).res:
    copy $(RES_DIR)\$(NAME).res
!endif


################## segmentation ##################
#
#   INIT   is the init/exit time segment
#   _TEXT  is the interrupt time fixed segment
#   WAVE   is the waveform support
#   MIDI   is the MIDI support
#   COMMON is common discardable wave/midi support

SEG  = $(CC) -NT TSEG $*.c
SEGA = $(ASM) -DSEGNAME=TSEG $*,$*.obj;

libinit.obj     : ; $(SEGA:TSEG=INIT)
config.obj      : ; $(SEG:TSEG=INIT)
inita.obj       : ; $(SEGA:TSEG=INIT)
initc.obj       : ; $(SEG:TSEG=INIT)
midia.obj       : ; $(SEGA:TSEG=MIDI)
midiout.obj     : ; $(SEG:TSEG=MIDI)
midiin.obj      : ; $(SEG:TSEG=MIDI)
wavea.obj       : ; $(SEGA:TSEG=WAVE)
waveout.obj     : ; $(SEG:TSEG=WAVE)
wavein.obj      : ; $(SEG:TSEG=WAVE)
sndblst.obj     : ; $(SEGA:TSEG=_TEXT)
wavefix.obj     : ; $(SEG:TSEG=_TEXT)
midifix.obj     : ; $(SEG:TSEG=_TEXT)
commona.obj     : ; $(SEGA:TSEG=COMMON)
drvproc.obj     : ; $(SEG:TSEG=COMMON)

##################### clean ######################

clean:
    -del *.drv
    -del *.obj
    -del *.map
    -del *.sym
    -del *.cod
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
config.obj: config.c sndblst.h

drvproc.obj: drvproc.c sndblst.h

initc.obj: initc.c sndblst.h

inita.obj: inita.asm sndblst.inc

midifix.obj: midifix.c sndblst.h

midia.obj: midia.asm sndblst.inc

libinit.obj: libinit.asm

midiin.obj: midiin.c sndblst.h

midiout.obj: midiout.c sndblst.h

wavefix.obj: wavefix.c sndblst.h

wavea.obj: wavea.asm sndblst.inc

sndblst.obj: sndblst.asm sndblst.inc

wavein.obj: wavein.c sndblst.h

waveout.obj: waveout.c sndblst.h

commona.obj: commona.asm sndblst.inc

# END Dependencies
