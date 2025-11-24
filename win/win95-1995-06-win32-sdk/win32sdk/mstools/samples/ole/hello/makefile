####
#makefile - makefile for hello.exe
#
#Purpose:
#  Builds the OLE 2.0 Automation object, hello.exe.
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
CFLAGS = -c -W3 -AM -GA -GEs -DWIN16
LINKFLAGS = /NOD /NOI /BATCH /ONERROR:NOEXE

LIBS = libw.lib mlibcew.lib

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
LINKFLAGS = $(linkdebug) $(guilflags)
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

APPS = hello


!if "$(TARGET)" == "WIN16"
LIBS = ole2.lib compobj.lib ole2disp.lib typelib.lib $(LIBS)
!endif
!if "$(TARGET)" == "WIN32"
LIBS = $(ole2libsmt)
!endif

OBJS = main.obj hello.obj hellocf.obj   


##########################################################################
#
# Default Goal
#

goal : setflags $(APPS).exe

setflags :
    set CL=$(CFLAGS)


##########################################################################
#
# Clean (erase) generated files
#
clean :
    if exist *.obj       del *.obj
    if exist $(APPS).exe del $(APPS).exe
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
$(APPS).exe : $(OBJS) $(APPS).def $(APPS).res $(APPS).ico
    link $(LINKFLAGS) @<<
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
$(APPS).exe : $(OBJS) $(APPS).def $(APPS).res $(APPS).ico
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

tlb.h : hello.odl
     if exist tlb.h  del tlb.h
     if exist hello.tlb  del hello.tlb
     $(WX) mktyplib /D$(TARGET) /h tlb.h /o hello.log /tlb hello.tlb hello.odl
     type hello.log
     
main.obj : main.cpp hello.h tlb.h
     $(CC) main.cpp     
hello.obj : hello.cpp hello.h tlb.h
     $(CC) hello.cpp
hellocf.obj : hellocf.cpp hello.h tlb.h
     $(CC) hellocf.cpp
