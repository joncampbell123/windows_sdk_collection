#
# Makefile : Builds the OLE 2.0 Outline series sample apps
#

!include <$(SDKROOT)\samples\ole\include\olesampl.mak>

all: $(APP).exe

OLE_FLAGS = -DOLE2FINAL -DWIN32S

!ifndef NO_DEBUG
OLE_FLAGS = $(OLE_FLAGS) -DDBG -D_DEBUG -DOLE2SHIP
!endif

OLELIBS   = ole32.lib uuid.lib oleaut32.lib ole2ui.lib gizmobar.lib bttncur.lib

PCHFLAGS  = -Yuoutline.h -Fp$(APP).pch

COMMONINCL =    outline.h frametls.h outlrc.h status.h cntroutl.h cntrrc.h svroutl.h oleoutl.h

!if "$(USE_MSGFILTER)"=="1"
OLE_FLAGS  = $(OLE_FLAGS) /DUSE_MSGFILTER
!endif

PRECOMPOBJ = $(APP).obj


##########################################################################
#
# main obj lists; add new obj files here
#

!if "$(APP)" == "cntroutl"
OLE_FLAGS = $(OLE_FLAGS) /DOLE_CNTR
APP_OBJS = main.obj memmgr.obj status.obj frametls.obj \
       dialogs.obj debug.obj \
       outlapp.obj outldoc.obj heading.obj \
       outllist.obj outlline.obj outltxtl.obj \
       outlntbl.obj outlname.obj \
       oleapp.obj oledoc.obj classfac.obj debug2.obj \
       dragdrop.obj clipbrd.obj linking.obj \
       cntrbase.obj cntrline.obj
!else

!if "$(APP)" == "svroutl"
OLE_FLAGS = $(OLE_FLAGS) /DOLE_SERVER
APP_OBJS = main.obj memmgr.obj status.obj frametls.obj \
       dialogs.obj debug.obj \
       outlapp.obj outldoc.obj heading.obj \
       outllist.obj outlline.obj outltxtl.obj \
       outlntbl.obj outlname.obj \
       oleapp.obj oledoc.obj classfac.obj debug2.obj \
       dragdrop.obj clipbrd.obj linking.obj \
       svrbase.obj svrpsobj.obj
!else

!if "$(APP)" == "icntrotl"
OLE_FLAGS = $(OLE_FLAGS) /DOLE_CNTR /DINPLACE_CNTR
APP_OBJS = main.obj memmgr.obj status.obj frametls.obj \
       dialogs.obj debug.obj \
       outlapp.obj outldoc.obj heading.obj \
       outllist.obj outlline.obj outltxtl.obj \
       outlntbl.obj outlname.obj \
       oleapp.obj oledoc.obj classfac.obj debug2.obj \
       dragdrop.obj clipbrd.obj linking.obj \
       cntrbase.obj cntrline.obj cntrinpl.obj
!else

!if "$(APP)" == "isvrotl"
OLE_FLAGS = $(OLE_FLAGS) /DOLE_SERVER /DINPLACE_SVR
APP_OBJS = main.obj memmgr.obj status.obj frametls.obj \
       dialogs.obj debug.obj \
       outlapp.obj outldoc.obj heading.obj \
       outllist.obj outlline.obj outltxtl.obj \
       outlntbl.obj outlname.obj \
       oleapp.obj oledoc.obj classfac.obj debug2.obj \
       dragdrop.obj clipbrd.obj linking.obj \
       svrbase.obj svrpsobj.obj svrinpl.obj
!else
APP_OBJS = main.obj memmgr.obj status.obj frametls.obj \
       dialogs.obj debug.obj \
       outlapp.obj outldoc.obj heading.obj \
       outllist.obj outlline.obj outltxtl.obj debug2.obj \
       outlntbl.obj outlname.obj

!endif
!endif
!endif
!endif


##########################################################################
#
# create precomiled header
#
$(APP).pch : $(APP).c $(COMMONINCL)
    @echo Precompiling outline.h ...
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) -Ycoutline.h -Fp$(APP).pch -Fo$(APP) $(APP).c

##########################################################################
#
# link/res commands

$(APP).exe: $(APP).pch $(APP_OBJS) $(APP).def $(APP).res ..\lib\gizmobar.lib ..\lib\bttncur.lib
    $(link) $(linkdebug) $(guilflags) $(PRECOMPOBJ) $(APP_OBJS) $(APP).res -out:$@ -map:$*.map $(OLELIBS) $(guilibsdll) advapi32.lib shell32.lib
    if not exist ..\bin mkdir ..\bin
    copy $(APP).exe ..\bin

$(APP).res: $(APP).rc outlrc.h cntrrc.h dialogs.dlg debug.rc
    rc -r -DWIN32 $(RCFLAGS) -fo$@ $(APP).rc


##########################################################################
#
# build rules for src directory
#

.c.obj:
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) $(PCHFLAGS) $*.c


##########################################################################
#
# clean (erase) generated files

clean:
    -del *.obj
    -del *.res
    -del *.exe
    -del *.map
    -del *.pch


#########################################################
# Dependencies
#########################################################

main.obj : $(COMMONINCL)
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS) main.c

outlapp.obj : $(COMMONINCL)

outldoc.obj : $(COMMONINCL)

outllist.obj : $(COMMONINCL)

outlline.obj : $(COMMONINCL)

outltxtl.obj : $(COMMONINCL)

outlntbl.obj : $(COMMONINCL)

outlname.obj : $(COMMONINCL)

classfac.obj : $(COMMONINCL)

oleapp.obj : $(COMMONINCL)

oledoc.obj : $(COMMONINCL)

dragdrop.obj : $(COMMONINCL)

clipbrd.obj : $(COMMONINCL)

linking.obj : $(COMMONINCL)

cntrbase.obj : $(COMMONINCL)

cntrline.obj : $(COMMONINCL)

cntrinpl.obj : $(COMMONINCL)

svrpsobj.obj : $(COMMONINCL)

svrinpl.obj : $(COMMONINCL)

svrbase.obj : $(COMMONINCL)

status.obj : $(COMMONINCL) message.h status.h

memmgr.obj :
    $(cc) $(cflags) $(cvars) $(cdebug) $(OLE_FLAGS)  memmgr.c

frametls.obj : $(COMMONINCL)

heading.obj : $(COMMONINCL)

dialogs.obj : $(COMMONINCL)

debug.obj : $(COMMONINCL)

debug2.obj : $(COMMONINCL)
