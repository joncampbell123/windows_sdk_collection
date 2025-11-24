# Makefile to build the MAPI Sample Address Book Provider

!include <win32.mak>

PROJ     = SMPAB32
BASEFILE = ..\common\mapibase.txt

# Main target dependencies

all: $(PROJ).dll

# Extra libraries need, not defined in win32.mak

extralibs = mapi32.lib ole32.lib ..\common\ctl3d32.lib

# Object files we need to build

OBJS = abcont.obj abctbl1.obj abctbl2.obj abctbl3.obj abguid.obj ablogon.obj abp.obj \
       absearch.obj abuser.obj ootid.obj oouser.obj root.obj status.obj tid.obj wrap.obj

# Build the object files

.c.obj:
  $(cc) /I..\common $(cdebug) $(cflags) $(cvars) $*.c

# Build the resources

$(PROJ).res: SAMPABP.RC SAMPABP.DLG
  $(rc) /I..\common $(rcflags) $(rcvars) /fo $(PROJ).res SAMPABP.RC

# Link it together to make the executable image

$(PROJ).dll: $(OBJS) $(PROJ).res $(COMMON_OBJS)
  $(link) $(linkdebug) $(lflags) -dll -subsystem:windows,$(APPVER) $(OBJS) \
  $(COMMON_OBJS) $(PROJ).res $(guilibsdll) $(extralibs) -def:$(PROJ).def -out:$(PROJ).dll \
  -base:@$(BASEFILE),$(PROJ)

# Clean up the intermediate files

clean:
  del *.obj *.res *.lib

