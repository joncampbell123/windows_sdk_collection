#############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   Copyright (c) 1992 - 1995	Microsoft Corporation.	All Rights Reserved.
#
#############################################################################
#
#   MSGSM610.ACM -- Audio Compression Manager CODEC for GSM 6.10
#
#   to make a NON-DEBUG build, type the following line:
#       nmake DEBUG=NO
#   just typing 'nmake' will build a DEBUG build
#
#   You can also set the environment variable DEBUG to NO
#
#############################################################################

NAME    = msgsm32
EXT     = acm

GOALS   = $(NAME).$(EXT) $(NAME).sym

# add -Fc if you want to see assembler that is generated: 
OPTZ    = -c -Ox -W3 -WX -Gs -Zp                                 # -Fc
DEFZ    = -D_MT -D_X86_ -DWINVER=0x0400 -DWIN4

OBJS    = init.obj codec.obj config.obj gsm610.obj gsm61032.obj debug.obj
LIBS	= user32.lib kernel32.lib winmm.lib advapi32.lib gdi32.lib

!IF "$(DEBUG)" == "NO"

DEF     = $(DEFZ)
CCOPT   =
ASOPT   =
RCOPT   =
LNOPT   =-debug:none

!ELSE

DEF     = -DDEBUG $(DEFZ)
CCOPT   = -Zi
ASOPT   = -Zi
RCOPT   =
LNOPT   = -debug:full -debugtype:cv

!ENDIF

CC  = cl -DIS_32 $(DEF) $(OPTZ) $(CCOPT) -Fo$@
RC  = rc -DIS_32 $(DEF) -r $(RCOPT) -I..\include
ASM = ml -coff -DIS_32 -c -Cx -Zd -W3 -WX $(ASOPT) $(DEF) -Fo $@
LN  = link -align:0x1000 $(LNOPT)
MAPSYM = mapsym -nologo

.c.obj:
        $(CC) $*.c

.asm.obj:
        $(ASM) $*.asm


#
#   Rules
#

all: $(GOALS)

$(NAME).$(EXT): $(OBJS) msgsm610.def $(NAME).res
	@$(LN) @<<
-out:$(NAME).$(EXT)
-machine:i386
-subsystem:windows,3.50
-dll
-noentry
-map:$(NAME).map
-def:msgsm610.def
$(NAME).res
$(LIBS)
$(OBJS)
<<

$(NAME).map: $(NAME).$(EXT)

$(NAME).sym: $(NAME).map
	$(MAPSYM) -mo $@ $*.map

$(NAME).res: codec.rc codec.rcv
	$(RC) -fo$@ codec.rc


#
#   Dependencies
#

init.obj: init.c codec.h debug.h

codec.obj: codec.c codec.h debug.h gsm610.h

config.obj: config.c codec.h debug.h gsm610.h

debug.obj: debug.c debug.h

gsm610.obj: gsm610.c gsm610.h codec.h debug.h

gsm61032.obj: gsm61032.asm gsm610.inc


#
#   Misc.
#
clean:
        -@del $(NAME).$(EXT)
        -@del *.cod
        -@del *.err
        -@del *.res
        -@del *.obj
        -@del *.map
        -@del *.sym
        -@del *.pdb
        -@del *.lib
        -@del *.exp
