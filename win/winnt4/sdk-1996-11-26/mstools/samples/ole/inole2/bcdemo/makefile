#
# MAKEFILE
# Buttons & Cursors Demo
#
# Copyright (c)1992-1996 Microsoft Corporation, All Rights Reserved
#

INCLUDE=$(INCLUDE);$(MSTOOLS)\samples\ole\inole2\inc

!include <$(MSTOOLS)\samples\ole\include\olesampl.mak>

DEFFILE = bcdemo.def

TARGET  = bcdemo
OUTFILE = bcdemo.exe

all:$(TARGET).exe

INCLS    = $(TARGET).h

OBJS1    = $(TARGET).obj init.obj
OBJS2    = ""
OBJS3    = ""
OBJS4    = ""
OBJS5    = ""
OBJS6    = ""
OBJS     = $(OBJS1)

RCFILES  = $(TARGET).ico appim72.bmp appim96.bmp appim120.bmp

LIBS    = bttncur.lib

OLE_FLAGS = -DWIN32S -I..\inc -DSTRICT

!ifndef NO_DEBUG
OLE_FLAGS = $(OLE_FLAGS) /D_DEBUG /DDEBUG /D_DEBUGTRACE=0
!endif


##### Dependencies #####

$(TARGET).obj : $(TARGET).c  $(INCLS)
init.obj      : init.c       $(INCLS)
$(TARGET).res : $(TARGET).rc $(INCLS) $(RCFILES)

#
# Tool Directives
#
.c.obj:
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) $*.c

.rc.res:
    rc -r -DWIN32 -DDEBUG -I..\inc $*.rc

clean:
    -del *.obj
    -del *.res
    -del *.exe

$(TARGET).exe: $(OBJS) $(TARGET).res
    $(link) $(linkdebug) $(guilflags) $(OBJS) $(TARGET).res -out:$@ $(guilibs) $(LIBS)
    if not exist $(MSTOOLS)\samples\ole\bin mkdir $(MSTOOLS)\samples\ole\bin
    copy $@ $(MSTOOLS)\samples\ole\bin

