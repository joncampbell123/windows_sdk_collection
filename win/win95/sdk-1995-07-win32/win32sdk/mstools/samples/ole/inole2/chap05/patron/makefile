#
# MAKEFILE
# Patron Chapter 5
#
# Copyright (c)1992-1995 Microsoft Corporation, All Rights Reserved
#

INCLUDE=$(INCLUDE);$(MSTOOLS)\samples\ole\inole2\inc

!include <$(MSTOOLS)\samples\ole\include\olesampl.mak>

DEFFILE = patron.def

TARGET  = patron
OUTFILE = patron05.exe

all: patron05.exe

INCLS    = $(TARGET).H resource.h

OBJS1    = $(TARGET).obj client.obj precomp.obj
OBJS2    = document.obj
OBJS3    = pages.obj pagewin.obj page.obj
OBJS4    = ""
OBJS5    = ""
OBJS6    = ""
OBJS     = $(OBJS1) $(OBJS2) $(OBJS3) $(TARGET).res

RCFILES1 = $(TARGET).ico document.ico about.dlg print.dlg
RCFILES2 = gizmo72.bmp gizmo96.bmp gizmo120.bmp
RCFILES  = $(RCFILES1) $(RCFILES2)

LIBS    = bttncur.lib gizmobar.lib ..\..\lib\stastrip.lib ..\..\lib\classmdi.lib olestd.lib oledlg.lib

OLE_FLAGS = -DSTRICT

!ifndef NO_DEBUG
OLE_FLAGS = $(OLE_FLAGS) /D_DEBUG /DDEBUG /D_DEBUGTRACE=0
!endif


##### Dependencies #####
$(TARGET).res : $(TARGET).rc $(INCLS) $(RCFILES)

#Application level things
$(TARGET).obj : $(TARGET).cpp $(INCLS)
client.obj    : client.cpp    $(INCLS)

#Document level things
document.obj  : document.cpp $(INCLS)

#Editor level things
pages.obj     : pages.cpp     $(INCLS) pages.h
pagewin.obj   : pagewin.cpp   $(INCLS) pages.h
page.obj      : page.cpp      $(INCLS) pages.h

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

$(OUTFILE): $(OBJS) $(TARGET).res
    $(link) $(linkdebug) $(guilflags) $(OBJS) -out:$@ $(olelibs) $(LIBS)
    if not exist $(MSTOOLS)\samples\ole\bin mkdir $(MSTOOLS)\samples\ole\bin
    copy $@ $(MSTOOLS)\samples\ole\bin

