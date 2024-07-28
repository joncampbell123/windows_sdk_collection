############################################################################
#
#   makefile
#
#   Copyright (c) 1991-1992 Microsoft Corporation.  All Rights Reserved.
#
#   constructs the Sound Blaster VxD
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

NAME    = vsbd
OBJS    = vsbd.obj vsbdmsg.obj
LIBS    =

!if "$(DEBUG)" == "NO"
DEF     =
MASMOPT =
LINKOPT =
!else
DEF     = -DDEBUG
MASMOPT = 
LINKOPT = 
!endif

ASM     = masm5 -Mx -w2 -t -p $(DEF) $(MASMOPT)
LINK    = link386 /NOPACK /NOD /NOI /MAP $(LINKOPT)

.asm.obj:
    $(ASM) $<;

###################### goal ######################

goal: iclean ..\bin\$(NAME).386 ..\bin\$(NAME).sym ..\inc\vsbd.inc ..\inc\vadlibd.inc
    @echo ***** finished making $(NAME) *****

..\bin\$(NAME).386: $$(@F)
    copy $(@F) $@

..\bin\$(NAME).sym: $$(@F)
    copy $(@F) $@

..\inc\vsbd.inc: $$(@F)
    copy $(@F) $@

..\inc\vadlibd.inc: $$(@F)
    copy $(@F) $@

iclean:
    del vsbdmsg.obj

##################################################

$(NAME).386 $(NAME).sym: $(RES_DIR)\vsbdmsg.asm vsbd.def $(OBJS)
    $(LINK) @<<
    $(OBJS),
    $(NAME).386,
    $(NAME).map,
    $(LIBS),
    vsbd.def
<<
    addhdr $(NAME).386
    mapsym32 $(NAME)

##################### clean ######################

clean: 
    del *.obj
    del *.map
    del *.sym
    del *.386

##################### depend #####################
# START Dependencies 

vsbd.obj $(NAME).lst: vsbd.asm vsbd.inc vadlibd.inc

vsbdmsg.obj: $(RES_DIR)\vsbdmsg.asm
    $(ASM) $(RES_DIR)\vsbdmsg.asm, vsbdmsg.obj;

# END Dependencies 
