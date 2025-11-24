#
# MAKEFILE
# Enumerator in C Chapter 3
#
# Copyright (c)1992-1994 Microsoft Corporation, All Rights Reserved
#

INCLUDE=$(INCLUDE);$(SDKROOT)\samples\ole\inole2\inc

!include <$(SDKROOT)\samples\ole\include\olesampl.mak>

DEFFILE = enumc.def

TARGET  = enumc
OUTFILE = enumc.exe

all:$(OUTFILE)

INCLS    = $(TARGET).h

OBJS1    = $(TARGET).obj ienum.obj
OBJS2    = ""
OBJS3    = ""
OBJS4    = ""
OBJS5    = ""
OBJS6    = ""
OBJS     = $(OBJS1) $(TARGET).res

RCFILES  = $(TARGET).ico

LIBS    = bttncur.lib gizmobar.lib ..\..\lib\stastrip.lib ..\..\lib\classmdi.lib ..\..\lib\bookui32.lib

OLE_FLAGS = -DSTRICT

!ifndef NO_DEBUG
OLE_FLAGS = $(OLE_FLAGS) /D_DEBUG /DDEBUG /D_DEBUGTRACE=0
!endif


##### Dependencies #####
$(TARGET).res : $(TARGET).rc $(INCLS) $(RCFILES)

#Application level things
$(TARGET).obj : $(TARGET).c $(INCLS)
ienum.obj    : ienum.c    $(INCLS)

#
# Tool Directives
#
.cpp.obj:
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) $*.cpp

.c.obj:
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) $*.c

.rc.res:
    rc -r -DWIN32 -DDEBUG -I..\..\inc $*.rc

clean:
    -del *.obj
    -del *.res
    -del *.exe

$(OUTFILE): $(OBJS)
    $(link) $(linkdebug) $(guilflags) $(OBJS) -out:$@ $(olelibs) $(LIBS)
