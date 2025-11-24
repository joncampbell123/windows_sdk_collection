#############################################################################
#
#   THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
#   KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
#   PURPOSE.
#
#   Copyright (c) 1993-1995  Microsoft Corporation.  All Rights Reserved.
#
#   makefile
#
#   constructs the pen calibration dll
#
#   to build a debug version:
#       NMAKE
#   to build a non debug version:
#       NMAKE DEBUG=NO
#
############################################################################

NAME=pencal

# compiler/asembler
ASM = ml
CL = cl

# C7 Macro
C7 = 0

# compile options
AFLAGS= -DMEDIUM -DIS_16 -nologo -W3 -Zd -c -Cx -DMASM6 -Sg
CFLAGS= -G2c -DNOWINDOWSX -nologo -W3 -Zdp -Gs -c -DIS_16 -Zl
LFLAGS = /ALIGN:16 /NOD /PACKDATA /ON:N /L /MAP /NOLOGO /NOPACKCODE /NOE

!if "$(DEBUG)" == "NO"
ADEBUG = -DDEBLEVEL=0
CDEBUG =
!else
ADEBUG = -DDEBUG -DDEBLEVEL=1 -Zi
CDEBUG = -DDEBUG -Od -Zi
!endif

AFLAGS = $(AFLAGS) $(ADEBUG)
CFLAGS = $(CFLAGS) $(CDEBUG)

SRCS=calinit.c caldlg.c calerr.c
ASMS=umuldiv.asm

LIBS=libw mdllcew oldnames penwin


goal: $(NAME).dll


# assembly files
.asm.obj:
      $(ASM) $(AFLAGS) $*.asm

# C files
.c.obj:
		$(CL) $(CFLAGS) $*.c

# $(NAME).res:
.rc.res:
		$(RC) /r $(NAME).rc

$(NAME).dll: $(ASMS:.asm=.obj) $(SRCS:.c=.obj) $(NAME).res 
		link @<<
		$(ASMS:.asm=.obj) $(SRCS:.c=.obj) $(OBJFILES),
      $(NAME).dll $(LFLAGS)
		$(NAME).map,
		$(LIBS),
		$(NAME).def
<<
 		$(RC) -fe $(NAME).dll $(NAME).res 
# 		$(RC) -T -40 $(NAME).res $(NAME).dll
!if !("$(DEBUG)" == "NO")
    mapsym $(NAME).map
!endif

clean:
		-@del *.obj
		-@del *.map
		-@del *.res
		-@del *.dll
		-@del *.sym
