!IFDEF WIN95_BUILD

#ROOT=\windev
BLDPROJ=MEMPHIS
NODEBUG=1
OBJS=winutb.obj misc.obj dlgutb.obj
LIBS=comctl32.lib
PROJ=winutb
WANT_C1032=TRUE
CLFLAGS=-DW32

INCLUDE = $(ROOT)\dev\msdev\include;$(ROOT)\dev\ntddk\inc;$(ROOT)\wdm\usb\inc;$(ROOT)\wdm\ddk\inc

default: all

!include $(ROOT)\dev\win32c.mk

!ELSE

!INCLUDE $(NTMAKEENV)\makefile.def

!ENDIF
