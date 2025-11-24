# Makefile to build the Checkers Sample Form

!include <win32.mak>

PROJ = WCHECK

# Main target dependencies

all: $(PROJ).exe

# Extra libraries needed, not defined in win32.mak

extralibs = mapi32.lib ole32.lib ..\common\ctl3d32.lib version.lib uuid.lib

# Object files we need to build

OBJS = wcheck.obj engine.obj form.obj

# Build the object files

.cpp.obj:
  $(cc) /I..\common $(cdebug) $(cflags) $(cvars) $*.cpp

engine.obj: engine\lut.cpp engine\debugio.cpp engine\quality.cpp \
		    engine\prune.cpp engine\check.cpp engine.cpp check.h
  $(cc) /I..\common $(cdebug) $(cflags) $(cvars) engine.cpp

# Build the resources

$(PROJ).res: $(PROJ).RC $(PROJ).DLG
  $(rc) $(rcflags) $(rcvars) /fo $(PROJ).res $(PROJ).RC

# Link it together to make the executable image

$(PROJ).exe: $(OBJS) $(PROJ).res $(COMMON_OBJS)
  $(link) $(linkdebug) $(lflags) -subsystem:windows,$(APPVER) $(OBJS) \
  $(COMMON_OBJS) $(PROJ).res $(guilibs) $(extralibs) -out:$(PROJ).exe

# Clean up the intermediate files

clean:
  del *.obj *.res

