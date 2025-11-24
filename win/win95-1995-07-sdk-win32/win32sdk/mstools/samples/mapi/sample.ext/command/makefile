# Makefile to build the Sample Extension

!include <win32.mak>

PROJ = CMDEXT32

# Main target dependencies

all: $(PROJ).dll

# Extra libraries needed, not defined in win32.mak

extralibs = mapi32.lib ole32.lib uuid.lib

# Object files we need to build

OBJS = CMDEXT.OBJ

# Build the object files

.cpp.obj:
  $(cc) $(cdebug) $(cflags) $(cvars) $*.cpp

# Build the resources

$(PROJ).res: CMDEXT.RC
  $(rc) /I..\..\common $(rcflags) $(rcvars) /fo $(PROJ).res CMDEXT.RC

# Link it together to make the executable image

$(PROJ).dll: $(OBJS) $(PROJ).res $(COMMON_OBJS)
  $(link) $(linkdebug) $(lflags) -dll -subsystem:windows,$(APPVER) $(OBJS) \
  $(COMMON_OBJS) $(PROJ).res $(guilibsdll) $(extralibs) -def:cmdext.def -out:$(PROJ).dll

# Clean up the intermediate files

clean:
  del *.obj *.res

