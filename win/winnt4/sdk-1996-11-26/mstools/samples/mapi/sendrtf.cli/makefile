# Makefile to build the MAPI sendrtf sample
# Copyright 1986-1996, Microsoft Corporation. All Rights Reserved.

#REM Key to CPU specific conditions, CPU == i386 || ALPHA || MIPS || PPC     1) cvtres
# 	cvtres -$(CPU) ...
!IF "$(CPU)" == ""
!IF "$(PROCESSOR_ARCHITECTURE)" == "MIPS" || "$(PROCESSOR_ARCHITECTURE)" == "ALPHA" || "$(PROCESSOR_ARCHITECTURE)" == "PPC"
CPU = $(PROCESSOR_ARCHITECTURE)
!ELSE
CPU = i386
!ENDIF
!ENDIF

!include <win32.mak>

!IFDEF MAPISAMP
HOME   = $(MAPISAMP)\sendrtf.cli^\
COMMON = $(MAPISAMP)\common^\
rcvars = $(rcvars) /I$(HOME)
!ELSE
HOME   = 
COMMON = ..\common^\
!ENDIF

!IFNDEF NODEBUG
cflags = $(cflags) -DDEBUG
!ENDIF

PROJ     = sndrtf32

# Main target dependencies

all: $(PROJ).exe

# Extra libraries need, not defined in win32.mak

extralibs = mapi32.lib ole32.lib uuid.lib

# Object files we need to build

OBJS = sendrtf.obj 

COMMON_OBJS = cindex.obj
!IFNDEF NODEBUG
COMMON_OBJS = $(COMMON_OBJS) mapidbg.obj
!ENDIF

# Build the object files

{$(HOME)}.c.obj:
  $(cc) /I$(COMMON) $(cdebug) $(cflags)  $(cvarsmt) $**

{$(HOME)}.cpp.obj:
  $(cc) /I$(COMMON) $(cdebug) $(cflags)  $(cvarsmt) $**

wrap3d.obj: $(COMMON)wrap3d.c
  $(cc) /I$(COMMON) $(cdebug) $(cflags) $(cvarsmt) $**

mapidbg.obj: $(COMMON)mapidbg.c
  $(cc) /I$(COMMON) $(cdebug) $(cflags) $(cvarsmt) $**

cindex.obj: $(COMMON)cindex.c
  $(cc) /I$(COMMON) $(cdebug) $(cflags) $(cvarsmt) $**


# Link it together to make the executable image

$(PROJ).exe: $(OBJS) $(COMMON_OBJS) 
  $(link) $(linkdebug) $(lflags) -subsystem:console,$(APPVER) $(OBJS) \
  $(COMMON_OBJS)  $(guilibsmt) $(extralibs) -out:$(PROJ).exe
!IFDEF MAPISAMP
  -copy $@ $(MAPISAMP)\bin\$(PLATFORM)
!ENDIF

# Clean up the intermediate files

clean:
  -del *.obj
  -del *.res 
  -del *.lib 
  -del *.exe 
  -del *.exp

