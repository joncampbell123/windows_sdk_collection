#############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   Copyright (c) 1992 - 1995  Microsoft Corporation.  All Rights Reserved.
#
#############################################################################
#
#   IMAADP32.ACM -- Audio Compression Manager CODEC for IMA/DVI ADPCM 
#
#   to make a NON-DEBUG build, type the following line:
#       nmake DEBUG=NO
#   just typing 'nmake' will build a DEBUG build
#
#   You can also set the environment variable DEBUG to NO
#
#############################################################################

NAME    = imaadp32
EXT     = acm

GOALS   = $(NAME).$(EXT) $(NAME).sym

# add -Fc if you want to see assembler that is generated: 
OPTZ    = -c -Ox -W3 -WX -Gs -Zp                                 # -Fc
DEFZ    = -D_MT -D_X86_ -DWINVER=0x0400 -DWIN4

OBJS    = codec.obj config.obj imaadpcm.obj debug.obj
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
LN  = link -align:0x1000 $(LNOPT)
MAPSYM = mapsym -nologo

.c.obj:
        $(CC) $*.c


#
#   Rules
#

all: $(GOALS)

$(NAME).$(EXT): $(OBJS) imaadpcm.def $(NAME).res
	@$(LN) @<<
-out:$(NAME).$(EXT)
-machine:i386
-subsystem:windows,3.50
-dll
-noentry
-map:$(NAME).map
-def:imaadpcm.def
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

codec.obj: codec.c codec.h debug.h imaadpcm.h

config.obj: config.c codec.h debug.h muldiv32.h imaadpcm.h

debug.obj: debug.c debug.h

imaadpcm.obj: imaadpcm.c imaadpcm.h codec.h debug.h


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
