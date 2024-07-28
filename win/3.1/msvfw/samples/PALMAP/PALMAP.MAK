#############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   Copyright (c) 1992, 1993  Microsoft Corporation.  All Rights Reserved.
#
#############################################################################
#
#   MAKEFILE
#
#   constructs the palette map DLL
#
#   to build a debug version:
#       NMAKE
#   to build a non debug version:
#       NMAKE DEBUG=NO
#
############################################################################

NAME	=palmap
DEFFILE	=$(NAME).def


!if "$(DEBUG)" == "NO"

DEF = 
CC  = cl -c -nologo -W3 -ASw -G2s -GD -DWINDLL -Zdp -Oxws $(DEF)
ASM = masm -Mx -t -DSEGNAME=_TEXT -D?QUIET $(DEF)
LINK= link /L /MAP /NOPACKCODE /NOE /NOD /ONERROR:NOEXE
LIBMAN = lib /NOE

!else

DEF   = -DDEBUG 
MASMOPT = -Zi -DDEBUG
ASM = masm -Mx -t -DSEGNAME=_TEXT -D?QUIET $(DEF) $(MASMOPT)
CC  = cl -c -nologo -W3 -ASw -G2s -GD -DWINDLL -Zdp -Oxws -Zid $(DEF)
LINK	= link /L /MAP /NOPACKCODE /NOE /NOD /CO /LI /ONERROR:NOEXE
LIBMAN = lib /NOE

!endif



OBJ1	= dibmap.obj palmap.obj

OBJ    = $(OBJ1)
LIBS	=libw sdllcew compobj vfw


.c.obj:
	$(CC) $*.c
.asm.obj:
	$(ASM) $*.asm;

###################### goal ######################

goal:  $(NAME).dll $(NAME).lib


##################################################

$(NAME).dll $(NAME).map: $(OBJ) $(NAME).res $(DEFFILE)
   $(LINK) @<<
	$(OBJ),
	$(NAME).dll,
	$(NAME),
	$(LIBS),
	$(NAME).def
<<
	rc -31 $(NAME).res $(NAME).dll
!if "$(DEBUG)" != "NO"
	cvpack -p $(NAME).dll
!endif
   mapsym $(NAME).map


$(NAME).res: $(NAME).rc 
	rc -r $(DEF) $(NAME).rc

$(NAME).rc:  $(NAME).h

$(NAME).lib: $(NAME).dll
	implib $(NAME).lib $(NAME).dll

##################### clean ######################

clean:
	del $(NAME).dll
	del *.obj
	del *.res
	del *.map
	del *.sym
	del *.pdb
	del *.lib

##################### copy #######################
copy:
	copy $(NAME).dll ..\..\bin
	copy $(NAME).sym ..\..\bin
	copy $(NAME).lib ..\..\lib


##################### depend #####################

$(NAME).obj: $(NAME).c $(NAME).h 
dibmap.obj: dibmap.c dibmap.h 

# END Dependencies 
