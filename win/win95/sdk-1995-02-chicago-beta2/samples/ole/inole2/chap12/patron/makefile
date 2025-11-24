#
# MAKEFILE
# Patron Chapter 12
#
# Copyright (c)1992-1994 Microsoft Corporation, All Rights Reserved
#

INCLUDE=$(INCLUDE);$(SDKROOT)\samples\ole\inole2\inc

!include <$(SDKROOT)\samples\ole\include\olesampl.mak>

DEFFILE = patron.def

TARGET  = patron
OUTFILE = patron12.exe

all: patron12.exe

INCLS    = $(TARGET).H pages.h tenant.h

OBJS1    = $(TARGET).obj client.obj precomp.obj
OBJS2    = document.obj
OBJS3    = pages.obj pagewin.obj page.obj
OBJS4    = pagemous.obj tenant.obj dragdrop.obj
OBJS5    = idroptgt.obj idropsrc.obj iclisite.obj iadvsink.obj
OBJS6    = iuilink.obj
OBJS     = $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) $(OBJS6) $(TARGET).res

RCFILES1 = $(TARGET).ico document.ico about.dlg print.dlg
RCFILES2 = gizmo72.bmp gizmo96.bmp gizmo120.bmp
RCFILES  = $(RCFILES1) $(RCFILES2)

LIBS    = bttncur.lib gizmobar.lib ..\..\lib\stastrip.lib ..\..\lib\classmdi.lib ..\..\lib\bookui32.lib

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
iuilink.obj   : iuilink.cpp  $(INCLS)

#Editor level things
pages.obj     : pages.cpp    $(INCLS)
pagewin.obj   : pagewin.cpp  $(INCLS)
page.obj      : page.cpp     $(INCLS)
pagemous.obj  : pagemous.cpp $(INCLS)

dragdrop.obj  : dragdrop.cpp $(INCLS)
idroptgt.obj  : idroptgt.cpp $(INCLS)
idropsrc.obj  : idropsrc.cpp $(INCLS)

tenant.obj    : tenant.cpp   $(INCLS)
iclisite.obj  : iclisite.cpp $(INCLS)
iadvsink.obj  : iadvsink.cpp $(INCLS)

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
    $(link) $(linkdebug) $(guilflags) $(OBJS) -out:$@ $(olelibs) $(LIBS) $(TARGET).res
