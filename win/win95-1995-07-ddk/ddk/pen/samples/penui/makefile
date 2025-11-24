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
#   constructs pen driver
#
#   to build a debug version:
#       NMAKE
#   to build a non debug version:
#       NMAKE DEBUG=NO
#
############################################################################

NAME=penui

# compiler/assembler
ASM   = ml
CL = cl

# Misc

KEEPFLAG=KEEP

# compile options

CFLAGS=-G2c -DNOWINDOWSX -nologo -W3 -Zdp -Gs -c -DIS_16 -Zl
LFLAGS = /NOD /BATCH /ON:N /L /MAP /NOLOGO /NOPACKCODE /NOE /ALIGN:16

!if "$(DEBUG)" == "NO"
CDEBUG = -Os
!else
CDEBUG = -DDEBUG -Od -Zi
!endif

CFLAGS = $(CFLAGS) $(CDEBUG)

SRCS=exports.c win40dlg.c
LIBS=libw mdllcew oldnames

goal: $(NAME).dll

# C files
.c.obj:
      $(CL) $(CFLAGS) $*.c

.rc.res:
      $(RC) /r $(NAME).rc

$(NAME).dll: $(SRCS:.c=.obj) $(NAME).res
      link @<<$(NAME).lnk
      $(SRCS:.c=.obj) $(OBJFILES),
      $(NAME).dll $(LFLAGS)
      $(NAME).map,
      $(LIBS),
      $(NAME).def
<<$(KEEPFLAG)
      $(RC) -T -40 $(NAME).res $(NAME).dll
!if !("$(DEBUG)" == "NO")
    mapsym $(NAME).map
!endif

clean:
      -@del *.obj
      -@del *.map
      -@del *.res
      -@del *.dll
      -@del *.sym
      -@del *.lst
      -@del *.cod
      -@del *.pdb
