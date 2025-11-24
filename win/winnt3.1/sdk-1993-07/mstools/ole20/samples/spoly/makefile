##############################################################################
#
#  (c) Copyright Microsoft Corp. 1992-1993 All Rights Reserved
#
#  File:
#
#    makefile - makefile for spoly.exe
#
#  Purpose:
#
#    Builds the OLE 2.0 sample IDispatch server, spoly.exe.
#
#
#  Usage:
#
#     NMAKE                     ; build with defaults
#     or: NMAKE option		; build with the given option(s)
#     or: NMAKE clean		; erase all compiled files
#
#     option: dev = [win16 | win32]    ; dev=win16 is the default
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

CFLAGS = $(CFLAGS) -Od -Zi -D_DEBUG $(CL)
LINKFLAGS = $(LINKFLAGS) /COD

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
CFLAGS = $(CFLAGS) -DOLE2SHIP -DOLE2FINAL -D_INC_OLE -D__RPC_H__ -D__RPCDCE_H__ -D_X86_ -DNTBETA2 -D_MT
LINKFLAGS = -subsystem:windows -entry:WinMainCRTStartup -machine:$(MACHINE) 

LIBS = libc.lib kernel32.lib user32.lib gdi32.lib 

CFLAGS = $(CFLAGS) -Od -Zi -D_DEBUG $(CL)
LINKFLAGS = -debug:full -debugtype:cv $(LINKFLAGS) 

!endif


##########################################################################
#
# Application Settings
#

!if "$(TARGET)" == "WIN16"
LIBS = ole2.lib compobj.lib ole2disp.lib ole2nls.lib $(LIBS)
!else 
!if "$(TARGET)" == "WIN32"
LIBS = ole2w32.lib ole2di32.lib $(LIBS)
!endif
!endif

OBJS = \
	winmain.obj	\
	cpoly.obj	\
	cpoint.obj	\
	cenumpt.obj	\
	statbar.obj	\
	clsid.obj	\
	misc.obj


goal : setflags spoly.exe

setflags :
	set CL=$(CFLAGS)

clean :
    if exist *.obj       del *.obj
    if exist spoly.exe del spoly.exe
    if exist spoly.map del spoly.map
    if exist spoly.res del spoly.res
    if exist spoly.rs  del spoly.rs
    if exist *.pdb       del *.pdb


##########################################################################
#
# Application Build (WIN16 Specific)
#

!if "$(TARGET)" == "WIN16"

spoly.exe : $(OBJS) spoly.def spoly.res spoly.ico
	link $(LINKFLAGS) @<<
$(OBJS),
$@,,
$(LIBS),
spoly.def
<<
	rc -k -t spoly.res $@
!endif


##########################################################################
#
# Application Build (WIN32 Specific)
#
!if "$(TARGET)" == "WIN32"

spoly.exe : $(OBJS) spoly.def spoly.res spoly.ico
      cvtres -$(MACHINE) spoly.res -o spoly.rs
      $(LINK) @<< 
        $(LINKFLAGS)
        -out:$@ 
        -map:$*.map
        $(OBJS)
        spoly.rs
        $(LIBS)
<<
!endif


spoly.res : spoly.rc resource.h
	rc $(RCFLAGS) -r -fo$@ spoly.rc

winmain.obj: winmain.cpp hostenv.h resource.h spoly.h cpoint.h cpoly.h statbar.h
	$(CC) -c winmain.cpp

cpoint.obj: cpoint.cpp cpoint.h hostenv.h spoly.h statbar.h
	$(CC) -c cpoint.cpp

cpoly.obj: cpoly.cpp cpoint.h cpoly.h hostenv.h spoly.h statbar.h
	$(CC) -c cpoly.cpp

clsid.obj: clsid.c clsid.h
	$(CC) -c clsid.c

cenumpt.obj: cenumpt.cpp cenumpt.h
	$(CC) -c cenumpt.cpp

statbar.obj: statbar.cpp statbar.h
	$(CC) -c statbar.cpp

misc.obj: misc.cpp hostenv.h spoly.h
	$(CC) -c misc.cpp
