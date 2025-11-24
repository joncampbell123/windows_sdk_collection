# Makefile to build the MAPI Sample Hook Provider

!include <win32.mak>

PROJ     = SMH32
BASEFILE = ..\common\mapibase.txt

# Main target dependencies

all: $(PROJ).dll

# Extra libraries needed, not defined in win32.mak

extralibs = mapi32.lib ole32.lib ..\common\ctl3d32.lib winmm.lib uuid.lib comctl32.lib

# Object files we need to build

OBJS = smh.obj smhcfg.obj smhmdb.obj smhoof.obj smhps.obj smhwiz.obj smhguid.obj

# Bitmap files we use

BITMAPS = down.bmp downdis.bmp downinv.bmp updis.bmp upinv.bmp up.bmp

# Build the object files

.c.obj:
  $(cc) /I..\common $(cdebug) $(cflags) $(cvars) $*.c

# Build the resources

$(PROJ).res: SMH.RC SMH.DLG SMH.ICO $(BITMAPS)
  $(rc) /I..\common $(rcflags) $(rcvars) /fo $(PROJ).res SMH.RC

# Link it together to make the executable image

$(PROJ).dll: $(OBJS) $(PROJ).res $(COMMON_OBJS)
  $(link) $(linkdebug) $(lflags) -dll -subsystem:windows,$(APPVER) $(OBJS) \
  $(COMMON_OBJS) $(PROJ).res $(guilibsdll) $(extralibs) -def:smh32.def -out:$(PROJ).dll \
  -base:@$(BASEFILE),$(PROJ)

# Clean up the intermediate files

clean:
  del *.obj *.res

