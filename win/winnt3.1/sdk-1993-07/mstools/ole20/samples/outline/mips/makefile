#
# (c) Copyright Microsoft Corp. 1992 All Rights Reserved
#
# Makefile : Builds the OLE 2.0 Outline series sample apps
#        The Outline series includes:
#           Outline.exe -- base version of app (w/o OLE functionality)
#           SvrOutl.exe -- OLE 2.0 Server sample app
#           CntrOutl.exe -- OLE 2.0 Containter (Container) sample app
#        NOTE: apps are built in a subdir of same name as app.
#
# Usage:     NMAKE              (build outline.exe--non-OLE version)
#    or:     NMAKE APP=svroutl  (build svroutl.exe--server-only version)
#    or:     NMAKE APP=cntroutl (build cntroutl.exe--container-only version)
#    or:     NMAKE APP=isvrotl  (build isvrotl.exe--InPlace server version)
#    or:     NMAKE APP=icntrotl (build icntrotl.exe--InPlace container ver.)
#    or:     NMAKE clean        (erase all compiled files in $(APP) dir)
#
# eg.:  nmake APP=cntroutl clean    (deletes compiled files in .\cntroutl\ )
#       nmake                       (builds outline\outline.exe)
#
# Environment variables:
#       DEVROOT_DIR=<path>  (root dir for sample code development)

!if "$(app)" == "svroutl"
APP = svroutl
!endif

!if "$(app)" == "cntroutl"
APP = cntroutl
!endif

!if "$(app)" == "icntrotl"
APP = icntrotl
!endif

!if "$(app)" == "isvrotl"
APP = isvrotl
!endif

!if "$(app)" == "outline" || "$(app)" == ""
APP = outline
!endif

!ifndef DEVROOT_DIR
!error You must define DEVROOT_DIR (ole20 sub-tree)
!endif
!ifndef INCLUDE
!error You must define INCLUDE (non ole20 include files)
!endif
!ifndef LIB
!error You must deinfe LIB (non ole20 libraries)
!endif
!ifndef OLEBUILD
OLEBUILD=NT
!endif

OLE2_H=$(DEVROOT_DIR)\h
OLE2_LIB=$(DEVROOT_DIR)\lib

!ifndef SAMPLE_DIR
SAMPLE_DIR = $(DEVROOT_DIR)\samples
!endif
SRC_DIR   = $(SAMPLE_DIR)\outline

SRC_DIR   = ..

!if "$(OLEBUILD)" == "NT"

!ifndef MACHINE
MACHINE=MIPS
!endif

CPP       = mcl
CPPFLAGS  = /D_MIPS_ /Fc /DOLE2FINAL /D_DEBUG /DWIN32 /DOLE2SHIP /D$(MACHINE)=1 /D_NTWIN /D_WINDOWS /DWINVER=0x030A /W3 /nologo /Z7 /Od $(CL)
LINKFLAGS = -subsystem:windows -machine:$(MACHINE) -debug:mapped,full -debugtype:both
OLELIBS   = ole2w32.lib outlui.lib storag32.lib gizmobar.lib bttncur.lib
W32LIBS	  = kernel32.lib comdlg32.lib user32.lib gdi32.lib  
LIBS      = $(OLELIBS) $(W32LIBS) crtdll.lib 

!else

CPP       =cl
CPPFLAGS  = /f /AM /GA /GEs /W3 /Zp /G2 /nologo /D_DEBUG $(CL) /Od /Zi
LINKFLAGS = /NOD /NOE /batch /ONERROR:NOEXE /COD:N
LIBS      = libw shell ole2 storage compobj outlui commdlg mlibcew \
            gizmobar bttncur $(SRC_DIR)\ctl3d

!endif

BUILD_DIR = $(APP)
PCHFLAGS  = -Yuoutline.h -Fpprecomp.pch

COMMONINCL =    $(SRC_DIR)/outline.h $(OLE2_H)/ole2.h \
    $(SRC_DIR)/frametls.h $(OLE2_H)/compobj.h  \
    $(OLE2_H)/scode.h $(OLE2_H)/dvobj.h \
    $(OLE2_H)/storage.h $(OLE2_H)/moniker.h \
    $(SRC_DIR)/outlrc.h $(SRC_DIR)/status.h $(SRC_DIR)/cntroutl.h  \
    $(SRC_DIR)/cntrrc.h $(OLE2_H)/ole2ui.h $(SRC_DIR)/svroutl.h \
    $(SRC_DIR)/oleoutl.h

!if "$(USE_MSGFILTER)"=="1"
CPPFLAGS  = $(CPPFLAGS) /DUSE_MSGFILTER
!endif


##########################################################################
#
# main obj lists; add new obj files here
#

!if "$(APP)" == "cntroutl" 
CPPFLAGS = $(CPPFLAGS) /DOLE_CNTR
APP_OBJS = precomp.obj main.obj memmgr.obj status.obj frametls.obj \
       dialogs.obj debug.obj \
       outlapp.obj outldoc.obj heading.obj \
       outllist.obj outlline.obj outltxtl.obj \
       outlntbl.obj outlname.obj \
       oleapp.obj oledoc.obj classfac.obj debug2.obj \
       dragdrop.obj clipbrd.obj linking.obj \
       cntrbase.obj cntrline.obj
!else

!if "$(APP)" == "svroutl" 
CPPFLAGS = $(CPPFLAGS) /DOLE_SERVER
APP_OBJS = precomp.obj main.obj memmgr.obj status.obj frametls.obj \
       dialogs.obj debug.obj \
       outlapp.obj outldoc.obj heading.obj \
       outllist.obj outlline.obj outltxtl.obj \
       outlntbl.obj outlname.obj \
       oleapp.obj oledoc.obj classfac.obj debug2.obj \
       dragdrop.obj clipbrd.obj linking.obj \
       svrbase.obj svrpsobj.obj 
!else

!if "$(APP)" == "icntrotl" 
CPPFLAGS = $(CPPFLAGS) /DOLE_CNTR /DINPLACE_CNTR
APP_OBJS = precomp.obj main.obj memmgr.obj status.obj frametls.obj \
       dialogs.obj debug.obj \
       outlapp.obj outldoc.obj heading.obj \
       outllist.obj outlline.obj outltxtl.obj \
       outlntbl.obj outlname.obj \
       oleapp.obj oledoc.obj classfac.obj debug2.obj \
       dragdrop.obj clipbrd.obj linking.obj \
       cntrbase.obj cntrline.obj cntrinpl.obj
!else

!if "$(APP)" == "isvrotl" 
CPPFLAGS = $(CPPFLAGS) /DOLE_SERVER /DINPLACE_SVR
APP_OBJS = precomp.obj main.obj memmgr.obj status.obj frametls.obj \
       dialogs.obj debug.obj \
       outlapp.obj outldoc.obj heading.obj \
       outllist.obj outlline.obj outltxtl.obj \
       outlntbl.obj outlname.obj \
       oleapp.obj oledoc.obj classfac.obj debug2.obj \
       dragdrop.obj clipbrd.obj linking.obj \
       svrbase.obj svrpsobj.obj svrinpl.obj
!else
APP_OBJS = precomp.obj main.obj memmgr.obj status.obj frametls.obj \
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
# overall goal; cd to build dir; build application; return to src dir
#
goal: cd_build $(APP).exe

cd_build:
    cd $(BUILD_DIR)
    set INCLUDE=$(OLE2_H);$(INCLUDE)
    set LIB=$(OLE2_LIB);$(LIB)
    set cl=$(CPPFLAGS)


##########################################################################
#
# create precomiled header
#
precomp.pch precomp.obj : $(SRC_DIR)/precomp.c $(COMMONINCL)
    @echo Precompiling outline.h ...
    $(CPP) -c -Ycoutline.h -Fpprecomp.pch -Foprecomp $(SRC_DIR)\precomp.c

##########################################################################
#
# link/res commands

$(APP).exe: $(APP_OBJS) $(SRC_DIR)\$(APP).def $(APP).res precomp.pch
    @echo Linking $@...
!if "$(OLEBUILD)" == "NT"
      cvtres -$(MACHINE) $(APP).res -o $(APP).rs
      link32 $(LINKFLAGS) -entry:WinMainCRTStartup $(APP_OBJS) $(APP).rs -out:$@ -map:$*.map $(LIBS) 
!else
    link $(LINKFLAGS) @<<$(APP).lnk
$(APP_OBJS: = +^
)
$@,
$(APP).map /MAP:FULL /LINE,
$(LIBS),
$(DEVROOT_DIR)\outline\$(APP).def
<<KEEP
    mapsym $(APP).map
    @echo Adding resources to $@...
    rc -k -t -i$(SRC_DIR) $(RCFLAGS) $*.res $@
    @echo Packing symbols for $@...
!if "$(DONTUSE_CVPACK)"==""
    cvpack $@
!endif
!endif

$(APP).res: $(SRC_DIR)\$(APP).rc $(SRC_DIR)/outlrc.h $(SRC_DIR)/cntrrc.h \
    $(SRC_DIR)\dialogs.dlg $(SRC_DIR)/debug.rc
    @echo Compiling resources...
!if "$(OLEBUILD)" == "NT"
    rc -r -DWIN32 -i$(SRC_DIR) $(RCFLAGS) -fo$@ $(SRC_DIR)\$(APP).rc
!else
    rc -r -i$(SRC_DIR) $(RCFLAGS) -fo$@ $?
!endif


##########################################################################
#
# build rules for src directory
#

{$(SRC_DIR)}.c{}.obj:
    @echo Compiling $<...
    $(CPP) $(CPPFLAGS) $(PCHFLAGS) /c $<


##########################################################################
#
# clean (erase) generated files

clean: 
    -del $(BUILD_DIR)\*.obj
    -del $(BUILD_DIR)\$(APP).res
    -del $(BUILD_DIR)\$(APP).exe
    -del $(BUILD_DIR)\$(APP).sym
    -del $(BUILD_DIR)\$(APP).map
    -del $(BUILD_DIR)\$(APP).lnk



#########################################################
# Dependencies
#########################################################

main.obj : $(COMMONINCL) 
    $(CPP) $(CPPFLAGS) /c $(SRC_DIR)/main.c

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

status.obj : $(COMMONINCL) $(SRC_DIR)/message.h $(SRC_DIR)/status.h

memmgr.obj : 
    $(CPP) $(CPPFLAGS) /c $(SRC_DIR)/memmgr.c

frametls.obj : $(COMMONINCL) $(OLE2_H)/ole2ui.h

heading.obj : $(COMMONINCL)

dialogs.obj : $(COMMONINCL)

debug.obj : $(COMMONINCL) $(OLE2_H)/msgfiltr.h 

debug2.obj : $(COMMONINCL)
