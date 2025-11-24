# Makefile to build the Cample CMC Client

!include <win32.mak>

PROJ = CMCCLI32

# Main target dependencies

all: $(PROJ).exe

# Extra libraries needed, not defined in win32.mak

extralibs = mapi32.lib ole32.lib ..\common\ctl3d32.lib

# Object files we need tp build

OBJS = CMCCLI.OBJ PVALLOC.OBJ

# Build the object files

.c.obj:
  $(cc) -I..\common $(cdebug) $(cflags) $(cvars) $*.c

pvalloc.obj: ..\common\pvalloc.c
  $(cc) -I..\common $(cdebug) $(cflags) $(cvars) ..\common\pvalloc.c

# Build the resources

$(PROJ).res: cmccli.rc cmccli.ico cmccli2.ico read.bmp unread.bmp reada.bmp unreada.bmp
  $(rc) /I..\common $(rcflags) $(rcvars) /fo $(PROJ).res CMCCLI.RC

# Link it together to make the executable image

$(PROJ).exe: $(OBJS) $(PROJ).res $(COMMON_OBJS)
  $(link) $(linkdebug) $(lflags) -subsystem:windows,$(APPVER) $(OBJS) \
  $(COMMON_OBJS) $(PROJ).res $(guilibsdll) $(extralibs) -out:$(PROJ).exe

# Clean up the intermediate files

clean:
  del *.obj *.res

