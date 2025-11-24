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

NAME=pen

# compiler/assembler
ASM   = ml
CL = cl

# Misc

KEEPFLAG=KEEP

# compile options

LFLAGS = /NOD /BATCH /ON:N /L /MAP /NOLOGO /NOPACKCODE /NOE /ALIGN:16
CFLAGS=-AMw -GD2c -nologo -W3 -Gs -c -DIS_16 -DNOWINDOWSX

!if "$(DEBUG)" == "NO"
CDEBUG = -Os
!else
CDEBUG = -DDEBUG -Od -Zi -Fc -Zdp
!endif

CFLAGS = $(CFLAGS) $(CDEBUG)

SRCS=dbgutils.c interupt.c drvrproc.c
LIBS=libw mdllcew oldnames

goal: $(NAME)c.drv

# C files
.c.obj:
      $(CL) $(CFLAGS) $*.c


$(NAME)c.drv: $(SRCS:.c=.obj)
      link @<<$(NAME)c.lnk
      $(SRCS:.c=.obj),
      $(NAME)c.drv $(LFLAGS)
      $(NAME).map,
      $(LIBS),
      $(NAME).def
<<$(KEEPFLAG)
    $(RC) -T -40 $(NAME)c.drv
!if !("$(DEBUG)" == "NO")
    mapsym -s -o $(NAME)c.sym $(NAME).map
!endif

clean:
      -@del *.obj
      -@del *.map
      -@del *.drv
      -@del *.sym
      -@del *.lst
      -@del *.cod
      -@del *.pdb
