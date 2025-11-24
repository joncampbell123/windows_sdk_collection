#
# MAKEFILE
# Patron Chapter 15
#
# Copyright (c)1992-1995 Microsoft Corporation, All Rights Reserved
#

INCLUDE=$(INCLUDE);$(MSTOOLS)\samples\ole\inole2\inc

!include <$(MSTOOLS)\samples\ole\include\olesampl.mak>

DEFFILE = patron.def

TARGET  = patron
OUTFILE = patron15.exe

all: patron15.exe

INCLS    = $(TARGET).H pages.h tenant.h

OBJS1    = $(TARGET).obj client.obj precomp.obj
OBJS2    = document.obj iipsite.obj iipuiwin.obj
OBJS3    = pages.obj pagewin.obj page.obj
OBJS4    = pagemous.obj tenant.obj dragdrop.obj
OBJS5    = idroptgt.obj idropsrc.obj iclisite.obj iadvsink.obj
OBJS6    = iuilink.obj iolecont.obj iperfile.obj iclassf.obj
OBJS     = $(OBJS1) $(OBJS2) $(OBJS3) $(OBJS4) $(OBJS5) $(OBJS6) $(TARGET).res

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
iuilink.obj    : iuilink.cpp $(INCLS)
iolecont.obj  : iolecont.cpp $(INCLS)
iclassf.obj   : iclassf.cpp  $(INCLS)
iperfile.obj  : iperfile.cpp $(INCLS)
iipuiwin.obj  : iipuiwin.cpp $(INCLS)

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
iipsite.obj   : iipsite.cpp $(INCLS)

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

