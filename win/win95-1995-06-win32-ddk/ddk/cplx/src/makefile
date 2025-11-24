#****************************************************************************
#                                                                           *
# THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
# KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
# IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
# PURPOSE.                                                                  *
#                                                                           *
# Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
#                                                                           *
#****************************************************************************

# Makefile for CPLX - Control Panel extensions for Mouse/Display

NAME     = cplx
CFLAGS   = -W3 -Zl -c -Oxs -G3fyz -DBUILDDLL -DWIN32 -D_X86_ -DIS_32 -bzalign
RCFLAGS  = -DWIN32
L32FLAGS = -merge:.bss=.data -subsystem:windows,4.0 -entry:LibMain@12 
L32FLAGS = $(L32FLAGS) -align:0x1000 -nodefaultlib -merge:.rdata=.text

!IFDEF DEBUG
CFLAGS   = $(CFLAGS) -DDEBLEVEL=1 -DDEBUG
RCFLAGS  = $(RCFLAGS) -DDEBUG
L32FLAGS = $(L32FLAGS) -debug:full -debugtype:cv
!ELSE
CFLAGS   = $(CFLAGS) -DDEBLEVEL=0
!ENDIF

.c.obj:
       set CL=$(CFLAGS)
       cl $<

.rc.res:
       rc $(RCFLAGS) -r $*.rc

cplx.dll: cplx.obj defclsf.obj cplx.def cplx.res
        link @<<$(NAME).lnk
$(L32FLAGS)
-out:$(NAME).dll -map:$(NAME).map -def:$(NAME).def -dll -base:0x410000
cplx.obj defclsf.obj kernel32.lib user32.lib gdi32.lib advapi32.lib 
comctl32.lib ole32.lib libcmt.lib $(NAME).res
<<
        mapsym -s $(NAME).map

$(NAME).obj: $(NAME).c

defclsf.obj: defclsf.c

$(NAME).res: $(NAME).rc

clean:
        del *.res
        del *.dll
        del *.obj
        del *.exp
        del *.lib
        del *.sym
        del *.map
