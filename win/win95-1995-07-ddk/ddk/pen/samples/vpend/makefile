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
#   constructs virtual pen driver
#
#   to build a debug version:
#       NMAKE
#   to build a non debug version:
#       NMAKE DEBUG=NO
#
############################################################################

NAME=vpend

# compiler/asembler
ASM   =ml
CL    =cl

# Misc

KEEPFLAG=KEEP

# compile options

AFLAGS=-coff -DBLD_COFF -DIS_32 -W2 -Zd -c -Cx -DMASM6 -Sg -DVxD /Fl -DUNIDRV
CFLAGS=-DIS_32 -Zdp -Gs -c -bzalign -Zl -Fc -DUNIDRV
LFLAGS=/VXD /NOD

!if "$(DEBUG)" == "NO"
ADEBUG=-DDEBLEVEL=0
CDEBUG=-Os -DDEBLEVEL=0
!else
#  For full debug information on VxD use the following...
# -DDEBLEVEL=2
ADEBUG=-DDEBUG -DDEBLEVEL=1 -DDBG=1
CDEBUG=-DDEBUG -DDEBLEVEL=1 -DDBG=1
LDEBUG=
!endif

AFLAGS=$(AFLAGS) $(ADEBUG)
CFLAGS=$(CFLAGS) $(CDEBUG)
LFLAGS=$(LFLAGS) $(LDEBUG)

SRCS_C=apiproc.c apiprocs.c concerto.c dbg_util.c hwint.c initutil.c\
miscutil.c oemapi.c oembios.c oemhwint.c oemserl.c timercb.c

SRCS_A=util.asm vpend.asm

goal: $(NAME)c.vxd $(NAME)c.sym

# assembly files
.asm.obj:
      $(ASM) $(AFLAGS) $*.asm

# C files
.c.obj:
      $(CL) $(CFLAGS) $*.c

# target

$(NAME)c.vxd: $(SRCS_C:.c=.obj) $(SRCS_A:.asm=.obj)
      link @<<$(NAME)c.lnk
$(LFLAGS)
/OUT:$(NAME)c.vxd
/MAP:$(NAME)c.map
/DEF:$(NAME).def
$(SRCS_C:.c=.obj)
$(SRCS_A:.asm=.obj)
vxdwraps.clb
<<$(KEEPFLAG)

$(NAME)c.sym: $(NAME)c.map
      mapsym -s -o $(NAME)c.sym $(NAME)c.map

clean:
      -@del *.obj
      -@del *.map
      -@del *.vxd
      -@del *.sym
      -@del *.lst
      -@del *.cod
