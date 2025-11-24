####
#makefile - makefile for browseh.dll
#
#       Copyright (C) 1994, Microsoft Corporation
#
#Purpose:
#  Builds the Inproc OLE 2.0 Automation object, browseh.dll.
#
#
#  Usage: NMAKE                 ; build with defaults
#     or: NMAKE option          ; build with the given option(s)
#     or: NMAKE clean           ; erase all compiled files
#
#     option: dev = [win16 | win32]    ; dev=win32 is the default
#             DEBUG=[0 | 1]            ; DEBUG=1 is the default
#             HOST=[DOS | NT | WIN95]  ; HOST=DOS (for win16)
#                                      ; HOST=NT (for win32 on NT)
#                                      ; HOST=WIN95 (for win32 on Win95)
#
#Notes:
#  This makefile assumes that the PATH, INCLUDE and LIB environment
#  variables are setup properly.
#
##############################################################################


##########################################################################
#
# Default Settings
# 

!if "$(dev)" == ""
dev = win32 
HOST = WIN95
!endif

!if !("$(dev)" == "win16" || "$(dev)" == "win32")
!error Invalid dev option, choose from [win16 | win32]
!endif

!if "$(dev)" == "win16"
TARGET  = WIN16
!if "$(HOST)" == ""
HOST  = DOS
!endif
!endif

!if "$(dev)" == "win32"
TARGET  = WIN32
!if "$(HOST)" == ""
HOST  = WIN95
!endif
!endif

!ifdef NODEBUG
DEBUG = 0
!endif

!if "$(DEBUG)" == "0"
NODEBUG = 1
!endif

!if "$(DEBUG)" == ""
DEBUG = 1
!endif


##########################################################################
#
# WIN16 Settings
#
!if "$(TARGET)" == "WIN16"

CC   = cl
LINK = link
!if "$(HOST)" == "DOS"
WX   = wx /w 
!else
WX   =
!endif

RCFLAGS = -dWIN16
CFLAGS = -c -W3 -AM -GD -DWIN16
LINKFLAGS = /NOD /BATCH /ONERROR:NOEXE 

LIBS = libw.lib mdllcew.lib

!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS) -Od -Zi -D_DEBUG $(CL)
LINKFLAGS = $(LINKFLAGS) /COD
!else
CFLAGS = $(CFLAGS) -Ox $(CL)
LINKFLAGS = $(LINKFLAGS) /FAR /PACKC
!endif
!endif


##########################################################################
#
# WIN32 Settings
#
!if "$(TARGET)" == "WIN32"


WX = 

!include <$(MSTOOLS)\samples\ole\include\olesampl.mak>

CC = $(cc)
CFLAGS = $(cflags) $(cvarsmt) -DINC_OLE2 $(cdebug)

!if "$(HOST)" == "NT"
CFLAGS = $(CFLAGS) -DUNICODE
!endif

!ifndef NODEBUG
CFLAGS = $(CFLAGS) -D_DEBUG
!endif

LINK = $(link)
LINKFLAGS = $(linkdebug) $(guilflags) -dll -entry:_DllMainCRTStartup$(DLLENTRY)\
      -export:DllGetClassObject -export:DllCanUnloadNow
RCFLAGS = -DWIN32

!endif

##########################################################################
#
# Build rules
#

.cpp.obj:
    @echo Compiling $<...
    $(CC) $<

.c.obj:
    @echo Compiling $<...
    $(CC) $<


##########################################################################
#
# Application Settings
#

APPS = browseh


!if "$(TARGET)" == "WIN16"
LIBS = $(LIBS) ole2.lib compobj.lib ole2disp.lib typelib.lib 
!endif
!if "$(TARGET)" == "WIN32"
LIBS = $(ole2libsmt)
!endif

OBJS = alias.obj main.obj mydisp.obj browseh.obj browsecf.obj collect.obj \
       enumvar.obj typelib.obj typeinfo.obj intface.obj dispface.obj module.obj \
       coclass.obj function.obj property.obj param.obj enum.obj constant.obj \
       type.obj union.obj struct.obj   



##########################################################################
#
# Default Goal
#

goal : setflags $(APPS).dll

setflags :
    set CL=$(CFLAGS)


##########################################################################
#
# Clean (erase) generated files
#
clean :
    if exist *.obj       del *.obj
    if exist $(APPS).dll del $(APPS).dll
    if exist $(APPS).tlb del $(APPS).tlb
    if exist $(APPS).map del $(APPS).map
    if exist $(APPS).res del $(APPS).res
    if exist tlb.h       del tlb.h
    if exist *.log       del *.log
    if exist *.pdb       del *.pdb


##########################################################################
#
# Application Build (WIN16 Specific)
#

!if "$(TARGET)" == "WIN16"
$(APPS).dll : $(OBJS) $(APPS).def $(APPS).res 
    link @<< 
$(LINKFLAGS)+ 
$(OBJS),
$@,,
$(LIBS),
$(APPS).def
<<
    rc -k -t $(APPS).res $@
!endif


##########################################################################
#
# Application Build (WIN32 Specific)
#
!if "$(TARGET)" == "WIN32"
$(APPS).dll : $(OBJS) $(APPS).def $(APPS).res 
      $(LINK) @<< 
        $(LINKFLAGS)
        -out:$@ 
        -map:$*.map
        $(OBJS)
        $(APPS).res
        $(LIBS)
<<
!endif


##########################################################################
#
# Application Build (Common)
#

$(APPS).res : $(APPS).rc
    rc $(RCFLAGS) -r -fo$@ $?


##########################################################################
#
# Dependencies
#

mydisp.tlb :: mydisp.odl 
     if exist mydisp.tlb  del mydisp.tlb 
     $(WX) mktyplib /D$(TARGET) /o mydisp.log /tlb mydisp.tlb mydisp.odl
     type mydisp.log

tlb.h : browseh.odl mydisp.tlb
     if exist tlb.h  del tlb.h
     if exist browseh.tlb  del browseh.tlb
     $(WX) mktyplib /D$(TARGET) /h tlb.h /o browseh.log /tlb browseh.tlb browseh.odl
     type browseh.log

main.obj : main.cpp browseh.h mydisp.h tlb.h
     $(CC) main.cpp 
mydisp.obj : mydisp.cpp browseh.h mydisp.h tlb.h
     $(CC) mydisp.cpp     
browseh.obj : browseh.cpp browseh.h mydisp.h tlb.h
     $(CC) browseh.cpp
browsecf.obj : browsecf.cpp browseh.h mydisp.h tlb.h
     $(CC) browsecf.cpp
typelib.obj : typelib.cpp browseh.h mydisp.h tlb.h
     $(CC) typelib.cpp     
typeinfo.obj : typeinfo.cpp browseh.h mydisp.h tlb.h
     $(CC) typeinfo.cpp
intface.obj : intface.cpp browseh.h mydisp.h tlb.h
     $(CC) intface.cpp
dispface.obj : dispface.cpp browseh.h mydisp.h tlb.h
     $(CC) dispface.cpp   
module.obj : module.cpp browseh.h mydisp.h tlb.h
     $(CC) module.cpp
coclass.obj : coclass.cpp browseh.h mydisp.h tlb.h
     $(CC) coclass.cpp    
function.obj : function.cpp browseh.h mydisp.h tlb.h
     $(CC) function.cpp
property.obj : property.cpp browseh.h mydisp.h tlb.h
     $(CC) property.cpp
param.obj : param.cpp browseh.h mydisp.h tlb.h
     $(CC) param.cpp   
collect.obj : collect.cpp browseh.h mydisp.h tlb.h
     $(CC) collect.cpp
enumvar.obj : enumvar.cpp browseh.h mydisp.h tlb.h
     $(CC) enumvar.cpp     
enum.obj : enum.cpp browseh.h mydisp.h tlb.h
     $(CC) enum.cpp 
constant.obj : constant.cpp browseh.h mydisp.h tlb.h
     $(CC) constant.cpp   
alias.obj : alias.cpp browseh.h mydisp.h tlb.h
     $(CC) alias.cpp      
struct.obj : struct.cpp browseh.h mydisp.h tlb.h
     $(CC) struct.cpp 
union.obj : union.cpp browseh.h mydisp.h tlb.h
     $(CC) union.cpp
type.obj : type.cpp browseh.h mydisp.h tlb.h
     $(CC) type.cpp

