##############################################################################
#
#  (c) Copyright Microsoft Corp. 1992-1993 All Rights Reserved
#
#  File:
#
#    makefile - makefile for spoly2.exe
#
#  Purpose:
#
#    Builds the OLE 2.0 sample IDispatch server, dispcalc.exe.
#
#
#  Usage:
#
#     NMAKE                     ; build with defaults
#     or: NMAKE option		; build with the given option(s)
#     or: NMAKE clean		; erase all compiled files
#
#     option: dev = [win16 | win32]    ; dev=win16 is the default
#             DEBUG=[0|1]              ; DEBUG=1 is the default
#
#
#  Notes:
#
#    This makefile assumes that the PATH, INCLUDE and LIB environment
#    variables are setup properly.
#
##############################################################################



##########################################################################
#
# Default Settings
#

!if "$(dev)" == ""
dev = win32
!endif

!if !("$(dev)" == "win16" || "$(dev)" == "win32" || "$(dev)" == "mac")
!error Invalid dev option, choose from [win16 | win32 | mac]
!endif

!if "$(dev)" == "win16"
TARGET  = WIN16
!endif

!if "$(dev)" == "win32"
TARGET  = WIN32
MACHINE = i386
!endif

!if "$(dev)" == "mac"
!error Mac build is currently not supported
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

RCFLAGS = -dWIN16
CFLAGS = -W3 -AM -GA -GEs -DWIN16
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

CC   = cl386
LINK = link32

RCFLAGS = -dWIN32
CFLAGS = -W3 -G3 -nologo -D$(MACHINE)=1 -DWIN32 -D_NTWIN -D_WINDOWS $(CL)
CFLAGS = $(CFLAGS) -DOLE2SHIP -DOLE2FINAL -D_INC_OLE  -D__RPC_H__ -D__RPCDCE_H__ -D_X86_ -DNTBETA2 -D_MT
LINKFLAGS = -subsystem:windows -entry:WinMainCRTStartup -machine:$(MACHINE) 

LIBS = libc.lib kernel32.lib user32.lib

!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS) -Od -Zi -D_DEBUG $(CL)
LINKFLAGS = -debug:full -debugtype:cv $(LINKFLAGS) 
!else
CFLAGS = $(CFLAGS) -Ox
!endif
!endif


##########################################################################
#
# Build rules
#

.cpp.obj:
    @echo Compiling $<...
    $(CC) -c $<

.c.obj:
    @echo Compiling $<...
    $(CC) -c $<


##########################################################################
#
# Application Settings
#

APPS = dispcalc


!if "$(TARGET)" == "WIN16"
LIBS = ole2.lib compobj.lib ole2disp.lib $(LIBS)
!else 
!if "$(TARGET)" == "WIN32"
LIBS = ole2w32.lib ole2di32.lib $(LIBS)
!endif
!endif

OBJS = \
	main.obj	\
	idata.obj	\
	dispcalc.obj	\
	clsid.obj


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
    if exist $(APPS).map del $(APPS).map
    if exist $(APPS).res del $(APPS).res
    if exist $(APPS).rs  del $(APPS).rs


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
      cvtres -$(MACHINE) $(APPS).res -o $(APPS).rs
      $(LINK) @<< 
        $(LINKFLAGS)
        -out:$@ 
        -map:$*.map
        $(OBJS)
        $(APPS).rs
        $(LIBS)
<<
!endif


##########################################################################
#
# Application Build (Common)
#

$(APPS).res : $(APPS).rc
	rc  $(RCFLAGS) -r -fo$@ $?


##########################################################################
#
# Dependencies
#

main.obj: main.cpp dispcalc.h
    $(CC) -c main.cpp

idata.obj: idata.cpp dispcalc.h
    $(CC) -c idata.cpp

dispcalc.obj: dispcalc.cpp dispcalc.h
    $(CC) -c dispcalc.cpp

clsid.obj: clsid.c clsid.h
    $(CC) -c clsid.c
